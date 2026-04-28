#pragma once
#include "IterationRecord.h"
#include <QWidget>
#include <QTimer>
#include <QImage>
#include <vector>
#include <functional>
#include <string>

enum class SolverMethod { Bisection, Newton, Secant };

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit GraphWidget(QWidget* parent = nullptr);

    // Set the function to plot and method-specific data
    void setFunction(std::function<double(double)> f, const std::string& expr);
    void setIterations(const std::vector<IterationRecord>& recs, SolverMethod method);

    // Start the scanline reveal animation
    void startReveal();
    void reset();

    // Step: reveal up to iteration N
    void showUpToIteration(int n);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onScanTick();

private:
    // ── Data ──────────────────────────────────────────────────────────────────
    std::function<double(double)> m_func;
    std::string                   m_expr;
    std::vector<IterationRecord>  m_iters;
    SolverMethod                  m_method = SolverMethod::Bisection;
    int                           m_showUpTo = 0; // how many iterations are revealed

    // ── Scanline state ────────────────────────────────────────────────────────
    QTimer* m_scanTimer;
    double  m_scanY      = 0.0;  // current scan line in widget pixels
    double  m_scanSpeed  = 3.0;  // pixels per tick
    bool    m_scanning   = false;
    bool    m_revealDone = false;

    // ── Off-screen buffers ────────────────────────────────────────────────────
    // "revealed" buffer: the fully rendered graph (drawn once)
    QImage  m_graphBuffer;
    // "hidden" buffer: dark background (pre-computed black)
    bool    m_buffersReady = false;

    // ── Coordinate helpers ────────────────────────────────────────────────────
    double m_xMin = -1.0, m_xMax = 4.0;
    double m_yMin = -3.0, m_yMax = 5.0;

    int    toScreenX(double wx) const;
    int    toScreenY(double wy) const;
    double toWorldX(int sx)     const;
    double toWorldY(int sy)     const;
    void   autoRange();

    // ── Rendering ─────────────────────────────────────────────────────────────
    void rebuildGraphBuffer();
    void drawGrid(QPainter& p);
    void drawAxes(QPainter& p);
    void drawCurve(QPainter& p);
    void drawBisectionStep(QPainter& p, int n);
    void drawNewtonStep(QPainter& p, int n);
    void drawSecantStep(QPainter& p, int n);
    void drawRootMarker(QPainter& p, int n);
    void drawScanline(QPainter& p);
    void applyDither(QImage& img);       // Bayer 2x2 dither pass
    void applyScanlineRows(QImage& img); // darken alternating rows
    void applyVignette(QImage& img);
};
