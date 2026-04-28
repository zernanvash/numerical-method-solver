#include "MainWindow.h"
#include "FunctionParser.h"
#include "BisectionSolver.h"
#include "NewtonSolver.h"
#include "SecantSolver.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QResizeEvent>
#include <QFont>
#include <QApplication>

// ── QSS theme (amber CRT) ─────────────────────────────────────────────────────
static const char* QSS = R"(
QWidget {
    background-color: #0A0800;
    color: #FFA800;
    font-family: "Courier New";
    font-size: 12px;
}
QMainWindow, QWidget#central {
    background-color: #0A0800;
}
QGroupBox {
    background-color: #100D00;
    border: 1px solid #3A2800;
    border-radius: 2px;
    margin-top: 14px;
    font-size: 11px;
    color: #7A5800;
    letter-spacing: 2px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 8px;
    top: 2px;
    color: #7A5800;
}
QLineEdit {
    background-color: #050300;
    color: #FFA800;
    border: 1px solid #4A3400;
    border-radius: 0px;
    padding: 4px 6px;
    selection-background-color: #3A2800;
}
QLineEdit:focus {
    border: 1px solid #FFA800;
}
QPushButton {
    background-color: #0A0800;
    color: #FFA800;
    border: 1px solid #4A3400;
    border-radius: 0px;
    padding: 6px 4px;
    letter-spacing: 2px;
}
QPushButton:hover {
    background-color: #1A1200;
    border: 1px solid #FFA800;
    color: #FFD080;
}
QPushButton:pressed {
    background-color: #2A1C00;
}
QPushButton#btnSolve {
    color: #FFC840;
    border: 1px solid #FFA800;
    font-size: 13px;
}
QPushButton#btnStep {
    color: #A0FF60;
    border: 1px solid #60AA30;
}
QPushButton#btnReset {
    color: #7A5800;
    border: 1px solid #3A2800;
}
QComboBox {
    background-color: #050300;
    color: #FFA800;
    border: 1px solid #4A3400;
    border-radius: 0px;
    padding: 4px 6px;
}
QComboBox QAbstractItemView {
    background-color: #100D00;
    color: #FFA800;
    border: 1px solid #4A3400;
    selection-background-color: #3A2800;
}
QComboBox::drop-down {
    border: none;
}
QLabel#statusLabel {
    color: #FFA800;
    border: 1px solid #3A2800;
    padding: 3px 8px;
    letter-spacing: 2px;
    font-size: 11px;
}
QLabel#rootLabel {
    color: #FFE060;
    font-size: 13px;
    letter-spacing: 1px;
}
QLabel#methodInfo {
    color: #6A5000;
    font-size: 10px;
    line-height: 160%;
}
QTableWidget {
    background-color: #050300;
    color: #FFA800;
    border: 1px solid #2A1E00;
    gridline-color: #1A1200;
    font-size: 11px;
    font-family: "Courier New";
}
QTableWidget::item {
    padding: 2px 6px;
    border-bottom: 1px solid #1A1200;
}
QTableWidget::item:selected {
    background-color: #2A1E00;
    color: #FFD080;
}
QHeaderView::section {
    background-color: #100D00;
    color: #7A5800;
    border: 1px solid #2A1E00;
    padding: 3px 6px;
    font-size: 10px;
    letter-spacing: 1px;
}
QScrollBar:vertical {
    background: #050300;
    width: 8px;
    border: 1px solid #2A1E00;
}
QScrollBar::handle:vertical {
    background: #3A2800;
    min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
)";

// ── Method descriptions ───────────────────────────────────────────────────────
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

// ── Constructor ───────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("ROOT-FINDING COMPUTATION CONSOLE v1.0");
    setMinimumSize(900, 620);

    m_stepTimer = new QTimer(this);
    m_stepTimer->setInterval(600);
    connect(m_stepTimer, &QTimer::timeout, this, &MainWindow::onStepTimerTick);

    buildUI();
    applyStyleSheet();
    updateMethodLabels();
}

// ── UI construction ───────────────────────────────────────────────────────────

void MainWindow::buildUI() {
    m_central = new QWidget;
    m_central->setObjectName("central");
    setCentralWidget(m_central);

    // ── Header bar ─────────────────────────────────────────────────────────
    QHBoxLayout* headerRow = new QHBoxLayout;
    QLabel* title = new QLabel("ROOT-FINDING COMPUTATION CONSOLE  v1.0");
    title->setStyleSheet("color:#FFA800;font-size:13px;letter-spacing:3px;font-weight:bold;");
    m_lblStatus = new QLabel("STATUS: READY");
    m_lblStatus->setObjectName("statusLabel");
    m_lblStatus->setStyleSheet("color:#FFA800;border:1px solid #4A3400;padding:3px 8px;letter-spacing:2px;font-size:11px;");
    headerRow->addWidget(title);
    headerRow->addStretch();
    headerRow->addWidget(m_lblStatus);

    // ── Left control panel ─────────────────────────────────────────────────
    QVBoxLayout* leftPanel = new QVBoxLayout;
    leftPanel->setSpacing(8);

    // Function input
    QGroupBox* gbFn = new QGroupBox("FUNCTION");
    QVBoxLayout* fnLayout = new QVBoxLayout(gbFn);
    m_fnInput = new QLineEdit("x^3 - x - 2");
    m_fnInput->setPlaceholderText("e.g. x^3 - x - 2");
    connect(m_fnInput, &QLineEdit::textChanged, this, &MainWindow::onFunctionChanged);
    fnLayout->addWidget(m_fnInput);
    leftPanel->addWidget(gbFn);

    // Method selector
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

    // Parameters
    QGroupBox* gbParams = new QGroupBox("PARAMETERS");
    QGridLayout* paramGrid = new QGridLayout(gbParams);
    paramGrid->setSpacing(5);

    auto makeParamRow = [&](QGridLayout* g, int row,
                             const QString& lbl, QLineEdit*& le, const QString& val) {
        g->addWidget(new QLabel(lbl), row, 0);
        le = new QLineEdit(val);
        le->setFixedWidth(80);
        g->addWidget(le, row, 1);
    };

    QLabel* lblA = new QLabel("a  >");
    lblA->setObjectName("labelA");
    m_paramA = new QLineEdit("1");
    m_paramA->setFixedWidth(80);
    paramGrid->addWidget(lblA, 0, 0);
    paramGrid->addWidget(m_paramA, 0, 1);

    QLabel* lblB = new QLabel("b  >");
    lblB->setObjectName("labelB");
    m_paramB = new QLineEdit("2");
    m_paramB->setFixedWidth(80);
    paramGrid->addWidget(lblB, 1, 0);
    paramGrid->addWidget(m_paramB, 1, 1);

    makeParamRow(paramGrid, 2, "tol >", m_paramTol, "0.0001");
    makeParamRow(paramGrid, 3, "max >", m_paramMax, "50");

    leftPanel->addWidget(gbParams);

    // Result display
    QGroupBox* gbResult = new QGroupBox("RESULT");
    QVBoxLayout* resLayout = new QVBoxLayout(gbResult);
    m_lblRoot = new QLabel("ROOT: —");
    m_lblRoot->setObjectName("rootLabel");
    resLayout->addWidget(m_lblRoot);
    leftPanel->addWidget(gbResult);

    // Action buttons
    m_btnSolve = new QPushButton("[ SOLVE ]");
    m_btnSolve->setObjectName("btnSolve");
    m_btnStep  = new QPushButton("[ STEP ]");
    m_btnStep->setObjectName("btnStep");
    m_btnReset = new QPushButton("[ RESET ]");
    m_btnReset->setObjectName("btnReset");
    connect(m_btnSolve, &QPushButton::clicked, this, &MainWindow::onSolve);
    connect(m_btnStep,  &QPushButton::clicked, this, &MainWindow::onStep);
    connect(m_btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    leftPanel->addWidget(m_btnSolve);
    leftPanel->addWidget(m_btnStep);
    leftPanel->addWidget(m_btnReset);
    leftPanel->addStretch();

    // ── Graph area ─────────────────────────────────────────────────────────
    m_graph = new GraphWidget;

    // ── Iteration table ────────────────────────────────────────────────────
    m_iterTable = new QTableWidget(0, 5);
    m_iterTable->setHorizontalHeaderLabels({"ITER","X VALUE","f(X)","ERROR","STATUS"});
    m_iterTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_iterTable->verticalHeader()->setVisible(false);
    m_iterTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_iterTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_iterTable->setMaximumHeight(170);

    // ── Right column ───────────────────────────────────────────────────────
    QVBoxLayout* rightPanel = new QVBoxLayout;
    rightPanel->addWidget(m_graph, 1);
    rightPanel->addWidget(m_iterTable);

    // ── Main layout ────────────────────────────────────────────────────────
    QWidget* leftWidget = new QWidget;
    leftWidget->setLayout(leftPanel);
    leftWidget->setFixedWidth(230);

    QHBoxLayout* bodyRow = new QHBoxLayout;
    bodyRow->addWidget(leftWidget);
    bodyRow->addLayout(rightPanel, 1);

    QVBoxLayout* root = new QVBoxLayout(m_central);
    root->setContentsMargins(8, 6, 8, 6);
    root->setSpacing(6);
    root->addLayout(headerRow);
    root->addLayout(bodyRow, 1);

    // CRT overlay (on top of everything)
    m_overlay = new CRTOverlay(m_central);
    m_overlay->setGeometry(m_central->rect());
    m_overlay->raise();
}

void MainWindow::applyStyleSheet() {
    qApp->setStyleSheet(QSS);
}

// ── Slots ─────────────────────────────────────────────────────────────────────

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
    // Parse function
    std::string err = FunctionParser::validate(m_fnInput->text().toStdString());
    if (!err.empty()) {
        setStatus("STATUS: INVALID INPUT", "#FF6B5E");
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
        setStatus(QString("STATUS: ERR: %1").arg(ex.what()).left(40), "#FF6B5E");
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

    setStatus("STATUS: PROCESSING", "#FFC857");
    clearTable();

    // Show all iterations in table immediately
    for (auto& r : m_records) addIterRow(r, false);
    // Highlight last
    if (!m_records.empty()) {
        int lastRow = m_iterTable->rowCount() - 1;
        for (int c = 0; c < 5; ++c) {
            if (auto* item = m_iterTable->item(lastRow, c))
                item->setForeground(QColor("#FFE060"));
        }
        m_iterTable->scrollToBottom();
    }

    const auto& last = m_records.back();
    if (last.status == "CONVERGED") {
        setStatus("STATUS: CONVERGED", "#7EE7FF");
        m_lblRoot->setText(QString("ROOT: x = %1\nf(x) = %2")
                           .arg(last.x, 0, 'f', 8)
                           .arg(last.fx, 0, 'e', 4));
    } else {
        setStatus("STATUS: MAX ITER REACHED", "#FFC857");
        m_lblRoot->setText(QString("BEST: x ≈ %1").arg(last.x, 0, 'f', 8));
    }

    // Trigger scanline reveal of full graph
    m_graph->showUpToIteration(static_cast<int>(m_records.size()));
}

void MainWindow::onStep() {
    if (m_records.empty()) {
        runCompute();
        if (m_records.empty()) return;
        clearTable();
        setStatus("STATUS: ITERATING", "#FFC857");
    }
    if (m_stepIndex >= static_cast<int>(m_records.size())) return;

    ++m_stepIndex;
    const auto& rec = m_records[m_stepIndex - 1];
    addIterRow(rec, true);
    m_iterTable->scrollToBottom();

    // Reveal graph up to this iteration via scanline
    m_graph->showUpToIteration(m_stepIndex);

    // Update status
    if (rec.status == "CONVERGED") {
        setStatus("STATUS: CONVERGED", "#7EE7FF");
        m_lblRoot->setText(QString("ROOT: x = %1\nf(x) = %2")
                           .arg(rec.x, 0, 'f', 8)
                           .arg(rec.fx, 0, 'e', 4));
    } else {
        setStatus(QString("STATUS: ITERATING [%1/%2]")
                  .arg(m_stepIndex).arg(m_records.size()), "#FFC857");
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
    setStatus("STATUS: READY", "#72FF8F");
    m_graph->reset();
}

void MainWindow::onStepTimerTick() {
    if (m_stepIndex >= static_cast<int>(m_records.size())) {
        m_stepTimer->stop();
        return;
    }
    onStep();
}

// ── Helpers ───────────────────────────────────────────────────────────────────

SolverMethod MainWindow::currentMethod() const {
    switch (m_methodBox->currentIndex()) {
    case 1: return SolverMethod::Newton;
    case 2: return SolverMethod::Secant;
    default: return SolverMethod::Bisection;
    }
}

void MainWindow::setStatus(const QString& msg, const QString& color) {
    m_lblStatus->setText(msg);
    m_lblStatus->setStyleSheet(
        QString("color:%1;border:1px solid %2;padding:3px 8px;"
                "letter-spacing:2px;font-size:11px;").arg(color, color));
}

void MainWindow::addIterRow(const IterationRecord& rec, bool highlight) {
    int row = m_iterTable->rowCount();
    m_iterTable->insertRow(row);

    auto mkItem = [](const QString& s) {
        auto* it = new QTableWidgetItem(s);
        it->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return it;
    };

    QString statusColor = "#FFA800";
    if (rec.status == "CONVERGED") statusColor = "#7EE7FF";
    else if (rec.status == "INIT")  statusColor = "#FFC857";

    m_iterTable->setItem(row, 0, mkItem(QString::number(rec.iteration)));
    m_iterTable->setItem(row, 1, mkItem(QString("%1").arg(rec.x,   14, 'f', 8)));
    m_iterTable->setItem(row, 2, mkItem(QString("%1").arg(rec.fx,  12, 'f', 6)));
    m_iterTable->setItem(row, 3, mkItem(QString("%1").arg(rec.error, 12, 'e', 4)));
    auto* stItem = mkItem(QString::fromStdString(rec.status));
    stItem->setForeground(QColor(statusColor));
    m_iterTable->setItem(row, 4, stItem);

    if (highlight) {
        for (int c = 0; c < 5; ++c) {
            if (auto* it = m_iterTable->item(row, c))
                it->setBackground(QColor("#1A1200"));
        }
    }
}

void MainWindow::clearTable() {
    m_iterTable->setRowCount(0);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (m_overlay && m_central)
        m_overlay->setGeometry(m_central->rect());
}
