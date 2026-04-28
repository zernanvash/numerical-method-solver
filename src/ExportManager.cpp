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
