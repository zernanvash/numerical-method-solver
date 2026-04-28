#pragma once

#include "IterationRecord.h"
#include <vector>
#include <functional>

class NewtonSolver {
public:
    static std::vector<IterationRecord> solve(
        const std::function<double(double)>& f,
        double x0,
        double tol = 1e-6,
        int maxIter = 100
        );
};