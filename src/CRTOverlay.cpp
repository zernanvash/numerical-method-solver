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

void CRTOverlay::setScanlinesEnabled(bool enabled) {
    m_scanlinesEnabled = enabled;
    update();
}

void CRTOverlay::setVignetteEnabled(bool enabled) {
    m_vignetteEnabled = enabled;
    update();
}

void CRTOverlay::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (m_scanlinesEnabled) {
        painter.setOpacity(0.07);
        painter.setPen(QColor(0, 0, 0));
        for (int y = 0; y < height(); y += 4) {
            painter.drawLine(0, y, width(), y);
        }
    }

    if (m_vignetteEnabled) {
        painter.setOpacity(1.0);
        int W = width(), H = height();
        int grad = 80;
        for (int edge = 0; edge < grad; ++edge) {
            int alpha = static_cast<int>(80.0 * (1.0 - static_cast<double>(edge) / grad));
            painter.setPen(QColor(0, 0, 0, alpha));
            painter.drawLine(edge, edge, W - edge, edge);
            painter.drawLine(edge, H - edge, W - edge, H - edge);
            painter.drawLine(edge, edge, edge, H - edge);
            painter.drawLine(W - edge, edge, W - edge, H - edge);
        }
    }
}
