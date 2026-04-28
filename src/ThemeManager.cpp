#include "ThemeManager.h"

static ThemePalette makeTheme(const QString& name,
                              const char* bg,
                              const char* panel,
                              const char* input,
                              const char* border,
                              const char* dim,
                              const char* text,
                              const char* accent,
                              const char* error) {
    return {name, QColor(bg), QColor(panel), QColor(input), QColor(border),
            QColor(dim), QColor(text), QColor(accent), QColor(error)};
}

QStringList ThemeManager::themeNames() {
    return {"Amber", "Yellow", "Green", "Red", "Blue", "White"};
}

ThemePalette ThemeManager::palette(const QString& name) {
    const QString n = name.trimmed().toLower();

    if (n == "yellow") {
        return makeTheme("Yellow", "#0A0A00", "#141400", "#050500", "#555500",
                         "#999933", "#FFFF66", "#FFF2A0", "#FF6B5E");
    }
    if (n == "green") {
        return makeTheme("Green", "#001008", "#001A0D", "#000804", "#2F7A42",
                         "#3A8F50", "#72FF8F", "#A0FFB0", "#FF6B5E");
    }
    if (n == "red") {
        return makeTheme("Red", "#100000", "#1A0505", "#070000", "#773333",
                         "#883333", "#FF5555", "#FF9999", "#FFCC66");
    }
    if (n == "blue") {
        return makeTheme("Blue", "#000A14", "#001426", "#00050A", "#245A70",
                         "#3A7890", "#7EE7FF", "#B8F3FF", "#FF6B5E");
    }
    if (n == "white") {
        return makeTheme("White", "#080808", "#141414", "#050505", "#555555",
                         "#888888", "#EAEAEA", "#FFFFFF", "#FF6B5E");
    }

    return makeTheme("Amber", "#0A0800", "#100D00", "#050300", "#4A3400",
                     "#7A5800", "#FFA800", "#FFC840", "#FF6B5E");
}

QString ThemeManager::buildStyleSheet(const AppConfig& cfg) {
    const ThemePalette t = palette(cfg.theme);
    const int base = static_cast<int>(cfg.fontSize * cfg.uiScale);
    const int small = qMax(8, base - 2);
    const int header = base + 1;
    const int padY = qMax(3, static_cast<int>(4 * cfg.uiScale));
    const int padX = qMax(5, static_cast<int>(6 * cfg.uiScale));

    return QString(R"(
QWidget {
    background-color: %1;
    color: %6;
    font-family: "Courier New";
    font-size: %9px;
}
QMainWindow, QWidget#central {
    background-color: %1;
}
QGroupBox {
    background-color: %2;
    border: 1px solid %4;
    border-radius: 2px;
    margin-top: 14px;
    font-size: %10px;
    color: %5;
    letter-spacing: 2px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 8px;
    top: 2px;
    color: %5;
}
QLineEdit {
    background-color: %3;
    color: %6;
    border: 1px solid %4;
    border-radius: 0px;
    padding: %12px %13px;
    selection-background-color: %4;
    selection-color: %1;
}
QLineEdit:focus {
    border: 1px solid %6;
}
QPushButton {
    background-color: %1;
    color: %6;
    border: 1px solid %4;
    border-radius: 0px;
    padding: %12px %13px;
    letter-spacing: 2px;
}
QPushButton:hover {
    background-color: %2;
    border: 1px solid %6;
    color: %7;
}
QPushButton:pressed {
    background-color: %4;
    color: %1;
}
QPushButton#btnSolve {
    color: %7;
    border: 1px solid %6;
    font-size: %11px;
}
QPushButton#btnStep {
    color: %7;
    border: 1px solid %4;
}
QPushButton#btnReset {
    color: %5;
    border: 1px solid %4;
}
QPushButton#btnSettings, QPushButton#btnExportTxt, QPushButton#btnExportCsv {
    color: %6;
    border: 1px solid %4;
}
QComboBox, QSpinBox, QDoubleSpinBox {
    background-color: %3;
    color: %6;
    border: 1px solid %4;
    border-radius: 0px;
    padding: %12px %13px;
}
QComboBox QAbstractItemView {
    background-color: %2;
    color: %6;
    border: 1px solid %4;
    selection-background-color: %4;
}
QComboBox::drop-down {
    border: none;
}
QCheckBox {
    color: %6;
    spacing: 8px;
}
QLabel#statusLabel {
    color: %6;
    border: 1px solid %4;
    padding: 3px 8px;
    letter-spacing: 2px;
    font-size: %10px;
}
QLabel#rootLabel {
    color: %7;
    font-size: %11px;
    letter-spacing: 1px;
}
QLabel#methodInfo {
    color: %5;
    font-size: %10px;
    line-height: 160%;
}
QTableWidget {
    background-color: %3;
    color: %6;
    border: 1px solid %4;
    gridline-color: %2;
    font-size: %10px;
    font-family: "Courier New";
}
QTableWidget::item {
    padding: 2px 6px;
    border-bottom: 1px solid %2;
}
QTableWidget::item:selected {
    background-color: %4;
    color: %7;
}
QHeaderView::section {
    background-color: %2;
    color: %5;
    border: 1px solid %4;
    padding: 3px 6px;
    font-size: %10px;
    letter-spacing: 1px;
}
QScrollBar:vertical {
    background: %3;
    width: 8px;
    border: 1px solid %4;
}
QScrollBar::handle:vertical {
    background: %4;
    min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
)")
        .arg(t.bg.name(), t.panel.name(), t.input.name(), t.border.name(),
             t.dim.name(), t.text.name(), t.accent.name(), t.error.name())
        .arg(base)
        .arg(small)
        .arg(header)
        .arg(padY)
        .arg(padX);
}
