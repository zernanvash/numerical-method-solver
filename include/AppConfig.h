#pragma once

#include <QString>

struct AppConfig {
    QString theme = "Amber";
    double uiScale = 1.0;
    int fontSize = 12;
    double animationSpeed = 3.0;

    bool overlayScanlines = true;
    bool overlayVignette = true;
    bool graphScanlines = true;
    bool graphDither = true;
    bool graphVignette = true;
    bool bootSplash = true;

    QString defaultTolerance = "0.0001";
    int defaultMaxIterations = 50;

    static AppConfig load();
    void save() const;
};
