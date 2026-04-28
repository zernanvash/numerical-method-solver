#pragma once
#include "IterationRecord.h"
#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QColor>
#include <vector>
#include <functional>
#include <string>

struct ThemePalette;
enum class SolverMethod { Bisection, Newton, Secant };

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit GraphWidget(QWidget* parent = nullptr);

    void setFunction(std::function<double(double)> f, const std::string& expr);
    void setIterations(const std::vector<IterationRecord>& recs, SolverMethod method);

    void startReveal();
    void reset();
    void showUpToIteration(int n);

    void setAnimationSpeed(double speed);
    void setGraphEffects(bool scanlines, bool dither, bool vignette);
    void setThemeColors(const QColor& background, const QColor& primary, const QColor& dim, const QColor& accent);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onScanTick();

private:
    std::function<double(double)> m_func;
    std::string                   m_expr;
    std::vector<IterationRecord>  m_iters;
    SolverMethod                  m_method = SolverMethod::Bisection;
    int                           m_showUpTo = 0;

    QTimer* m_scanTimer;
    double  m_scanY      = 0.0;
    double  m_scanSpeed  = 3.0;
    bool    m_scanning   = false;
    bool    m_revealDone = false;

    bool    m_enableGraphScanlines = true;
    bool    m_enableDither = true;
    bool    m_enableVignette = true;

    QColor  m_bg        = QColor(10, 8, 0);
    QColor  m_grid      = QColor(50, 38, 0, 90);
    QColor  m_axis      = QColor(100, 76, 0, 180);
    QColor  m_curve     = QColor(255, 176, 0);
    QColor  m_scan      = QColor(255, 210, 80);
    QColor  m_bisect    = QColor(180, 255, 100);
    QColor  m_tangent   = QColor(100, 200, 255);
    QColor  m_secant    = QColor(255, 200, 80);
    QColor  m_root      = QColor(255, 255, 0);
    QColor  m_dim       = QColor(90, 68, 0);

    QImage  m_graphBuffer;
    bool    m_buffersReady = false;

    double m_xMin = -1.0, m_xMax = 4.0;
    double m_yMin = -3.0, m_yMax = 5.0;

    int    toScreenX(double wx) const;
    int    toScreenY(double wy) const;
    double toWorldX(int sx)     const;
    double toWorldY(int sy)     const;
    void   autoRange();

    void rebuildGraphBuffer();
    void drawGrid(QPainter& p);
    void drawAxes(QPainter& p);
    void drawCurve(QPainter& p);
    void drawBisectionStep(QPainter& p, int n);
    void drawNewtonStep(QPainter& p, int n);
    void drawSecantStep(QPainter& p, int n);
    void drawRootMarker(QPainter& p, int n);
    void drawScanline(QPainter& p);
    void applyDither(QImage& img);
    void applyScanlineRows(QImage& img);
    void applyVignette(QImage& img);
};
