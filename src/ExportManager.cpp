#include "ExportManager.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QLocale>

static QString csvEscape(QString s) {
    s.replace('"', "\"\"");
    return '"' + s + '"';
}

bool ExportManager::exportTxt(const QString& filePath,
                              const QString& functionText,
                              const QString& methodText,
                              double tolerance,
                              int maxIterations,
                              const std::vector<IterationRecord>& records,
                              QString* errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << "ROOT-FINDING COMPUTATION REPORT\n";
    out << "================================\n\n";
    out << "Generated: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "Function : " << functionText << "\n";
    out << "Method   : " << methodText << "\n";
    out << "Tolerance: " << tolerance << "\n";
    out << "Max Iter : " << maxIterations << "\n\n";

    if (!records.empty()) {
        const auto& last = records.back();
        out << "Final Root : " << QString::number(last.x, 'f', 10) << "\n";
        out << "Final f(x) : " << QString::number(last.fx, 'e', 6) << "\n";
        out << "Final Error: " << QString::number(last.error, 'e', 6) << "\n";
        out << "Status     : " << QString::fromStdString(last.status) << "\n\n";
    }

    out << "ITER | X VALUE        | f(X)          | ERROR         | STATUS\n";
    out << "----------------------------------------------------------------\n";
    for (const auto& r : records) {
        out << QString("%1").arg(r.iteration, 4) << " | "
            << QString("%1").arg(r.x, 14, 'f', 8) << " | "
            << QString("%1").arg(r.fx, 13, 'e', 5) << " | "
            << QString("%1").arg(r.error, 13, 'e', 5) << " | "
            << QString::fromStdString(r.status) << "\n";
    }

    return true;
}

bool ExportManager::exportCsv(const QString& filePath,
                              const QString& functionText,
                              const QString& methodText,
                              double tolerance,
                              int maxIterations,
                              const std::vector<IterationRecord>& records,
                              QString* errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << "Function," << csvEscape(functionText) << "\n";
    out << "Method," << csvEscape(methodText) << "\n";
    out << "Tolerance," << tolerance << "\n";
    out << "Max Iterations," << maxIterations << "\n";
    out << "Generated," << csvEscape(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")) << "\n\n";

    out << "Iteration,X Value,f(x),Error,A,B,Tangent/Secant Slope,Status\n";
    for (const auto& r : records) {
        out << r.iteration << ','
            << QString::number(r.x, 'g', 17) << ','
            << QString::number(r.fx, 'g', 17) << ','
            << QString::number(r.error, 'g', 17) << ','
            << QString::number(r.a, 'g', 17) << ','
            << QString::number(r.b, 'g', 17) << ','
            << QString::number(r.tangentSlope, 'g', 17) << ','
            << csvEscape(QString::fromStdString(r.status)) << "\n";
    }

    return true;
}

bool ExportManager::exportOdeTxt(const QString& filePath,
                                 const QString& expressionText,
                                 const QString& methodText,
                                 double t0,
                                 double y0,
                                 double h,
                                 int steps,
                                 const std::vector<OdeIterationRecord>& records,
                                 QString* errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << "ODE SIMULATION REPORT\n";
    out << "=====================\n\n";
    out << "Generated              : " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "Method                 : " << methodText << "\n";
    out << "Differential equation  : dy/dt = " << expressionText << "\n";
    out << "Initial condition      : y(" << t0 << ") = " << y0 << "\n";
    out << "Step size              : " << h << "\n";
    out << "Steps                  : " << steps << "\n";

    if (!records.empty()) {
        const auto& last = records.back();
        out << "Final t                : " << QString::number(last.nextT, 'f', 10) << "\n";
        out << "Final y                : " << QString::number(last.nextY, 'f', 10) << "\n";
    }

    out << "\n";
    if (methodText.contains("RK4", Qt::CaseInsensitive)) {
        out << "ITER | t              | y              | k1             | k2             | k3             | k4             | NEXT t         | NEXT y         | STATUS\n";
        out << "-------------------------------------------------------------------------------------------------------------------------------------------------\n";
        for (const auto& r : records) {
            out << QString("%1").arg(r.iteration, 4) << " | "
                << QString("%1").arg(r.t, 14, 'f', 8) << " | "
                << QString("%1").arg(r.y, 14, 'f', 8) << " | "
                << QString("%1").arg(r.k1, 14, 'f', 8) << " | "
                << QString("%1").arg(r.k2, 14, 'f', 8) << " | "
                << QString("%1").arg(r.k3, 14, 'f', 8) << " | "
                << QString("%1").arg(r.k4, 14, 'f', 8) << " | "
                << QString("%1").arg(r.nextT, 14, 'f', 8) << " | "
                << QString("%1").arg(r.nextY, 14, 'f', 8) << " | "
                << r.status << "\n";
        }
    } else {
        out << "ITER | t              | y              | f(t,y)         | NEXT t         | NEXT y         | STATUS\n";
        out << "-----------------------------------------------------------------------------------------------------\n";
        for (const auto& r : records) {
            out << QString("%1").arg(r.iteration, 4) << " | "
                << QString("%1").arg(r.t, 14, 'f', 8) << " | "
                << QString("%1").arg(r.y, 14, 'f', 8) << " | "
                << QString("%1").arg(r.slope, 14, 'f', 8) << " | "
                << QString("%1").arg(r.nextT, 14, 'f', 8) << " | "
                << QString("%1").arg(r.nextY, 14, 'f', 8) << " | "
                << r.status << "\n";
        }
    }

    out << "\nInterpretation: each row advances the initial value problem one step; Next y is the approximation carried into the next row.\n";
    return true;
}

bool ExportManager::exportOdeCsv(const QString& filePath,
                                 const QString& expressionText,
                                 const QString& methodText,
                                 double t0,
                                 double y0,
                                 double h,
                                 int steps,
                                 const std::vector<OdeIterationRecord>& records,
                                 QString* errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << "Differential Equation," << csvEscape("dy/dt = " + expressionText) << "\n";
    out << "Method," << csvEscape(methodText) << "\n";
    out << "Initial t," << t0 << "\n";
    out << "Initial y," << y0 << "\n";
    out << "Step Size," << h << "\n";
    out << "Steps," << steps << "\n";
    out << "Generated," << csvEscape(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")) << "\n\n";

    if (methodText.contains("RK4", Qt::CaseInsensitive)) {
        out << "Iteration,t,y,k1,k2,k3,k4,Next t,Next y,Status\n";
        for (const auto& r : records) {
            out << r.iteration << ','
                << QString::number(r.t, 'g', 17) << ','
                << QString::number(r.y, 'g', 17) << ','
                << QString::number(r.k1, 'g', 17) << ','
                << QString::number(r.k2, 'g', 17) << ','
                << QString::number(r.k3, 'g', 17) << ','
                << QString::number(r.k4, 'g', 17) << ','
                << QString::number(r.nextT, 'g', 17) << ','
                << QString::number(r.nextY, 'g', 17) << ','
                << csvEscape(r.status) << "\n";
        }
    } else {
        out << "Iteration,t,y,f(t,y),Next t,Next y,Status\n";
        for (const auto& r : records) {
            out << r.iteration << ','
                << QString::number(r.t, 'g', 17) << ','
                << QString::number(r.y, 'g', 17) << ','
                << QString::number(r.slope, 'g', 17) << ','
                << QString::number(r.nextT, 'g', 17) << ','
                << QString::number(r.nextY, 'g', 17) << ','
                << csvEscape(r.status) << "\n";
        }
    }

    return true;
}
