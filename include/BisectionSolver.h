#pragma once

#include "IterationRecord.h"
#include <vector>
#include <functional>

class BisectionSolver {
public:
    static std::vector<IterationRecord> solve(
        const std::function<double(double)>& f,
        double a,
        double b,
        double tol = 1e-6,
        int maxIter = 100
        );
};