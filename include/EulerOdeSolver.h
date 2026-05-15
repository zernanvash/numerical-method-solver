#pragma once

#include "OdeIterationRecord.h"

#include <QString>
#include <vector>

class EulerOdeSolver {
public:
    static std::vector<OdeIterationRecord> solve(const QString& expression,
                                                 double t0,
                                                 double y0,
                                                 double h,
                                                 int steps);
};
