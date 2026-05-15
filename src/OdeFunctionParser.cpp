#include "OdeFunctionParser.h"

#include <cmath>
#include <cctype>
#include <functional>
#include <stdexcept>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

namespace {

using OdeFn = std::function<double(double, double)>;

struct Parser {
    const std::string& s;
    size_t pos = 0;

    void skipSpaces();
    char peek();
    char consume();
    double parseNumber();
    std::string parseIdent();
    OdeFn parseExpr();
    OdeFn parseTerm();
    OdeFn parseFactor();
    OdeFn parseBase();
};

void Parser::skipSpaces() {
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) {
        ++pos;
    }
}

char Parser::peek() {
    skipSpaces();
    return pos < s.size() ? s[pos] : '\0';
}

char Parser::consume() {
    skipSpaces();
    if (pos >= s.size()) {
        throw std::invalid_argument("Unexpected end of expression");
    }
    return s[pos++];
}

double Parser::parseNumber() {
    skipSpaces();
    size_t start = pos;

    while (pos < s.size() &&
           (std::isdigit(static_cast<unsigned char>(s[pos])) || s[pos] == '.')) {
        ++pos;
    }

    if (pos < s.size() && (s[pos] == 'e' || s[pos] == 'E')) {
        ++pos;
        if (pos < s.size() && (s[pos] == '+' || s[pos] == '-')) {
            ++pos;
        }
        while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) {
            ++pos;
        }
    }

    if (pos == start) {
        throw std::invalid_argument("Expected number near: " + s.substr(pos));
    }

    return std::stod(s.substr(start, pos - start));
}

std::string Parser::parseIdent() {
    skipSpaces();
    size_t start = pos;
    while (pos < s.size() &&
           (std::isalpha(static_cast<unsigned char>(s[pos])) || s[pos] == '_')) {
        ++pos;
    }
    return s.substr(start, pos - start);
}

OdeFn Parser::parseBase() {
    skipSpaces();

    if (peek() == '-') {
        consume();
        auto inner = parseBase();
        return [inner](double t, double y) { return -inner(t, y); };
    }

    if (peek() == '(') {
        consume();
        auto e = parseExpr();
        if (consume() != ')') {
            throw std::invalid_argument("Expected ')'");
        }
        return e;
    }

    if (std::isalpha(static_cast<unsigned char>(peek()))) {
        std::string id = parseIdent();

        if (id == "t" || id == "T") {
            return [](double t, double) { return t; };
        }
        if (id == "y" || id == "Y") {
            return [](double, double y) { return y; };
        }
        if (id == "pi" || id == "PI") {
            return [](double, double) { return M_PI; };
        }
        if (id == "e" || id == "E") {
            return [](double, double) { return M_E; };
        }

        if (peek() != '(') {
            throw std::invalid_argument("Expected '(' after function '" + id + "'");
        }

        consume();
        auto arg = parseExpr();
        if (consume() != ')') {
            throw std::invalid_argument("Expected ')' after argument");
        }

        if (id == "sin") return [arg](double t, double y) { return std::sin(arg(t, y)); };
        if (id == "cos") return [arg](double t, double y) { return std::cos(arg(t, y)); };
        if (id == "tan") return [arg](double t, double y) { return std::tan(arg(t, y)); };
        if (id == "sqrt") return [arg](double t, double y) { return std::sqrt(arg(t, y)); };
        if (id == "log" || id == "ln") return [arg](double t, double y) { return std::log(arg(t, y)); };
        if (id == "log10") return [arg](double t, double y) { return std::log10(arg(t, y)); };
        if (id == "exp") return [arg](double t, double y) { return std::exp(arg(t, y)); };
        if (id == "abs") return [arg](double t, double y) { return std::abs(arg(t, y)); };

        throw std::invalid_argument("Unknown function: " + id);
    }

    double value = parseNumber();
    return [value](double, double) { return value; };
}

OdeFn Parser::parseFactor() {
    auto base = parseBase();
    if (peek() == '^') {
        consume();
        auto exponent = parseFactor();
        return [base, exponent](double t, double y) {
            return std::pow(base(t, y), exponent(t, y));
        };
    }
    return base;
}

OdeFn Parser::parseTerm() {
    auto lhs = parseFactor();
    while (true) {
        skipSpaces();
        if (peek() == '*') {
            consume();
            auto rhs = parseFactor();
            lhs = [lhs, rhs](double t, double y) { return lhs(t, y) * rhs(t, y); };
        } else if (peek() == '/') {
            consume();
            auto rhs = parseFactor();
            lhs = [lhs, rhs](double t, double y) { return lhs(t, y) / rhs(t, y); };
        } else {
            break;
        }
    }
    return lhs;
}

OdeFn Parser::parseExpr() {
    auto lhs = parseTerm();
    while (true) {
        skipSpaces();
        if (peek() == '+') {
            consume();
            auto rhs = parseTerm();
            lhs = [lhs, rhs](double t, double y) { return lhs(t, y) + rhs(t, y); };
        } else if (peek() == '-') {
            consume();
            auto rhs = parseTerm();
            lhs = [lhs, rhs](double t, double y) { return lhs(t, y) - rhs(t, y); };
        } else {
            break;
        }
    }
    return lhs;
}

} // namespace

std::function<double(double, double)> OdeFunctionParser::parse(const std::string& expr) {
    Parser p{expr};
    auto fn = p.parseExpr();
    p.skipSpaces();
    if (p.pos != expr.size()) {
        throw std::invalid_argument("Unexpected character at position " +
                                    std::to_string(p.pos) + ": '" + expr.substr(p.pos) + "'");
    }
    return fn;
}

std::string OdeFunctionParser::validate(const std::string& expr) {
    if (expr.empty()) {
        return "Empty ODE expression";
    }

    try {
        auto fn = parse(expr);
        const double value = fn(0.0, 1.0);
        if (!std::isfinite(value)) {
            return "Expression produced NaN or infinity";
        }
        return "";
    } catch (const std::exception& e) {
        return e.what();
    }
}
