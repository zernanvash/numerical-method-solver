#include "GraphWidget.h"
#include <QPainter>
#include <QPen>
#include <QResizeEvent>
#include <cmath>
#include <algorithm>

// Bayer 2x2 threshold matrix (values 0–3, divide by 4)
static const int BAYER2[2][2] = {{0,2},{3,1}};

static double niceTickStep(double span, int targetTicks = 8) {
    if (!std::isfinite(span) || span <= 0.0) return 1.0;

    const double raw = span / std::max(1, targetTicks);
    const double mag = std::pow(10.0, std::floor(std::log10(raw)));
    const double norm = raw / mag;

    if (norm <= 1.0) return mag;
    if (norm <= 2.0) return 2.0 * mag;
    if (norm <= 5.0) return 5.0 * mag;
    return 10.0 * mag;
}

static QString tickLabel(double value) {
    const double absValue = std::abs(value);
    if (absValue >= 1000.0 || (absValue > 0.0 && absValue < 0.01)) {
        return QString::number(value, 'g', 3);
    }
    if (std::abs(value - std::round(value)) < 1e-8) {
        return QString::number(static_cast<int>(std::round(value)));
    }
    return QString::number(value, 'f', 2);
}

GraphWidget::GraphWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(300, 220);
    setAttribute(Qt::WA_OpaquePaintEvent);

    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(100); // chunky ~10 fps CRT beam
    connect(m_scanTimer, &QTimer::timeout, this, &GraphWidget::onScanTick);
}

// ── Public API ────────────────────────────────────────────────────────────────

void GraphWidget::setFunction(std::function<double(double)> f, const std::string& expr) {
    m_func = std::move(f);
    m_expr = expr;
    m_graphMode = GraphMode::RootFinding;
    m_buffersReady = false;
}

void GraphWidget::setIterations(const std::vector<IterationRecord>& recs, SolverMethod method) {
    m_iters  = recs;
    m_method = method;
    m_graphMode = GraphMode::RootFinding;
    m_showUpTo = 0;
    m_buffersReady = false;
    autoRange();
}

void GraphWidget::setOdeIterations(const std::vector<OdeIterationRecord>& recs, const std::string& expr, const std::string& method) {
    m_odeIters = recs;
    m_expr = expr;
    m_odeMethod = method;
    m_graphMode = GraphMode::OdeSimulation;
    m_func = nullptr;
    m_showUpTo = 0;
    m_buffersReady = false;
    autoRangeOde();
}

void GraphWidget::startReveal() {
    if (!m_func && m_graphMode != GraphMode::OdeSimulation) return;
    m_scanY      = 0.0;
    m_scanning   = true;
    m_revealDone = false;
    m_buffersReady = false;
    rebuildGraphBuffer();
    m_scanTimer->start();
    update();
}

void GraphWidget::reset() {
    m_scanTimer->stop();
    m_scanning   = false;
    m_revealDone = false;
    m_scanY      = 0.0;
    m_showUpTo   = 0;
    m_func = nullptr;
    m_iters.clear();
    m_odeIters.clear();
    m_graphMode = GraphMode::RootFinding;
    m_buffersReady = false;
    update();
}

void GraphWidget::showUpToIteration(int n) {
    m_showUpTo = n;
    m_buffersReady = false;
    startReveal();
}

void GraphWidget::setAnimationSpeed(double speed) {
    m_scanSpeed = std::clamp(speed, 0.5, 20.0);
}

void GraphWidget::setGraphEffects(bool scanlines, bool dither, bool vignette) {
    m_enableGraphScanlines = scanlines;
    m_enableDither = dither;
    m_enableVignette = vignette;
    m_buffersReady = false;
    update();
}

void GraphWidget::setThemeColors(const QColor& background, const QColor& primary, const QColor& dim, const QColor& accent) {
    m_bg = background;
    m_curve = primary;
    m_scan = accent;
    m_root = accent;
    m_dim = dim;
    m_grid = QColor(primary.red(), primary.green(), primary.blue(), 45);
    m_axis = QColor(primary.red(), primary.green(), primary.blue(), 130);
    m_bisect = QColor(primary.red(), primary.green(), primary.blue(), 170);
    m_tangent = QColor(accent.red(), accent.green(), accent.blue(), 210);
    m_secant = QColor(primary.red(), primary.green(), primary.blue(), 180);
    m_buffersReady = false;
    update();
}

// ── Coordinate helpers ────────────────────────────────────────────────────────

int GraphWidget::toScreenX(double wx) const {
    return static_cast<int>((wx - m_xMin) / (m_xMax - m_xMin) * width());
}
int GraphWidget::toScreenY(double wy) const {
    return static_cast<int>(height() - (wy - m_yMin) / (m_yMax - m_yMin) * height());
}
double GraphWidget::toWorldX(int sx) const {
    return m_xMin + static_cast<double>(sx) / width() * (m_xMax - m_xMin);
}
double GraphWidget::toWorldY(int sy) const {
    return m_yMin + static_cast<double>(height() - sy) / height() * (m_yMax - m_yMin);
}

void GraphWidget::autoRange() {
    if (m_iters.empty() || !m_func) {
        m_xMin = -1.0; m_xMax = 4.0;
        m_yMin = -3.0; m_yMax = 5.0;
        return;
    }
    // Gather all x values
    double xlo = m_iters[0].a, xhi = m_iters[0].b;
    for (auto& r : m_iters) {
        xlo = std::min({xlo, r.a, r.b, r.x});
        xhi = std::max({xhi, r.a, r.b, r.x});
    }
    double xpad = std::max(1.0, (xhi - xlo) * 0.4);
    m_xMin = xlo - xpad;
    m_xMax = xhi + xpad;

    // Compute y range from function samples
    double ylo = 1e18, yhi = -1e18;
    int samples = 300;
    for (int i = 0; i <= samples; ++i) {
        double wx = m_xMin + (m_xMax - m_xMin) * i / samples;
        try {
            double wy = m_func(wx);
            if (std::isfinite(wy)) {
                ylo = std::min(ylo, wy);
                yhi = std::max(yhi, wy);
            }
        } catch (...) {}
    }
    if (ylo >= yhi) { ylo = -3.0; yhi = 5.0; }
    double ypad = std::max(1.0, (yhi - ylo) * 0.3);
    m_yMin = ylo - ypad;
    m_yMax = yhi + ypad;
}

void GraphWidget::autoRangeOde() {
    if (m_odeIters.empty()) {
        m_xMin = -1.0; m_xMax = 4.0;
        m_yMin = -3.0; m_yMax = 5.0;
        return;
    }

    double tlo = m_odeIters.front().t;
    double thi = m_odeIters.front().nextT;
    double ylo = m_odeIters.front().y;
    double yhi = m_odeIters.front().nextY;

    for (const auto& r : m_odeIters) {
        tlo = std::min({tlo, r.t, r.nextT});
        thi = std::max({thi, r.t, r.nextT});
        ylo = std::min({ylo, r.y, r.nextY});
        yhi = std::max({yhi, r.y, r.nextY});
    }

    double tpad = std::max(0.5, (thi - tlo) * 0.12);
    double ypad = std::max(1.0, (yhi - ylo) * 0.18);
    m_xMin = tlo - tpad;
    m_xMax = thi + tpad;
    m_yMin = ylo - ypad;
    m_yMax = yhi + ypad;
}

// ── Scanline timer ────────────────────────────────────────────────────────────

void GraphWidget::onScanTick() {
    m_scanY += std::max(12.0, m_scanSpeed * 6.0);
    if (m_scanY >= height()) {
        m_scanY      = height();
        m_revealDone = true;
        m_scanning   = false;
        m_scanTimer->stop();
    }
    update();
}

// ── Buffer building ───────────────────────────────────────────────────────────

void GraphWidget::rebuildGraphBuffer() {
    if (width() <= 0 || height() <= 0) return;

    m_graphBuffer = QImage(width(), height(), QImage::Format_RGB32);
    m_graphBuffer.fill(m_bg);

    QPainter p(&m_graphBuffer);
    p.setRenderHint(QPainter::Antialiasing, false); // pixel-crisp

    drawGrid(p);
    drawAxes(p);

    if (m_graphMode == GraphMode::OdeSimulation) {
        int n = m_showUpTo > 0 ? m_showUpTo : static_cast<int>(m_odeIters.size());
        drawOdePath(p, n);
    } else if (m_func) {
        drawCurve(p);

        int n = m_showUpTo > 0 ? m_showUpTo : static_cast<int>(m_iters.size());

        for (int i = 1; i <= n; ++i) {
            switch (m_method) {
                case SolverMethod::Bisection: drawBisectionStep(p, i); break;
                case SolverMethod::Newton:    drawNewtonStep(p, i);    break;
                case SolverMethod::Secant:    drawSecantStep(p, i);    break;
            }
        }
        if (n > 0) drawRootMarker(p, n);
    }

    // Draw axis tick labels (pixelated monospace via QPainter)
    QFont mono("Fixedsys", 7);
    mono.setStyleHint(QFont::TypeWriter);
    p.setFont(mono);
    p.setPen(m_dim);

    // X-axis labels
    const double xStep = niceTickStep(m_xMax - m_xMin);
    const double xStart = std::ceil(m_xMin / xStep) * xStep;
    for (double wx = xStart; wx <= m_xMax + xStep * 0.5; wx += xStep) {
        int sx = toScreenX(wx);
        int sy = toScreenY(0.0);
        p.drawText(sx - 12, std::clamp(sy + 12, 10, height() - 4), tickLabel(wx));
    }
    // Y-axis labels
    const double yStep = niceTickStep(m_yMax - m_yMin);
    const double yStart = std::ceil(m_yMin / yStep) * yStep;
    for (double wy = yStart; wy <= m_yMax + yStep * 0.5; wy += yStep) {
        if (std::abs(wy) < 1e-8) continue;
        int sx = toScreenX(0.0);
        int sy = toScreenY(wy);
        p.drawText(std::clamp(sx + 3, 0, width() - 42), sy + 4, tickLabel(wy));
    }

    p.end();

    // CRT post-processing passes
    if (m_enableGraphScanlines) applyScanlineRows(m_graphBuffer);
    if (m_enableDither) applyDither(m_graphBuffer);
    if (m_enableVignette) applyVignette(m_graphBuffer);

    m_buffersReady = true;
}

// ── Drawing primitives ────────────────────────────────────────────────────────

void GraphWidget::drawGrid(QPainter& p) {
    p.setPen(QPen(m_grid, 1));
    for (int wx = static_cast<int>(std::ceil(m_xMin)); wx <= static_cast<int>(m_xMax); ++wx) {
        int sx = toScreenX(wx);
        p.drawLine(sx, 0, sx, height());
    }
    for (int wy = static_cast<int>(std::ceil(m_yMin)); wy <= static_cast<int>(m_yMax); ++wy) {
        int sy = toScreenY(wy);
        p.drawLine(0, sy, width(), sy);
    }
}

void GraphWidget::drawAxes(QPainter& p) {
    p.setPen(QPen(m_axis, 1));
    // X axis
    int sy0 = toScreenY(0.0);
    if (sy0 >= 0 && sy0 < height()) p.drawLine(0, sy0, width(), sy0);
    // Y axis
    int sx0 = toScreenX(0.0);
    if (sx0 >= 0 && sx0 < width()) p.drawLine(sx0, 0, sx0, height());
}

void GraphWidget::drawCurve(QPainter& p) {
    p.setPen(QPen(m_curve, 2));
    bool first = true;
    QPoint prev;
    for (int sx = 0; sx < width(); ++sx) {
        double wx = toWorldX(sx);
        double wy = 0.0;
        try { wy = m_func(wx); }
        catch (...) { first = true; continue; }
        if (!std::isfinite(wy) || wy < m_yMin - 2.0 || wy > m_yMax + 2.0)
            { first = true; continue; }
        QPoint cur(sx, toScreenY(wy));
        if (!first) p.drawLine(prev, cur);
        prev  = cur;
        first = false;
    }
}

void GraphWidget::drawBisectionStep(QPainter& p, int n) {
    if (n < 1 || n > static_cast<int>(m_iters.size())) return;
    const auto& rec = m_iters[n - 1];
    double alpha = 0.15 + 0.55 * (static_cast<double>(n) / m_iters.size());

    // Shaded interval
    QColor shade = m_bisect;
    shade.setAlphaF(alpha * 0.25);
    int sx_a = toScreenX(rec.a);
    int sx_b = toScreenX(rec.b);
    p.fillRect(QRect(sx_a, 0, sx_b - sx_a, height()), shade);

    // Endpoint markers (dashed)
    QPen ep(m_bisect, 1, Qt::DashLine);
    ep.setColor(QColor(m_bisect.red(), m_bisect.green(), m_bisect.blue(),
                       static_cast<int>(alpha * 200)));
    p.setPen(ep);
    p.drawLine(sx_a, 0, sx_a, height());
    p.drawLine(sx_b, 0, sx_b, height());

    // Midpoint
    QColor mc = m_curve;
    mc.setAlphaF(alpha);
    p.setPen(QPen(mc, 2));
    int sx_c = toScreenX(rec.x);
    p.drawLine(sx_c, 0, sx_c, height());

    // Dot on curve at midpoint
    int sy_c = toScreenY(rec.fx);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255,176,0, static_cast<int>(alpha*255)));
    p.drawEllipse(QPoint(sx_c, sy_c), 3, 3);
    p.setBrush(Qt::NoBrush);
}

void GraphWidget::drawNewtonStep(QPainter& p, int n) {
    if (n < 1 || n > static_cast<int>(m_iters.size())) return;
    const auto& rec = m_iters[n - 1];
    double alpha = 0.2 + 0.6 * (static_cast<double>(n) / m_iters.size());

    double xp = rec.a; // previous x
    double fx  = m_func(xp);
    double dfx = rec.tangentSlope;

    // Tangent line: y - fx = dfx*(x - xp)  =>  y = dfx*(x - xp) + fx
    auto tangent = [&](double x){ return dfx * (x - xp) + fx; };

    QColor tc(m_tangent.red(), m_tangent.green(), m_tangent.blue(),
              static_cast<int>(alpha * 200));
    QPen tp(tc, 1, Qt::DashLine);
    p.setPen(tp);

    QPoint pA(0,               toScreenY(tangent(toWorldX(0))));
    QPoint pB(width() - 1,     toScreenY(tangent(toWorldX(width() - 1))));
    p.drawLine(pA, pB);

    // Vertical drop from x_{n-1} on curve to x-axis
    p.setPen(QPen(tc, 1, Qt::DotLine));
    p.drawLine(toScreenX(xp), toScreenY(fx), toScreenX(xp), toScreenY(0.0));

    // Dot at x_{n-1} on curve
    p.setPen(Qt::NoPen);
    p.setBrush(tc);
    p.drawEllipse(QPoint(toScreenX(xp), toScreenY(fx)), 3, 3);
    p.setBrush(Qt::NoBrush);
}

void GraphWidget::drawSecantStep(QPainter& p, int n) {
    if (n < 1 || n > static_cast<int>(m_iters.size())) return;
    const auto& rec = m_iters[n - 1];
    double alpha = 0.2 + 0.6 * (static_cast<double>(n) / m_iters.size());

    double x0 = rec.a, x1 = rec.b;
    double f0 = m_func(x0), f1 = m_func(x1);
    double slope = (std::abs(x1 - x0) > 1e-12) ? (f1 - f0) / (x1 - x0) : 0.0;
    auto chord = [&](double x){ return f0 + slope * (x - x0); };

    QColor sc(m_secant.red(), m_secant.green(), m_secant.blue(),
              static_cast<int>(alpha * 200));
    QPen sp(sc, 1, Qt::DashLine);
    p.setPen(sp);
    p.drawLine(0,         toScreenY(chord(toWorldX(0))),
               width()-1, toScreenY(chord(toWorldX(width()-1))));

    // Dots at x0 and x1
    p.setPen(Qt::NoPen);
    p.setBrush(sc);
    p.drawEllipse(QPoint(toScreenX(x0), toScreenY(f0)), 3, 3);
    p.drawEllipse(QPoint(toScreenX(x1), toScreenY(f1)), 3, 3);
    p.setBrush(Qt::NoBrush);
}

void GraphWidget::drawRootMarker(QPainter& p, int n) {
    if (n < 1 || n > static_cast<int>(m_iters.size())) return;
    const auto& rec = m_iters[n - 1];
    int sx = toScreenX(rec.x);
    int sy = toScreenY(rec.fx);
    int sy0 = toScreenY(0.0);

    // Glow rings on curve
    for (int r = 8; r >= 2; r -= 2) {
        int alpha = static_cast<int>(180.0 * (1.0 - r / 10.0));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(m_root.red(), m_root.green(), m_root.blue(), alpha));
        p.drawEllipse(QPoint(sx, sy), r, r);
    }
    // Solid centre
    p.setBrush(m_root);
    p.drawEllipse(QPoint(sx, sy), 3, 3);

    // Root on x-axis
    p.setBrush(QColor(255,200,0));
    p.drawEllipse(QPoint(sx, sy0), 4, 4);
    p.setBrush(Qt::NoBrush);

    // Label
    QFont mono("Fixedsys", 8);
    mono.setBold(true);
    p.setFont(mono);
    p.setPen(m_root);
    p.drawText(sx + 6, sy0 - 5, QString("x≈%1").arg(rec.x, 0, 'f', 5));
}

void GraphWidget::drawOdePath(QPainter& p, int n) {
    if (m_odeIters.empty()) return;

    n = std::clamp(n, 1, static_cast<int>(m_odeIters.size()));

    p.setPen(QPen(m_curve, 2));
    QPoint startPoint(toScreenX(m_odeIters.front().t), toScreenY(m_odeIters.front().y));

    p.setBrush(QColor(m_curve.red(), m_curve.green(), m_curve.blue(), 180));
    p.drawEllipse(startPoint, 3, 3);

    for (int i = 0; i < n; ++i) {
        const auto& rec = m_odeIters[i];
        QPoint from(toScreenX(rec.t), toScreenY(rec.y));
        QPoint to(toScreenX(rec.nextT), toScreenY(rec.nextY));

        double alpha = 0.25 + 0.65 * (static_cast<double>(i + 1) / n);
        QColor pathColor(m_curve.red(), m_curve.green(), m_curve.blue(), static_cast<int>(alpha * 255));
        p.setPen(QPen(pathColor, 2));
        p.drawLine(from, to);

        p.setPen(Qt::NoPen);
        p.setBrush(pathColor);
        p.drawEllipse(to, 3, 3);
    }

    const auto& last = m_odeIters[n - 1];
    QPoint finalPoint(toScreenX(last.nextT), toScreenY(last.nextY));
    p.setBrush(m_root);
    p.setPen(Qt::NoPen);
    p.drawEllipse(finalPoint, 5, 5);

    QFont mono("Fixedsys", 8);
    mono.setBold(true);
    p.setFont(mono);
    p.setPen(m_root);
    p.drawText(finalPoint.x() + 6, finalPoint.y() - 5,
               QString("y=%1").arg(last.nextY, 0, 'f', 4));
    p.setPen(m_dim);
    p.drawText(8, 16, QString::fromStdString(m_odeMethod + "  t vs y"));
}

// ── CRT post-process passes ───────────────────────────────────────────────────

void GraphWidget::applyDither(QImage& img) {
    // Ordered Bayer 2x2 dither: reduce colour precision to simulate low-res phosphor
    for (int y = 0; y < img.height(); ++y) {
        QRgb* row = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            QRgb px = row[x];
            int r = qRed(px), g = qGreen(px), b = qBlue(px);

            // Quantise to 5-bit and add Bayer noise
            int thresh = BAYER2[y & 1][x & 1];   // 0–3
            int bias   = thresh * 8 - 12;          // -12 .. +12

            r = std::clamp(((r + bias) >> 3) << 3, 0, 255);
            g = std::clamp(((g + bias) >> 3) << 3, 0, 255);
            b = std::clamp(((b + bias) >> 3) << 3, 0, 255);
            row[x] = qRgb(r, g, b);
        }
    }
}

void GraphWidget::applyScanlineRows(QImage& img) {
    // Dim every other row (even rows) to simulate CRT horizontal scanline gap
    for (int y = 0; y < img.height(); y += 2) {
        QRgb* row = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            QRgb px = row[x];
            row[x] = qRgb(qRed(px)*7/10, qGreen(px)*7/10, qBlue(px)*7/10);
        }
    }
}

void GraphWidget::applyVignette(QImage& img) {
    // Darken corners/edges to simulate curved CRT tube falloff
    int W = img.width(), H = img.height();
    for (int y = 0; y < H; ++y) {
        QRgb* row = reinterpret_cast<QRgb*>(img.scanLine(y));
        double ny = (2.0 * y / H) - 1.0; // -1 to 1
        for (int x = 0; x < W; ++x) {
            double nx = (2.0 * x / W) - 1.0;
            double d  = nx * nx + ny * ny;           // 0 at centre, ~2 at corners
            double v  = std::max(0.0, 1.0 - d * 0.45);
            QRgb px = row[x];
            row[x] = qRgb(static_cast<int>(qRed(px)   * v),
                          static_cast<int>(qGreen(px) * v),
                          static_cast<int>(qBlue(px)  * v));
        }
    }
}

// ── paintEvent ────────────────────────────────────────────────────────────────

void GraphWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);

    if (!m_func && m_graphMode != GraphMode::OdeSimulation) {
        p.fillRect(rect(), m_bg);
        p.setPen(m_dim);
        p.setFont(QFont("Fixedsys", 9));
        p.drawText(rect(), Qt::AlignCenter, "NO FUNCTION LOADED");
        return;
    }

    if (!m_buffersReady) rebuildGraphBuffer();

    if (m_revealDone || !m_scanning) {
        // Draw fully revealed graph
        p.drawImage(0, 0, m_graphBuffer);
    } else {
        // Black everything first
        p.fillRect(rect(), m_bg);

        // Reveal rows 0 .. scanY from the graph buffer
        int scanPx = static_cast<int>(m_scanY);
        if (scanPx > 0) {
            p.drawImage(QRect(0, 0, width(), scanPx),
                        m_graphBuffer,
                        QRect(0, 0, width(), scanPx));
        }

        // Draw the moving scan line itself (bright phosphor sweep)
        drawScanline(p);
    }
}

void GraphWidget::drawScanline(QPainter& p) {
    int y = static_cast<int>(m_scanY);

    // Blocky phosphor sweep: a few hard rows instead of a smooth glow.
    for (int band = 0; band < 6; ++band) {
        int by = y - band;
        if (by < 0) continue;
        int alpha = band == 0 ? 230 : 150 - band * 18;
        p.setPen(QPen(QColor(m_scan.red(), m_scan.green(), m_scan.blue(), alpha), 1));
        p.drawLine(0, by, width(), by);
    }
}

void GraphWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_buffersReady = false;
}
