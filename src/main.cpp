#include "MainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <QFontDatabase>

// Build a simple amber-on-black boot splash painted with QPainter
static QPixmap makeSplash(int w, int h) {
    QPixmap pm(w, h);
    pm.fill(QColor(8, 6, 0));
    QPainter p(&pm);

    QFont mono("Courier New", 11);
    p.setFont(mono);

    auto line = [&](int y, const QString& s, QColor c = QColor(255,168,0)) {
        p.setPen(c);
        p.drawText(40, y, s);
    };

    line(60,  "ROOT-FINDING COMPUTATION CONSOLE  v1.0");
    line(80,  "========================================", QColor(60,44,0));
    line(110, "INITIALIZING NUMERICAL ENGINE...",        QColor(120,90,0));
    line(130, "LOADING FUNCTION PARSER............OK",   QColor(100,76,0));
    line(150, "LOADING BISECTION MODULE...........OK",   QColor(100,76,0));
    line(170, "LOADING NEWTON-RAPHSON MODULE......OK",   QColor(100,76,0));
    line(190, "LOADING SECANT MODULE..............OK",   QColor(100,76,0));
    line(210, "LOADING CRT RENDERER...............OK",   QColor(100,76,0));
    line(240, "SYSTEM READY",                            QColor(255,200,60));
    line(260, "PRESS ANY KEY TO CONTINUE",               QColor(60,44,0));

    // Scanline overlay on splash
    p.setOpacity(0.06);
    p.setPen(QColor(0,0,0));
    for (int y = 0; y < h; y += 4) p.drawLine(0, y, w, y);

    return pm;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("RootFinder");
    app.setApplicationDisplayName("Root-Finding Console");

    // Prefer system monospaced font for labels
    QFontDatabase::addApplicationFont(":/fonts/optional_mono_font.ttf"); // optional

    // Boot splash
    QPixmap splash = makeSplash(560, 310);
    QSplashScreen* sc = new QSplashScreen(splash);
    sc->show();
    app.processEvents();

    MainWindow w;

    // Show main window after splash delay
    QTimer::singleShot(2200, [&]() {
        sc->finish(&w);
        w.show();
    });

    return app.exec();
}
