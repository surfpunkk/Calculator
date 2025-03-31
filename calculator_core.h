#ifndef CALCULATOR_CORE_H
#define CALCULATOR_CORE_H

#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <numbers>
#include <unicode/unistr.h>
#include <unicode/uchar.h>

inline bool result_shown = false;
inline bool no_empty_state = false;

class CalculatorCore {
public:
    static int getPrecedence(const std::string &op);
    static bool isUnaryOperator(const std::string &c);
    static bool isBinaryOperator(const std::string &c);
    static bool isOperator(const std::string &c);
    static std::string applyOperator(double a, double b, const std::string& op);
    static std::vector<std::string> tokenize(const std::string& expression);
    static std::vector<std::string> infixToRPN(const std::vector<std::string>& tokens);
    static std::string evaluateRPN(const std::vector<std::string>& rpn);
    static std::string calculate(const std::string& expression);
};

#endif //CALCULATOR_CORE_H
