#pragma once

#include "IterationRecord.h"
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
};
