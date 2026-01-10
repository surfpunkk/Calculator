#include "calculator_core.h"

int CalculatorCore::getPrecedence(const std::string &op) {
    if (op == "+" || op == "-") return 1;
    if (op == "×" || op == "*" || op == "÷" || op == "/" || op == ":" || op == "%") return 2;
    if (op == "^" || op == "√" || op == "!") return 3;
    return 0;
}

bool CalculatorCore::isUnaryOperator(const std::string &c) {
    return c == "√" || c == "!";
}

bool CalculatorCore::isBinaryOperator(const std::string &c) {
    return c == "+" || c == "-" || c == "×" || c == "*" || c == "÷" || c == "/" || c == ":"
    || c == "^" || c == "%";
}

bool CalculatorCore::isOperator(const std::string &c) {
    return isUnaryOperator(c) || isBinaryOperator(c);
}

std::string CalculatorCore::applyOperator(const double a, const double b, const std::string& op) {
    if (op == "+") return std::to_string(a + b);
    if (op == "-") return std::to_string(a - b);
    if (op == "×" || op == "*") return std::to_string(a * b);
    if (op == "÷" || op == "/" || op == ":") {
        if (b == 0) {
            no_empty_state = true;
            return "Error: Division by 0";
        }
        return std::to_string(a / b);
    }
    if (op == "^") return std::to_string(std::pow(a, b));
    if (op == "%") {
        if (a < 0) {
            no_empty_state = true;
            return "Error: Procent couldn't be negative";
        }
        return std::to_string(a * (b / 100.0));
    }
    return "Error: No result";
}

std::vector<std::string> CalculatorCore::tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    const icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(expression);
    std::string currentToken;
    bool expectOperator = false;

    for (int32_t i = 0; i < ustr.length(); ++i) {
        const UChar32 c = ustr.char32At(i);
        std::string charStr;
        icu::UnicodeString(c).toUTF8String(charStr);

        if (std::isspace(c)) {
            continue;
        }

        if (u_isalpha(c)) {
            currentToken += charStr;
            expectOperator = true;
            continue;
        }

        if (std::isdigit(c) || (charStr == "," && !currentToken.empty() && currentToken.find(',') == std::string::npos)) {
            currentToken += charStr;
            expectOperator = true;
        } else if (charStr == "-") {
            if (!expectOperator || (tokens.empty() && currentToken.empty())) currentToken += charStr;
            else {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.emplace_back(charStr);
                expectOperator = false;
            }
        } else if ((charStr == "e" || charStr == "E") && !currentToken.empty() && std::isdigit(currentToken.back())) currentToken += charStr;
        else if ((charStr == "+" || charStr == "-") && !currentToken.empty() && (currentToken.back() == 'e' || currentToken.back() == 'E')) currentToken += charStr;
        else if (std::isdigit(c) && !currentToken.empty() &&
        (currentToken.back() == '+' || currentToken.back() == '-' ||
        currentToken.back() == 'e' || currentToken.back() == 'E')) {
                currentToken += charStr;
                expectOperator = true;
        } else if (charStr == "√" && !expectOperator) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.emplace_back(charStr);
        } else if (charStr == "!") {
                if (!expectOperator) {
                    no_empty_state = true;
                    return {"Error: Factorial must follow a number"};
                }
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.emplace_back(charStr);
                expectOperator = true;
        } else if (isBinaryOperator(charStr)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.emplace_back(charStr);
                expectOperator = false;
        } else if (charStr == "(" || charStr == ")") {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.emplace_back(charStr);
            expectOperator = charStr == ")";
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
        } else if (isUnaryOperator(token)) operators.push(token);
        else if (isBinaryOperator(token)) {
            while (!operators.empty() && operators.top() != "(" && getPrecedence(operators.top()) >= getPrecedence(token)) {
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
                if (!operators.empty() && isUnaryOperator(operators.top())) {
                    output.push_back(operators.top());
                    operators.pop();
                }
            } else if (token == "π") {
                output.push_back(std::to_string(std::numbers::pi));
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
if (isOperator(token) && !token.empty()) {
    if (isUnaryOperator(token)) {
        if (values.empty()) {
            no_empty_state = true;
            return "Error: Missing operand for " + token;
        }
        const double operand = values.top(); values.pop();

        if (token == "√") {
            if (operand < 0) {
                no_empty_state = true;
                return "Error: Negative root";
            }
            values.push(std::sqrt(operand));
        } else if (token == "!") {
            if (operand < 0) {
                no_empty_state = true;
                return "Error: Factorial is not defined for negative numbers";
            }
            if (operand != std::floor(operand)) {
                no_empty_state = true;
                return "Error: Factorial requires integer";
            }
            double result = 1;
            for (int i = 1; i <= static_cast<int>(operand); ++i) {
                result *= i;
            }
            values.push(result);
        }
    } else {
        if (values.size() < 2) {
            no_empty_state = true;
            return "Error: Missing operands for " + token;
        }
        const double b = values.top(); values.pop();
        const double a = values.top(); values.pop();

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
        const auto tokens = tokenize(expression);
        if (tokens.empty() || tokens[0] == "Error: Incorrect") {
            return "Error: Invalid input";
        }
        const auto rpn = infixToRPN(tokens);
        if (rpn.empty() || rpn[0] == "Error: Incorrect") {
            return "Error: Incorrect";
        }
        return evaluateRPN(rpn);
    } catch (...) {
        no_empty_state = true;
        return "Error: Incorrect";
    }
}