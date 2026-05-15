#pragma once

#include <QString>

struct OdeIterationRecord {
    int iteration = 0;

    double t = 0.0;
    double y = 0.0;
    double slope = 0.0;

    double k1 = 0.0;
    double k2 = 0.0;
    double k3 = 0.0;
    double k4 = 0.0;

    double nextT = 0.0;
    double nextY = 0.0;

    QString status;
};
