#include "MainWindow.h"
#include "FunctionParser.h"
#include "BisectionSolver.h"
#include "NewtonSolver.h"
#include "SecantSolver.h"
#include "ThemeManager.h"
#include "ExportManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QResizeEvent>
#include <QFont>
#include <QApplication>
#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QDateTime>
#include <QScrollArea>
#include <QSplitter>
#include <QSizePolicy>
#include <QFrame>

static const char* DESC_BISECT =
    "TYPE: BRACKETING\n"
    "NEEDS: f(a)*f(b) < 0\n"
    "CONV: LINEAR (slow)";
static const char* DESC_NEWTON =
    "TYPE: OPEN METHOD\n"
    "NEEDS: DERIVATIVE\n"
    "CONV: QUADRATIC (fast)";
static const char* DESC_SECANT =
    "TYPE: OPEN METHOD\n"
    "NEEDS: TWO GUESSES\n"
    "CONV: SUPERLINEAR";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_config(AppConfig::load())
{
    setWindowTitle("ROOT-FINDING COMPUTATION CONSOLE v1.0");
    // Responsive minimum: small enough for laptops, but prevents unreadable crushing.
    setMinimumSize(760, 520);

    m_stepTimer = new QTimer(this);
    m_stepTimer->setInterval(600);
    connect(m_stepTimer, &QTimer::timeout, this, &MainWindow::onStepTimerTick);

    buildUI();
    applyConfig();
    updateMethodLabels();
}

void MainWindow::buildUI() {
    m_central = new QWidget;
    m_central->setObjectName("central");
    setCentralWidget(m_central);

    QHBoxLayout* headerRow = new QHBoxLayout;
    m_titleLabel = new QLabel("ROOT-FINDING COMPUTATION CONSOLE  v1.0");
    m_lblStatus = new QLabel("STATUS: READY");
    m_lblStatus->setObjectName("statusLabel");
    headerRow->addWidget(m_titleLabel);
    headerRow->addStretch();
    headerRow->addWidget(m_lblStatus);

    QVBoxLayout* leftPanel = new QVBoxLayout;
    leftPanel->setContentsMargins(0, 0, 0, 0);
    leftPanel->setSpacing(8);

    QGroupBox* gbFn = new QGroupBox("FUNCTION");
    QVBoxLayout* fnLayout = new QVBoxLayout(gbFn);
    m_fnInput = new QLineEdit("x^3 - x - 2");
    m_fnInput->setPlaceholderText("e.g. x^3 - x - 2");
    connect(m_fnInput, &QLineEdit::textChanged, this, &MainWindow::onFunctionChanged);
    fnLayout->addWidget(m_fnInput);
    leftPanel->addWidget(gbFn);

    QGroupBox* gbMethod = new QGroupBox("METHOD");
    QVBoxLayout* methodLayout = new QVBoxLayout(gbMethod);
    m_methodBox = new QComboBox;
    m_methodBox->addItems({"BISECTION", "NEWTON-RAPHSON", "SECANT"});
    connect(m_methodBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onMethodChanged);
    m_lblMethodInfo = new QLabel(DESC_BISECT);
    m_lblMethodInfo->setObjectName("methodInfo");
    m_lblMethodInfo->setWordWrap(true);
    methodLayout->addWidget(m_methodBox);
    methodLayout->addWidget(m_lblMethodInfo);
    leftPanel->addWidget(gbMethod);

    QGroupBox* gbParams = new QGroupBox("PARAMETERS");
    QGridLayout* paramGrid = new QGridLayout(gbParams);
    paramGrid->setSpacing(5);

    auto makeParamRow = [&](QGridLayout* g, int row,
                             const QString& lbl, QLineEdit*& le, const QString& val) {
        g->addWidget(new QLabel(lbl), row, 0);
        le = new QLineEdit(val);
        le->setMinimumWidth(70);
        le->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        g->addWidget(le, row, 1);
    };

    QLabel* lblA = new QLabel("a  >");
    lblA->setObjectName("labelA");
    m_paramA = new QLineEdit("1");
    m_paramA->setMinimumWidth(70);
    m_paramA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    paramGrid->addWidget(lblA, 0, 0);
    paramGrid->addWidget(m_paramA, 0, 1);

    QLabel* lblB = new QLabel("b  >");
    lblB->setObjectName("labelB");
    m_paramB = new QLineEdit("2");
    m_paramB->setMinimumWidth(70);
    m_paramB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    paramGrid->addWidget(lblB, 1, 0);
    paramGrid->addWidget(m_paramB, 1, 1);

    makeParamRow(paramGrid, 2, "tol >", m_paramTol, m_config.defaultTolerance);
    makeParamRow(paramGrid, 3, "max >", m_paramMax, QString::number(m_config.defaultMaxIterations));
    paramGrid->setColumnStretch(0, 0);
    paramGrid->setColumnStretch(1, 1);

    leftPanel->addWidget(gbParams);

    QGroupBox* gbResult = new QGroupBox("RESULT");
    QVBoxLayout* resLayout = new QVBoxLayout(gbResult);
    m_lblRoot = new QLabel("ROOT: —");
    m_lblRoot->setObjectName("rootLabel");
    resLayout->addWidget(m_lblRoot);
    leftPanel->addWidget(gbResult);

    m_btnSolve = new QPushButton("[ SOLVE ]");
    m_btnSolve->setObjectName("btnSolve");
    m_btnStep  = new QPushButton("[ STEP ]");
    m_btnStep->setObjectName("btnStep");
    m_btnReset = new QPushButton("[ RESET ]");
    m_btnReset->setObjectName("btnReset");
    m_btnSettings = new QPushButton("[ SETTINGS ]");
    m_btnSettings->setObjectName("btnSettings");
    m_btnExportTxt = new QPushButton("[ EXPORT TXT ]");
    m_btnExportTxt->setObjectName("btnExportTxt");
    m_btnExportCsv = new QPushButton("[ EXPORT CSV ]");
    m_btnExportCsv->setObjectName("btnExportCsv");

    for (QPushButton* btn : {m_btnSolve, m_btnStep, m_btnReset, m_btnSettings, m_btnExportTxt, m_btnExportCsv}) {
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumHeight(34);
    }

    connect(m_btnSolve, &QPushButton::clicked, this, &MainWindow::onSolve);
    connect(m_btnStep,  &QPushButton::clicked, this, &MainWindow::onStep);
    connect(m_btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(m_btnSettings, &QPushButton::clicked, this, &MainWindow::onSettings);
    connect(m_btnExportTxt, &QPushButton::clicked, this, &MainWindow::onExportTxt);
    connect(m_btnExportCsv, &QPushButton::clicked, this, &MainWindow::onExportCsv);

    leftPanel->addWidget(m_btnSolve);
    leftPanel->addWidget(m_btnStep);
    leftPanel->addWidget(m_btnReset);
    leftPanel->addWidget(m_btnSettings);
    leftPanel->addWidget(m_btnExportTxt);
    leftPanel->addWidget(m_btnExportCsv);
    leftPanel->addStretch();

    m_graph = new GraphWidget;
    m_graph->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_iterTable = new QTableWidget(0, 5);
    m_iterTable->setHorizontalHeaderLabels({"ITER","X VALUE","f(X)","ERROR","STATUS"});
    m_iterTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_iterTable->verticalHeader()->setVisible(false);
    m_iterTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_iterTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_iterTable->setMinimumHeight(110);
    m_iterTable->setMaximumHeight(240);
    m_iterTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* rightPanel = new QVBoxLayout;
    rightPanel->setContentsMargins(0, 0, 0, 0);
    rightPanel->addWidget(m_graph, 1);
    rightPanel->addWidget(m_iterTable);

    m_leftWidget = new QWidget;
    m_leftWidget->setLayout(leftPanel);
    m_leftWidget->setMinimumWidth(220);
    m_leftWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    m_leftScroll = new QScrollArea;
    m_leftScroll->setWidgetResizable(true);
    m_leftScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_leftScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_leftScroll->setFrameShape(QFrame::NoFrame);
    m_leftScroll->setWidget(m_leftWidget);
    m_leftScroll->setMinimumWidth(230);
    m_leftScroll->setMaximumWidth(360);
    m_leftScroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    QWidget* rightWidget = new QWidget;
    rightWidget->setLayout(rightPanel);
    rightWidget->setMinimumWidth(420);
    rightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_bodySplitter = new QSplitter(Qt::Horizontal);
    m_bodySplitter->addWidget(m_leftScroll);
    m_bodySplitter->addWidget(rightWidget);
    m_bodySplitter->setStretchFactor(0, 0);
    m_bodySplitter->setStretchFactor(1, 1);
    m_bodySplitter->setCollapsible(0, false);
    m_bodySplitter->setCollapsible(1, false);
    m_bodySplitter->setSizes({260, 700});

    QVBoxLayout* root = new QVBoxLayout(m_central);
    root->setContentsMargins(8, 6, 8, 6);
    root->setSpacing(6);
    root->addLayout(headerRow);
    root->addWidget(m_bodySplitter, 1);

    m_overlay = new CRTOverlay(m_central);
    m_overlay->setGeometry(m_central->rect());
    m_overlay->raise();
}

void MainWindow::applyStyleSheet() {
    qApp->setStyleSheet(ThemeManager::buildStyleSheet(m_config));
}

void MainWindow::applyConfig() {
    applyStyleSheet();
    const ThemePalette pal = ThemeManager::palette(m_config.theme);

    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(QString("color:%1;font-size:%2px;letter-spacing:3px;font-weight:bold;")
                                    .arg(pal.text.name())
                                    .arg(static_cast<int>((m_config.fontSize + 1) * m_config.uiScale)));
    }

    const int leftMin = static_cast<int>(220 * m_config.uiScale);
    const int leftMax = static_cast<int>(360 * m_config.uiScale);
    const int fieldMin = static_cast<int>(70 * m_config.uiScale);

    if (m_leftWidget) {
        m_leftWidget->setMinimumWidth(leftMin);
    }
    if (m_leftScroll) {
        m_leftScroll->setMinimumWidth(leftMin + 10);
        m_leftScroll->setMaximumWidth(leftMax);
    }
    for (QLineEdit* le : {m_paramA, m_paramB, m_paramTol, m_paramMax}) {
        if (le) {
            le->setMinimumWidth(fieldMin);
            le->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }
    }

    if (m_graph) {
        m_graph->setAnimationSpeed(m_config.animationSpeed);
        m_graph->setGraphEffects(m_config.graphScanlines, m_config.graphDither, m_config.graphVignette);
        m_graph->setThemeColors(pal.bg, pal.text, pal.dim, pal.accent);
    }
    if (m_overlay) {
        m_overlay->setScanlinesEnabled(m_config.overlayScanlines);
        m_overlay->setVignetteEnabled(m_config.overlayVignette);
        m_overlay->raise();
    }

    setStatus("STATUS: READY", pal.text.name());
}

void MainWindow::onFunctionChanged(const QString& text) {
    std::string err = FunctionParser::validate(text.toStdString());
    if (err.empty()) {
        m_fnInput->setStyleSheet("");
    } else {
        m_fnInput->setStyleSheet("border:1px solid #FF4422;");
    }
}

void MainWindow::onMethodChanged(int) {
    updateMethodLabels();
    onReset();
}

void MainWindow::updateMethodLabels() {
    int idx = m_methodBox->currentIndex();
    QLabel* lblA = m_central->findChild<QLabel*>("labelA");
    QLabel* lblB = m_central->findChild<QLabel*>("labelB");
    const char* desc = DESC_BISECT;

    if (idx == 0) {
        if (lblA) lblA->setText("a  >");
        if (lblB) lblB->setText("b  >");
        desc = DESC_BISECT;
    } else if (idx == 1) {
        if (lblA) lblA->setText("x0 >");
        if (lblB) lblB->setText("x1 >");
        desc = DESC_NEWTON;
    } else {
        if (lblA) lblA->setText("x0 >");
        if (lblB) lblB->setText("x1 >");
        desc = DESC_SECANT;
    }
    m_lblMethodInfo->setText(desc);
}

void MainWindow::runCompute() {
    std::string err = FunctionParser::validate(m_fnInput->text().toStdString());
    if (!err.empty()) {
        setStatus("STATUS: INVALID INPUT", ThemeManager::palette(m_config.theme).error.name());
        return;
    }
    m_func = FunctionParser::parse(m_fnInput->text().toStdString());
    m_graph->setFunction(m_func, m_fnInput->text().toStdString());

    double a   = m_paramA->text().toDouble();
    double b   = m_paramB->text().toDouble();
    double tol = m_paramTol->text().toDouble();
    int    maxI = m_paramMax->text().toInt();

    try {
        switch (currentMethod()) {
        case SolverMethod::Bisection:
            m_records = BisectionSolver::solve(m_func, a, b, tol, maxI);
            break;
        case SolverMethod::Newton:
            m_records = NewtonSolver::solve(m_func, a, tol, maxI);
            break;
        case SolverMethod::Secant:
            m_records = SecantSolver::solve(m_func, a, b, tol, maxI);
            break;
        }
    } catch (const std::exception& ex) {
        setStatus(QString("STATUS: ERR: %1").arg(ex.what()).left(40), ThemeManager::palette(m_config.theme).error.name());
        return;
    }

    m_graph->setIterations(m_records, currentMethod());
    m_stepIndex = 0;
}

void MainWindow::onSolve() {
    m_stepTimer->stop();
    m_autoStepping = false;
    onReset();
    runCompute();
    if (m_records.empty()) return;

    setStatus("STATUS: PROCESSING", ThemeManager::palette(m_config.theme).accent.name());
    clearTable();

    for (auto& r : m_records) addIterRow(r, false);
    if (!m_records.empty()) {
        int lastRow = m_iterTable->rowCount() - 1;
        for (int c = 0; c < 5; ++c) {
            if (auto* item = m_iterTable->item(lastRow, c))
                item->setForeground(ThemeManager::palette(m_config.theme).accent);
        }
        m_iterTable->scrollToBottom();
    }

    const auto& last = m_records.back();
    if (last.status == "CONVERGED") {
        setStatus("STATUS: CONVERGED", ThemeManager::palette(m_config.theme).accent.name());
        m_lblRoot->setText(QString("ROOT: x = %1\nf(x) = %2")
                           .arg(last.x, 0, 'f', 8)
                           .arg(last.fx, 0, 'e', 4));
    } else {
        setStatus("STATUS: MAX ITER REACHED", ThemeManager::palette(m_config.theme).accent.name());
        m_lblRoot->setText(QString("BEST: x ≈ %1").arg(last.x, 0, 'f', 8));
    }

    m_graph->showUpToIteration(static_cast<int>(m_records.size()));
}

void MainWindow::onStep() {
    if (m_records.empty()) {
        runCompute();
        if (m_records.empty()) return;
        clearTable();
        setStatus("STATUS: ITERATING", ThemeManager::palette(m_config.theme).accent.name());
    }
    if (m_stepIndex >= static_cast<int>(m_records.size())) return;

    ++m_stepIndex;
    const auto& rec = m_records[m_stepIndex - 1];
    addIterRow(rec, true);
    m_iterTable->scrollToBottom();

    m_graph->showUpToIteration(m_stepIndex);

    if (rec.status == "CONVERGED") {
        setStatus("STATUS: CONVERGED", ThemeManager::palette(m_config.theme).accent.name());
        m_lblRoot->setText(QString("ROOT: x = %1\nf(x) = %2")
                           .arg(rec.x, 0, 'f', 8)
                           .arg(rec.fx, 0, 'e', 4));
    } else {
        setStatus(QString("STATUS: ITERATING [%1/%2]")
                  .arg(m_stepIndex).arg(m_records.size()), ThemeManager::palette(m_config.theme).accent.name());
        m_lblRoot->setText(QString("x ≈ %1").arg(rec.x, 0, 'f', 6));
    }
}

void MainWindow::onReset() {
    m_stepTimer->stop();
    m_autoStepping = false;
    m_records.clear();
    m_stepIndex = 0;
    clearTable();
    m_lblRoot->setText("ROOT: —");
    setStatus("STATUS: READY", ThemeManager::palette(m_config.theme).text.name());
    m_graph->reset();
}

void MainWindow::onStepTimerTick() {
    if (m_stepIndex >= static_cast<int>(m_records.size())) {
        m_stepTimer->stop();
        return;
    }
    onStep();
}

void MainWindow::onSettings() {
    QDialog dialog(this);
    dialog.setWindowTitle("SYSTEM CONFIGURATION");
    dialog.setModal(true);

    auto* root = new QVBoxLayout(&dialog);
    auto* form = new QFormLayout;

    auto* themeBox = new QComboBox;
    themeBox->addItems(ThemeManager::themeNames());
    themeBox->setCurrentText(m_config.theme);

    auto* scaleSpin = new QDoubleSpinBox;
    scaleSpin->setRange(0.75, 1.75);
    scaleSpin->setSingleStep(0.05);
    scaleSpin->setValue(m_config.uiScale);

    auto* fontSpin = new QSpinBox;
    fontSpin->setRange(9, 22);
    fontSpin->setValue(m_config.fontSize);

    auto* speedSpin = new QDoubleSpinBox;
    speedSpin->setRange(0.5, 20.0);
    speedSpin->setSingleStep(0.5);
    speedSpin->setValue(m_config.animationSpeed);

    auto* overlayScan = new QCheckBox("Window CRT scanlines");
    overlayScan->setChecked(m_config.overlayScanlines);
    auto* overlayVignette = new QCheckBox("Window CRT vignette");
    overlayVignette->setChecked(m_config.overlayVignette);
    auto* graphScan = new QCheckBox("Graph scanlines");
    graphScan->setChecked(m_config.graphScanlines);
    auto* graphDither = new QCheckBox("Graph pixel dithering");
    graphDither->setChecked(m_config.graphDither);
    auto* graphVignette = new QCheckBox("Graph vignette");
    graphVignette->setChecked(m_config.graphVignette);
    auto* bootSplash = new QCheckBox("Boot splash screen");
    bootSplash->setChecked(m_config.bootSplash);

    auto* tolEdit = new QLineEdit(m_config.defaultTolerance);
    auto* maxSpin = new QSpinBox;
    maxSpin->setRange(1, 10000);
    maxSpin->setValue(m_config.defaultMaxIterations);

    form->addRow("Theme", themeBox);
    form->addRow("UI Scale", scaleSpin);
    form->addRow("Font Size", fontSpin);
    form->addRow("Animation Speed", speedSpin);
    form->addRow("Default Tolerance", tolEdit);
    form->addRow("Default Max Iterations", maxSpin);
    form->addRow("", overlayScan);
    form->addRow("", overlayVignette);
    form->addRow("", graphScan);
    form->addRow("", graphDither);
    form->addRow("", graphVignette);
    form->addRow("", bootSplash);

    root->addLayout(form);

    auto* row = new QHBoxLayout;
    auto* applyBtn = new QPushButton("[ APPLY ]");
    auto* saveBtn = new QPushButton("[ SAVE & CLOSE ]");
    auto* closeBtn = new QPushButton("[ CLOSE ]");
    row->addStretch();
    row->addWidget(applyBtn);
    row->addWidget(saveBtn);
    row->addWidget(closeBtn);
    root->addLayout(row);

    auto applyValues = [&]() {
        m_config.theme = themeBox->currentText();
        m_config.uiScale = scaleSpin->value();
        m_config.fontSize = fontSpin->value();
        m_config.animationSpeed = speedSpin->value();
        m_config.overlayScanlines = overlayScan->isChecked();
        m_config.overlayVignette = overlayVignette->isChecked();
        m_config.graphScanlines = graphScan->isChecked();
        m_config.graphDither = graphDither->isChecked();
        m_config.graphVignette = graphVignette->isChecked();
        m_config.bootSplash = bootSplash->isChecked();
        m_config.defaultTolerance = tolEdit->text();
        m_config.defaultMaxIterations = maxSpin->value();
        m_config.save();
        applyConfig();
        m_paramTol->setText(m_config.defaultTolerance);
        m_paramMax->setText(QString::number(m_config.defaultMaxIterations));
    };

    connect(applyBtn, &QPushButton::clicked, &dialog, applyValues);
    connect(saveBtn, &QPushButton::clicked, &dialog, [&]() {
        applyValues();
        dialog.accept();
    });
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}

void MainWindow::onExportTxt() {
    if (!ensureHasRecordsForExport()) return;
    QString defaultName = QString("root_report_%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString path = QFileDialog::getSaveFileName(this, "Export TXT Report", defaultName, "Text Files (*.txt)");
    if (path.isEmpty()) return;
    if (!path.endsWith(".txt", Qt::CaseInsensitive)) path += ".txt";

    QString err;
    bool ok = ExportManager::exportTxt(path, m_fnInput->text(), currentMethodName(),
                                       m_paramTol->text().toDouble(), m_paramMax->text().toInt(),
                                       m_records, &err);
    if (!ok) QMessageBox::warning(this, "Export Failed", err);
    else setStatus("STATUS: TXT EXPORTED", ThemeManager::palette(m_config.theme).accent.name());
}

void MainWindow::onExportCsv() {
    if (!ensureHasRecordsForExport()) return;
    QString defaultName = QString("root_iterations_%1.csv")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString path = QFileDialog::getSaveFileName(this, "Export CSV Data", defaultName, "CSV Files (*.csv)");
    if (path.isEmpty()) return;
    if (!path.endsWith(".csv", Qt::CaseInsensitive)) path += ".csv";

    QString err;
    bool ok = ExportManager::exportCsv(path, m_fnInput->text(), currentMethodName(),
                                       m_paramTol->text().toDouble(), m_paramMax->text().toInt(),
                                       m_records, &err);
    if (!ok) QMessageBox::warning(this, "Export Failed", err);
    else setStatus("STATUS: CSV EXPORTED", ThemeManager::palette(m_config.theme).accent.name());
}

SolverMethod MainWindow::currentMethod() const {
    switch (m_methodBox->currentIndex()) {
    case 1: return SolverMethod::Newton;
    case 2: return SolverMethod::Secant;
    default: return SolverMethod::Bisection;
    }
}

QString MainWindow::currentMethodName() const {
    switch (currentMethod()) {
    case SolverMethod::Newton: return "Newton-Raphson";
    case SolverMethod::Secant: return "Secant";
    default: return "Bisection";
    }
}

void MainWindow::setStatus(const QString& msg, const QString& color) {
    const QString c = color.isEmpty() ? ThemeManager::palette(m_config.theme).text.name() : color;
    m_lblStatus->setText(msg);
    m_lblStatus->setStyleSheet(
        QString("color:%1;border:1px solid %1;padding:3px 8px;"
                "letter-spacing:2px;font-size:%2px;")
                .arg(c)
                .arg(static_cast<int>(qMax(9.0, (m_config.fontSize - 1) * m_config.uiScale))));
}

void MainWindow::addIterRow(const IterationRecord& rec, bool highlight) {
    int row = m_iterTable->rowCount();
    m_iterTable->insertRow(row);

    auto mkItem = [](const QString& s) {
        auto* it = new QTableWidgetItem(s);
        it->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return it;
    };

    const ThemePalette pal = ThemeManager::palette(m_config.theme);
    QColor statusColor = pal.text;
    if (rec.status == "CONVERGED") statusColor = pal.accent;
    else if (rec.status == "INIT")  statusColor = pal.dim;

    m_iterTable->setItem(row, 0, mkItem(QString::number(rec.iteration)));
    m_iterTable->setItem(row, 1, mkItem(QString("%1").arg(rec.x,   14, 'f', 8)));
    m_iterTable->setItem(row, 2, mkItem(QString("%1").arg(rec.fx,  12, 'f', 6)));
    m_iterTable->setItem(row, 3, mkItem(QString("%1").arg(rec.error, 12, 'e', 4)));
    auto* stItem = mkItem(QString::fromStdString(rec.status));
    stItem->setForeground(statusColor);
    m_iterTable->setItem(row, 4, stItem);

    if (highlight) {
        for (int c = 0; c < 5; ++c) {
            if (auto* it = m_iterTable->item(row, c))
                it->setBackground(pal.panel);
        }
    }
}

void MainWindow::clearTable() {
    m_iterTable->setRowCount(0);
}

bool MainWindow::ensureHasRecordsForExport() {
    if (!m_records.empty()) return true;

    runCompute();
    if (!m_records.empty()) return true;

    QMessageBox::information(this, "Nothing to Export", "Solve the equation first before exporting.");
    return false;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (m_overlay && m_central)
        m_overlay->setGeometry(m_central->rect());
}
