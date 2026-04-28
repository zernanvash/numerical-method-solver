#include "SecantSolver.h"

#include <cmath>
#include <stdexcept>

std::vector<IterationRecord> SecantSolver::solve(
    const std::function<double(double)>& f,
    double x0,
    double x1,
    double tol,
    int maxIter
    ) {
    std::vector<IterationRecord> records;

    for (int i = 1; i <= maxIter; ++i) {
        double f0 = f(x0);
        double f1 = f(x1);
        double denom = f1 - f0;

        if (std::abs(denom) < 1e-14) {
            throw std::runtime_error("Division by near-zero in secant method");
        }

        double x2 = x1 - f1 * (x1 - x0) / denom;
        double err = std::abs(x2 - x1);

        IterationRecord rec;
        rec.iteration = i;
        rec.x = x2;
        rec.fx = f(x2);
        rec.error = err;
        rec.a = x0;
        rec.b = x1;
        rec.tangentSlope = denom / (x1 - x0);

        bool converged = (err < tol) || (std::abs(rec.fx) < tol);
        rec.status = converged ? "CONVERGED" : (i == 1 ? "INIT" : "UPDATE");

        records.push_back(rec);

        if (converged) {
            break;
        }

        x0 = x1;
        x1 = x2;

        if (i == maxIter) {
            records.back().status = "MAX_ITER";
        }
    }

    return records;
}