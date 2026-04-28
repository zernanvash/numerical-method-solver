#pragma once
#include <string>
#include <functional>
#include <stdexcept>

class FunctionParser {
public:
    // Parse expression string into callable f(x).
    // Supports: +  -  *  /  ^  sin  cos  tan  sqrt  log  exp  abs
    // Throws std::invalid_argument on parse error.
    static std::function<double(double)> parse(const std::string& expr);

    // Numerical derivative via central difference
    static double derivative(const std::function<double(double)>& f, double x,
                             double h = 1e-7);

    // Validate expression (returns empty string on success, error message on failure)
    static std::string validate(const std::string& expr);
};
