#pragma once
#include <QWidget>

class CRTOverlay : public QWidget {
    Q_OBJECT
public:
    explicit CRTOverlay(QWidget* parent = nullptr);

    void setScanlinesEnabled(bool enabled);
    void setVignetteEnabled(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_scanlinesEnabled = true;
    bool m_vignetteEnabled = true;
};
