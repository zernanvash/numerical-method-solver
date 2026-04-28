#pragma once
#include "GraphWidget.h"
#include "IterationRecord.h"
#include "CRTOverlay.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <vector>
#include <functional>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onSolve();
    void onStep();
    void onReset();
    void onMethodChanged(int index);
    void onFunctionChanged(const QString& text);
    void onStepTimerTick();   // for auto-playback animation

private:
    // ── Widgets ───────────────────────────────────────────────────────────────
    QWidget*      m_central;
    QLineEdit*    m_fnInput;
    QComboBox*    m_methodBox;
    QLineEdit*    m_paramA;
    QLineEdit*    m_paramB;
    QLineEdit*    m_paramTol;
    QLineEdit*    m_paramMax;
    QPushButton*  m_btnSolve;
    QPushButton*  m_btnStep;
    QPushButton*  m_btnReset;
    QLabel*       m_lblStatus;
    QLabel*       m_lblRoot;
    QLabel*       m_lblMethodInfo;
    QTableWidget* m_iterTable;
    GraphWidget*  m_graph;
    CRTOverlay*   m_overlay;

    // ── State ─────────────────────────────────────────────────────────────────
    std::vector<IterationRecord>  m_records;
    std::function<double(double)> m_func;
    int                           m_stepIndex = 0;
    QTimer*                       m_stepTimer;
    bool                          m_autoStepping = false;

    // ── Helpers ───────────────────────────────────────────────────────────────
    void buildUI();
    void applyStyleSheet();
    void setStatus(const QString& msg, const QString& color = "#72FF8F");
    void addIterRow(const IterationRecord& rec, bool highlight = false);
    void clearTable();
    void updateMethodLabels();
    SolverMethod currentMethod() const;
    void runCompute();
};
