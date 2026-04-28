#include "NewtonSolver.h"
#include "FunctionParser.h"

#include <cmath>
#include <stdexcept>
#include <string>

std::vector<IterationRecord> NewtonSolver::solve(
    const std::function<double(double)>& f,
    double x0,
    double tol,
    int maxIter
    ) {
    std::vector<IterationRecord> records;

    double x = x0;

    for (int i = 1; i <= maxIter; ++i) {
        double fx = f(x);
        double dfx = FunctionParser::derivative(f, x);

        if (std::abs(dfx) < 1e-14) {
            throw std::runtime_error(
                "Derivative too close to zero at x = " + std::to_string(x)
                );
        }

        double xn = x - fx / dfx;
        double err = std::abs(xn - x);

        IterationRecord rec;
        rec.iteration = i;
        rec.x = xn;
        rec.fx = f(xn);
        rec.error = err;
        rec.a = x;
        rec.b = xn;
        rec.tangentSlope = dfx;

        bool converged = (err < tol) || (std::abs(rec.fx) < tol);
        rec.status = converged ? "CONVERGED" : (i == 1 ? "INIT" : "UPDATE");

        records.push_back(rec);

        if (converged) {
            break;
        }

        x = xn;

        if (i == maxIter) {
            records.back().status = "MAX_ITER";
        }
    }

    return records;
}