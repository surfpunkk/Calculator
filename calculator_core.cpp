#include "calculator_core.h"
#include <stack>
#include <cmath>
#include <vector>

int CalculatorCore::getPrecedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^':           return 3;
        default:            return 0;
    }
}

bool CalculatorCore::isOperator(wchar_t c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%' || c == L'√';
}

std::string CalculatorCore::applyOperator(double a, double b, const std::string& op) {
    if (op == "+") return std::to_string(a + b);
    if (op == "-") return std::to_string(a - b);
    if (op == "*") return std::to_string(a * b);
    if (op == "/") {
        if (b == 0) {
            no_empty_state = true;
            return "Error: Division by 0";
        }
        return std::to_string(a / b);
    }
    if (op == "^") return std::to_string(std::pow(a, b));
    if (op == "%") {
        return std::to_string(a * (b / 100.0));
    }
    if (op == "√") {
        if (b < 0) {
            return "Error: Negative root";
        }
        return std::to_string(std::sqrt(b));
    }
    return "Error: No empty";
}

std::vector<std::string> CalculatorCore::tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string currentToken;
    bool expectOperator = false;
    bool inExponent = false;

    for (const char i : expression) {
        wchar_t c = static_cast<unsigned char>(i);

        if (std::isspace(c)) {
            continue;
        }

        if (std::isdigit(c) || (c == ',' && !currentToken.empty() && currentToken.find(',') == std::string::npos)) {
            currentToken += c;
            expectOperator = true;
        } else if (c == '-') {
            if (!expectOperator || (tokens.empty() && currentToken.empty())) {
                currentToken += c;
            } else {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.emplace_back(1, c);
                expectOperator = false;
            }
        } else if ((c == 'e' || c == 'E') && !currentToken.empty() && !inExponent && std::isdigit(currentToken.back())) {
            currentToken += c;
            inExponent = true;
        } else if ((c == '+' || c == '-') && inExponent && (currentToken.back() == 'e' || currentToken.back() == 'E')) {
            currentToken += c;
            inExponent = false;
        } else if (isOperator(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.emplace_back(1, c);
            expectOperator = (c != L'√');
            inExponent = false;
        } else if (c == '(' || c == ')') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.emplace_back(1, c);
            expectOperator = (c == ')');
        } else {
            no_empty_state = true;
            return {"Error: Incorrect"};
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

std::vector<std::string> CalculatorCore::infixToRPN(const std::vector<std::string>& tokens) {
    std::stack<std::string> operators;
    std::vector<std::string> output;

    for (const auto& token : tokens) {
        if (std::isdigit(token[0]) || (token[0] == '-' && token.size() > 1) || token.find(',') != std::string::npos) {
            output.push_back(token);
        } else if (isOperator(token[0])) {
            while (!operators.empty() && getPrecedence(operators.top()[0]) >= getPrecedence(token[0])) {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        } else if (token == "(") {
            operators.push(token);
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                output.push_back(operators.top());
                operators.pop();
            }
            if (operators.empty() || operators.top() != "(") {
                no_empty_state = true;
                return {"Error: Mismatched parentheses"};
            }
            operators.pop();
        } else {
            no_empty_state = true;
            return {"Error: Unknown element"};
        }
    }
    while (!operators.empty()) {
        if (operators.top() == "(") {
            no_empty_state = true;
            return {"Error: Mismatched parentheses"};
        }
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

std::string CalculatorCore::evaluateRPN(const std::vector<std::string>& rpn) {
    std::stack<double> values;
    for (const auto& token : rpn) {
        if (isOperator(token[0]) && token.size() == 1) {
            if (token[0] == L'√') {
                if (values.empty()) {
                    no_empty_state = true;
                    return "Error: Missing operand for √";
                }
                double operand = values.top(); values.pop();
                if (operand < 0) {
                    no_empty_state = true;
                    return "Error: Negative root";
                }
            } else {
                if (values.size() < 2) {
                    no_empty_state = true;
                    return "Error: Missing operands";
                }
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();

                std::string result = applyOperator(a, b, token);
                if (result.find("Error") != std::string::npos) {
                    return result;
                }

                values.push(std::stod(result));
            }
        } else {
            try {
                values.push(std::stod(token));
            } catch (...) {
                no_empty_state = true;
                return "Error: Invalid number format";
            }
        }
    }
    if (values.size() != 1) {
        no_empty_state = true;
        return "Error: Incorrect expression";
    }

    return std::to_string(values.top());
}

std::string CalculatorCore::calculate(const std::string& expression) {
    no_empty_state = false;
    try {
        auto tokens = tokenize(expression);
        if (tokens.empty() || tokens[0] == "Error: Incorrect") {
            return "Error: Invalid input";
        }
        auto rpn = infixToRPN(tokens);
        if (rpn.empty() || rpn[0] == "Error: Incorrect") {
            return "Error: Incorrect";
        }
        return evaluateRPN(rpn);
    } catch (...) {
        no_empty_state = true;
        return "Error: Incorrect";
    }
}