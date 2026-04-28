#pragma once
#include <string>

struct IterationRecord {
    int    iteration  = 0;
    double x          = 0.0;
    double fx         = 0.0;
    double error      = 0.0;
    double a          = 0.0;   // bisection: left bound  | newton/secant: x_{n-1}
    double b          = 0.0;   // bisection: right bound | newton/secant: x_n
    double tangentSlope = 0.0; // newton: f'(x)          | secant: slope of secant
    std::string status;        // "INIT" | "UPDATE" | "CONVERGED" | "MAX_ITER"
};
