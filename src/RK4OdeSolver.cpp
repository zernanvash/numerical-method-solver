#include "RK4OdeSolver.h"
#include "OdeFunctionParser.h"

#include <cmath>
#include <stdexcept>

namespace {

void validateInputs(const QString& expression, double t0, double y0, double h, int steps) {
    if (expression.trimmed().isEmpty()) {
        throw std::invalid_argument("Empty ODE expression");
    }
    if (!std::isfinite(t0) || !std::isfinite(y0) || !std::isfinite(h)) {
        throw std::invalid_argument("Initial values and step size must be finite");
    }
    if (h == 0.0) {
        throw std::invalid_argument("Step size h cannot be zero");
    }
    if (steps <= 0) {
        throw std::invalid_argument("Steps must be greater than zero");
    }
}

bool validNumber(double value) {
    return std::isfinite(value);
}

} // namespace

std::vector<OdeIterationRecord> RK4OdeSolver::solve(const QString& expression,
                                                    double t0,
                                                    double y0,
                                                    double h,
                                                    int steps) {
    validateInputs(expression, t0, y0, h, steps);

    auto f = OdeFunctionParser::parse(expression.toStdString());
    std::vector<OdeIterationRecord> records;
    records.reserve(static_cast<size_t>(steps));

    double t = t0;
    double y = y0;

    for (int i = 0; i < steps; ++i) {
        OdeIterationRecord rec;
        rec.iteration = i + 1;
        rec.t = t;
        rec.y = y;

        rec.k1 = f(t, y);
        rec.k2 = f(t + h / 2.0, y + h * rec.k1 / 2.0);
        rec.k3 = f(t + h / 2.0, y + h * rec.k2 / 2.0);
        rec.k4 = f(t + h, y + h * rec.k3);
        rec.slope = rec.k1;

        rec.nextT = t + h;
        rec.nextY = y + (h / 6.0) * (rec.k1 + 2.0 * rec.k2 + 2.0 * rec.k3 + rec.k4);

        if (!validNumber(rec.k1) || !validNumber(rec.k2) || !validNumber(rec.k3) ||
            !validNumber(rec.k4) || !validNumber(rec.nextT) || !validNumber(rec.nextY)) {
            rec.status = "ERROR: NaN/INF";
            records.push_back(rec);
            break;
        }

        rec.status = "UPDATE";
        records.push_back(rec);

        t = rec.nextT;
        y = rec.nextY;
    }

    return records;
}
