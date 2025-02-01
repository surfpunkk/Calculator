#ifndef CALCULATOR_CORE_H
#define CALCULATOR_CORE_H

#include <string>
#include <stdexcept>
#include <vector>

class CalculatorCore {
public:
    static int getPrecedence(char op);
    static bool isOperator(wchar_t c);
    static std::string applyOperator(double a, double b, const std::string& op);
    static std::vector<std::string> tokenize(const std::string& expression);
    static std::vector<std::string> infixToRPN(const std::vector<std::string>& tokens);
    static std::string evaluateRPN(const std::vector<std::string>& rpn);
    static std::string calculate(const std::string& expression);
};

#endif //CALCULATOR_CORE_H
