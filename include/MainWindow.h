#pragma once
#include "GraphWidget.h"
#include "IterationRecord.h"
#include "CRTOverlay.h"
#include "AppConfig.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QScrollArea>
#include <QSplitter>
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
    void onStepTimerTick();
    void onSettings();
    void onExportTxt();
    void onExportCsv();

private:
    QWidget*      m_central = nullptr;
    QWidget*      m_leftWidget = nullptr;
    QScrollArea*  m_leftScroll = nullptr;
    QSplitter*    m_bodySplitter = nullptr;
    QLabel*       m_titleLabel = nullptr;
    QLineEdit*    m_fnInput = nullptr;
    QComboBox*    m_methodBox = nullptr;
    QLineEdit*    m_paramA = nullptr;
    QLineEdit*    m_paramB = nullptr;
    QLineEdit*    m_paramTol = nullptr;
    QLineEdit*    m_paramMax = nullptr;
    QPushButton*  m_btnSolve = nullptr;
    QPushButton*  m_btnStep = nullptr;
    QPushButton*  m_btnReset = nullptr;
    QPushButton*  m_btnSettings = nullptr;
    QPushButton*  m_btnExportTxt = nullptr;
    QPushButton*  m_btnExportCsv = nullptr;
    QLabel*       m_lblStatus = nullptr;
    QLabel*       m_lblRoot = nullptr;
    QLabel*       m_lblMethodInfo = nullptr;
    QTableWidget* m_iterTable = nullptr;
    GraphWidget*  m_graph = nullptr;
    CRTOverlay*   m_overlay = nullptr;

    std::vector<IterationRecord>  m_records;
    std::function<double(double)> m_func;
    int                           m_stepIndex = 0;
    QTimer*                       m_stepTimer = nullptr;
    bool                          m_autoStepping = false;
    AppConfig                     m_config;

    void buildUI();
    void applyStyleSheet();
    void applyConfig();
    void setStatus(const QString& msg, const QString& color = QString());
    void addIterRow(const IterationRecord& rec, bool highlight = false);
    void clearTable();
    void updateMethodLabels();
    SolverMethod currentMethod() const;
    QString currentMethodName() const;
    void runCompute();
    bool ensureHasRecordsForExport();
};
