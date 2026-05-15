#pragma once

#include "IterationRecord.h"
#include "OdeIterationRecord.h"
#include <QString>
#include <vector>

class ExportManager {
public:
    static bool exportTxt(const QString& filePath,
                          const QString& functionText,
                          const QString& methodText,
                          double tolerance,
                          int maxIterations,
                          const std::vector<IterationRecord>& records,
                          QString* errorMessage = nullptr);

    static bool exportCsv(const QString& filePath,
                          const QString& functionText,
                          const QString& methodText,
                          double tolerance,
                          int maxIterations,
                          const std::vector<IterationRecord>& records,
                          QString* errorMessage = nullptr);

    static bool exportOdeTxt(const QString& filePath,
                             const QString& expressionText,
                             const QString& methodText,
                             double t0,
                             double y0,
                             double h,
                             int steps,
                             const std::vector<OdeIterationRecord>& records,
                             QString* errorMessage = nullptr);

    static bool exportOdeCsv(const QString& filePath,
                             const QString& expressionText,
                             const QString& methodText,
                             double t0,
                             double y0,
                             double h,
                             int steps,
                             const std::vector<OdeIterationRecord>& records,
                             QString* errorMessage = nullptr);
};
