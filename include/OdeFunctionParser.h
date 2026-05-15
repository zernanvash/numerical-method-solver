#pragma once

#include <functional>
#include <string>

class OdeFunctionParser {
public:
    static std::function<double(double, double)> parse(const std::string& expr);
    static std::string validate(const std::string& expr);
};
