#include "FunctionParser.h"

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

// ── Recursive-descent parser ──────────────────────────────────────────────────
// Grammar:
//   expr   := term  (('+' | '-') term)*
//   term   := factor (('*' | '/') factor)*
//   factor := base ('^' factor)?           right-associative
//   base   := '-' base | '(' expr ')' | number | func '(' expr ')' | x | pi | e
//   func   := sin | cos | tan | sqrt | log | ln | log10 | exp | abs

namespace {

struct Parser {
    const std::string& s;
    size_t pos = 0;

    void skipSpaces();
    char peek();
    char consume();

    double parseNumber();
    std::string parseIdent();

    std::function<double(double)> parseExpr();
    std::function<double(double)> parseTerm();
    std::function<double(double)> parseFactor();
    std::function<double(double)> parseBase();
};

void Parser::skipSpaces() {
    while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) {
        ++pos;
    }
}

char Parser::peek() {
    skipSpaces();
    return (pos < s.size()) ? s[pos] : '\0';
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

std::function<double(double)> Parser::parseBase() {
    skipSpaces();

    // Unary minus
    if (peek() == '-') {
        consume();
        auto inner = parseBase();
        return [inner](double x) {
            return -inner(x);
        };
    }

    // Parenthesized expression
    if (peek() == '(') {
        consume();

        auto e = parseExpr();

        if (consume() != ')') {
            throw std::invalid_argument("Expected ')'");
        }

        return e;
    }

    // Identifier: x, constants, or function call
    if (std::isalpha(static_cast<unsigned char>(peek()))) {
        std::string id = parseIdent();

        if (id == "x" || id == "X") {
            return [](double x) {
                return x;
            };
        }

        if (id == "pi" || id == "PI") {
            return [](double) {
                return M_PI;
            };
        }

        if (id == "e" || id == "E") {
            return [](double) {
                return M_E;
            };
        }

        if (peek() != '(') {
            throw std::invalid_argument("Expected '(' after function '" + id + "'");
        }

        consume();

        auto arg = parseExpr();

        if (consume() != ')') {
            throw std::invalid_argument("Expected ')' after argument");
        }

        if (id == "sin") {
            return [arg](double x) {
                return std::sin(arg(x));
            };
        }

        if (id == "cos") {
            return [arg](double x) {
                return std::cos(arg(x));
            };
        }

        if (id == "tan") {
            return [arg](double x) {
                return std::tan(arg(x));
            };
        }

        if (id == "sqrt") {
            return [arg](double x) {
                return std::sqrt(arg(x));
            };
        }

        if (id == "log" || id == "ln") {
            return [arg](double x) {
                return std::log(arg(x));
            };
        }

        if (id == "log10") {
            return [arg](double x) {
                return std::log10(arg(x));
            };
        }

        if (id == "exp") {
            return [arg](double x) {
                return std::exp(arg(x));
            };
        }

        if (id == "abs") {
            return [arg](double x) {
                return std::abs(arg(x));
            };
        }

        throw std::invalid_argument("Unknown function: " + id);
    }

    // Number
    double value = parseNumber();

    return [value](double) {
        return value;
    };
}

std::function<double(double)> Parser::parseFactor() {
    auto base = parseBase();

    if (peek() == '^') {
        consume();

        auto exponent = parseFactor();

        return [base, exponent](double x) {
            return std::pow(base(x), exponent(x));
        };
    }

    return base;
}

std::function<double(double)> Parser::parseTerm() {
    auto lhs = parseFactor();

    while (true) {
        skipSpaces();

        if (peek() == '*') {
            consume();

            auto rhs = parseFactor();

            lhs = [lhs, rhs](double x) {
                return lhs(x) * rhs(x);
            };
        } else if (peek() == '/') {
            consume();

            auto rhs = parseFactor();

            lhs = [lhs, rhs](double x) {
                return lhs(x) / rhs(x);
            };
        } else {
            break;
        }
    }

    return lhs;
}

std::function<double(double)> Parser::parseExpr() {
    auto lhs = parseTerm();

    while (true) {
        skipSpaces();

        if (peek() == '+') {
            consume();

            auto rhs = parseTerm();

            lhs = [lhs, rhs](double x) {
                return lhs(x) + rhs(x);
            };
        } else if (peek() == '-') {
            consume();

            auto rhs = parseTerm();

            lhs = [lhs, rhs](double x) {
                return lhs(x) - rhs(x);
            };
        } else {
            break;
        }
    }

    return lhs;
}

} // anonymous namespace

std::function<double(double)> FunctionParser::parse(const std::string& expr) {
    Parser p{expr};

    auto fn = p.parseExpr();

    p.skipSpaces();

    if (p.pos != expr.size()) {
        throw std::invalid_argument(
            "Unexpected character at position " +
            std::to_string(p.pos) +
            ": '" +
            expr.substr(p.pos) +
            "'"
            );
    }

    return fn;
}

double FunctionParser::derivative(
    const std::function<double(double)>& f,
    double x,
    double h
    ) {
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

std::string FunctionParser::validate(const std::string& expr) {
    try {
        auto fn = parse(expr);
        fn(1.0);
        return "";
    } catch (const std::exception& e) {
        return e.what();
    }
}