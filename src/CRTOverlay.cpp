#include "CRTOverlay.h"
#include <QPainter>
#include <QPaintEvent>

CRTOverlay::CRTOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
}

void CRTOverlay::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Horizontal scanlines every 4 pixels
    painter.setOpacity(0.07);
    painter.setPen(QColor(0, 0, 0));
    for (int y = 0; y < height(); y += 4) {
        painter.drawLine(0, y, width(), y);
    }

    // Corner vignette (radial darkening)
    painter.setOpacity(1.0);
    int W = width(), H = height();
    int grad = 80; // how many pixels inward the vignette reaches
    for (int edge = 0; edge < grad; ++edge) {
        int alpha = static_cast<int>(80.0 * (1.0 - static_cast<double>(edge) / grad));
        painter.setPen(QColor(0, 0, 0, alpha));
        // Top
        painter.drawLine(edge, edge, W - edge, edge);
        // Bottom
        painter.drawLine(edge, H - edge, W - edge, H - edge);
        // Left
        painter.drawLine(edge, edge, edge, H - edge);
        // Right
        painter.drawLine(W - edge, edge, W - edge, H - edge);
    }
}
