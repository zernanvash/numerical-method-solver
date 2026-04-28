#include "AppConfig.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

static QString configPath() {
    return QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
}

AppConfig AppConfig::load() {
    AppConfig cfg;
    QSettings s(configPath(), QSettings::IniFormat);

    cfg.theme = s.value("Appearance/theme", cfg.theme).toString();
    cfg.uiScale = s.value("Appearance/uiScale", cfg.uiScale).toDouble();
    cfg.fontSize = s.value("Appearance/fontSize", cfg.fontSize).toInt();
    cfg.animationSpeed = s.value("Appearance/animationSpeed", cfg.animationSpeed).toDouble();

    cfg.overlayScanlines = s.value("Effects/overlayScanlines", cfg.overlayScanlines).toBool();
    cfg.overlayVignette = s.value("Effects/overlayVignette", cfg.overlayVignette).toBool();
    cfg.graphScanlines = s.value("Effects/graphScanlines", cfg.graphScanlines).toBool();
    cfg.graphDither = s.value("Effects/graphDither", cfg.graphDither).toBool();
    cfg.graphVignette = s.value("Effects/graphVignette", cfg.graphVignette).toBool();
    cfg.bootSplash = s.value("Effects/bootSplash", cfg.bootSplash).toBool();

    cfg.defaultTolerance = s.value("Defaults/tolerance", cfg.defaultTolerance).toString();
    cfg.defaultMaxIterations = s.value("Defaults/maxIterations", cfg.defaultMaxIterations).toInt();

    if (cfg.uiScale < 0.75) cfg.uiScale = 0.75;
    if (cfg.uiScale > 1.75) cfg.uiScale = 1.75;
    if (cfg.fontSize < 9) cfg.fontSize = 9;
    if (cfg.fontSize > 22) cfg.fontSize = 22;
    if (cfg.animationSpeed < 0.5) cfg.animationSpeed = 0.5;
    if (cfg.animationSpeed > 20.0) cfg.animationSpeed = 20.0;
    if (cfg.defaultMaxIterations < 1) cfg.defaultMaxIterations = 1;
    if (cfg.defaultMaxIterations > 10000) cfg.defaultMaxIterations = 10000;

    return cfg;
}

void AppConfig::save() const {
    QSettings s(configPath(), QSettings::IniFormat);

    s.setValue("Appearance/theme", theme);
    s.setValue("Appearance/uiScale", uiScale);
    s.setValue("Appearance/fontSize", fontSize);
    s.setValue("Appearance/animationSpeed", animationSpeed);

    s.setValue("Effects/overlayScanlines", overlayScanlines);
    s.setValue("Effects/overlayVignette", overlayVignette);
    s.setValue("Effects/graphScanlines", graphScanlines);
    s.setValue("Effects/graphDither", graphDither);
    s.setValue("Effects/graphVignette", graphVignette);
    s.setValue("Effects/bootSplash", bootSplash);

    s.setValue("Defaults/tolerance", defaultTolerance);
    s.setValue("Defaults/maxIterations", defaultMaxIterations);
    s.sync();
}
