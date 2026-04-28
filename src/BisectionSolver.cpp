#include "BisectionSolver.h"
#include <cmath>
#include <stdexcept>

std::vector<IterationRecord> BisectionSolver::solve(
    const std::function<double(double)>& f,
    double a, double b, double tol, int maxIter)
{
    std::vector<IterationRecord> records;

    double fa = f(a);
    double fb = f(b);
    if (fa * fb > 0.0)
        throw std::invalid_argument("f(a) and f(b) must have opposite signs");

    for (int i = 1; i <= maxIter; ++i) {
        double c  = (a + b) / 2.0;
        double fc = f(c);
        double err = (b - a) / 2.0;

        IterationRecord rec;
        rec.iteration = i;
        rec.x         = c;
        rec.fx        = fc;
        rec.error     = std::abs(err);
        rec.a         = a;
        rec.b         = b;

        bool converged = (std::abs(fc) < tol) || (std::abs(err) < tol);
        rec.status     = converged ? "CONVERGED" : (i == 1 ? "INIT" : "UPDATE");
        records.push_back(rec);

        if (converged) break;

        if (fa * fc < 0.0) { b = c; fb = fc; }
        else               { a = c; fa = fc; }

        if (i == maxIter) records.back().status = "MAX_ITER";
    }
    return records;
}

// ── Newton-Raphson ────────────────────────────────────────────────────────────
std::vector<IterationRecord> NewtonSolver::solve(
    const std::function<double(double)>& f,
    double x0, double tol, int maxIter)
{
    std::vector<IterationRecord> records;
    double x = x0;

    for (int i = 1; i <= maxIter; ++i) {
        double fx  = f(x);
        double dfx = FunctionParser::derivative(f, x);

        if (std::abs(dfx) < 1e-14)
            throw std::runtime_error("Derivative too close to zero at x = " +
                                     std::to_string(x));

        double xn  = x - fx / dfx;
        double err = std::abs(xn - x);

        IterationRecord rec;
        rec.iteration    = i;
        rec.x            = xn;
        rec.fx           = f(xn);
        rec.error        = err;
        rec.a            = x;   // previous x
        rec.b            = xn;  // new x
        rec.tangentSlope = dfx;

        bool converged = (err < tol) || (std::abs(rec.fx) < tol);
        rec.status     = converged ? "CONVERGED" : (i == 1 ? "INIT" : "UPDATE");
        records.push_back(rec);

        if (converged) break;
        x = xn;
        if (i == maxIter) records.back().status = "MAX_ITER";
    }
    return records;
}

// ── Secant ────────────────────────────────────────────────────────────────────
std::vector<IterationRecord> SecantSolver::solve(
    const std::function<double(double)>& f,
    double x0, double x1, double tol, int maxIter)
{
    std::vector<IterationRecord> records;

    for (int i = 1; i <= maxIter; ++i) {
        double f0 = f(x0);
        double f1 = f(x1);
        double denom = f1 - f0;

        if (std::abs(denom) < 1e-14)
            throw std::runtime_error("Division by near-zero in secant method");

        double x2  = x1 - f1 * (x1 - x0) / denom;
        double err = std::abs(x2 - x1);

        IterationRecord rec;
        rec.iteration    = i;
        rec.x            = x2;
        rec.fx           = f(x2);
        rec.error        = err;
        rec.a            = x0;
        rec.b            = x1;
        rec.tangentSlope = denom / (x1 - x0); // slope of secant line

        bool converged = (err < tol) || (std::abs(rec.fx) < tol);
        rec.status     = converged ? "CONVERGED" : (i == 1 ? "INIT" : "UPDATE");
        records.push_back(rec);

        if (converged) break;
        x0 = x1;
        x1 = x2;
        if (i == maxIter) records.back().status = "MAX_ITER";
    }
    return records;
}
