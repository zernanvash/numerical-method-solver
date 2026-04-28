#pragma once
#include <QWidget>

// Transparent overlay drawn on top of the entire window.
// Paints horizontal scanline lines + corner vignette to simulate CRT glass.
class CRTOverlay : public QWidget {
    Q_OBJECT
public:
    explicit CRTOverlay(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};
