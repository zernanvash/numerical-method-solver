#pragma once

#include "OdeIterationRecord.h"

#include <QString>
#include <vector>

class RK4OdeSolver {
public:
    static std::vector<OdeIterationRecord> solve(const QString& expression,
                                                 double t0,
                                                 double y0,
                                                 double h,
                                                 int steps);
};
