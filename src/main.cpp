#include "MainWindow.h"
#include "AppConfig.h"
#include "ThemeManager.h"

#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QFont>

static QPixmap makeSplash(int w, int h, const ThemePalette& pal) {
    QPixmap pm(w, h);
    pm.fill(pal.bg);
    QPainter p(&pm);

    QFont mono("Courier New", 11);
    p.setFont(mono);

    auto line = [&](int y, const QString& s, QColor c) {
        p.setPen(c);
        p.drawText(40, y, s);
    };

    line(60,  "ROOT-FINDING COMPUTATION CONSOLE  v1.0", pal.text);
    line(80,  "========================================", pal.dim);
    line(110, "INITIALIZING NUMERICAL ENGINE...",        pal.dim);
    line(130, "LOADING FUNCTION PARSER............OK",   pal.border);
    line(150, "LOADING BISECTION MODULE...........OK",   pal.border);
    line(170, "LOADING NEWTON-RAPHSON MODULE......OK",   pal.border);
    line(190, "LOADING SECANT MODULE..............OK",   pal.border);
    line(210, "LOADING CRT RENDERER...............OK",   pal.border);
    line(240, "SYSTEM READY",                            pal.accent);
    line(260, "LOADING INTERFACE",                       pal.dim);

    p.setOpacity(0.06);
    p.setPen(QColor(0,0,0));
    for (int y = 0; y < h; y += 4) p.drawLine(0, y, w, y);

    return pm;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("RootFinder");
    app.setApplicationDisplayName("Root-Finding Console");

    AppConfig cfg = AppConfig::load();
    ThemePalette pal = ThemeManager::palette(cfg.theme);

    MainWindow w;

    if (!cfg.bootSplash) {
        w.show();
        return app.exec();
    }

    QPixmap splash = makeSplash(560, 310, pal);
    QSplashScreen sc(splash);
    sc.show();
    app.processEvents();

    QTimer::singleShot(1800, [&]() {
        sc.finish(&w);
        w.show();
    });

    return app.exec();
}
