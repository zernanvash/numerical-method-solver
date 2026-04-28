#pragma once

#include "AppConfig.h"
#include <QColor>
#include <QString>
#include <QStringList>

struct ThemePalette {
    QString name;
    QColor bg;
    QColor panel;
    QColor input;
    QColor border;
    QColor dim;
    QColor text;
    QColor accent;
    QColor error;
};

class ThemeManager {
public:
    static QStringList themeNames();
    static ThemePalette palette(const QString& name);
    static QString buildStyleSheet(const AppConfig& cfg);
};
