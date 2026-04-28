#include "BisectionSolver.h"

#include <cmath>
#include <stdexcept>

std::vector<IterationRecord> BisectionSolver::solve(
    const std::function<double(double)>& f,
    double a,
    double b,
    double tol,
    int maxIter
    ) {
    std::vector<IterationRecord> records;

    double fa = f(a);
    double fb = f(b);

    if (fa * fb > 0.0) {
        throw std::invalid_argument("f(a) and f(b) must have opposite signs");
    }

    for (int i = 1; i <= maxIter; ++i) {
        double c = (a + b) / 2.0;
        double fc = f(c);
        double err = (b - a) / 2.0;

        IterationRecord rec;
        rec.iteration = i;
        rec.x = c;
        rec.fx = fc;
        rec.error = std::abs(err);
        rec.a = a;
        rec.b = b;

        bool converged = (std::abs(fc) < tol) || (std::abs(err) < tol);
        rec.status = converged ? "CONVERGED" : (i == 1 ? "INIT" : "UPDATE");

        records.push_back(rec);

        if (converged) {
            break;
        }

        if (fa * fc < 0.0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }

        if (i == maxIter) {
            records.back().status = "MAX_ITER";
        }
    }

    return records;
}