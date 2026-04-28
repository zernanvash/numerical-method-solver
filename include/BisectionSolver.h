#pragma once
#include "IterationRecord.h"
#include "FunctionParser.h"
#include <vector>
#include <functional>
#include <string>

// ── Bisection ─────────────────────────────────────────────────────────────────
class BisectionSolver {
public:
    static std::vector<IterationRecord> solve(
        const std::function<double(double)>& f,
        double a, double b,
        double tol     = 1e-6,
        int    maxIter = 100);
};

// ── Newton-Raphson ────────────────────────────────────────────────────────────
class NewtonSolver {
public:
    static std::vector<IterationRecord> solve(
        const std::function<double(double)>& f,
        double x0,
        double tol     = 1e-6,
        int    maxIter = 100);
};

// ── Secant ────────────────────────────────────────────────────────────────────
class SecantSolver {
public:
    static std::vector<IterationRecord> solve(
        const std::function<double(double)>& f,
        double x0, double x1,
        double tol     = 1e-6,
        int    maxIter = 100);
};
