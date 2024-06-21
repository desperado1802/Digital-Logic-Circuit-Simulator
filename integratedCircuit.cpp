#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <functional>
#include <stack>
#include <bitset>
#include <algorithm>

class LogicCircuit {
public:
    std::string name;
    std::vector<std::string> inputs;
    std::string expression;

    LogicCircuit() = default;

    LogicCircuit(std::string n, std::vector<std::string> in, std::string expr)
        : name(n), inputs(in), expression(expr) {}

    bool evaluate(const std::vector<bool>& inputValues) const {
        std::map<std::string, bool> inputMap;
        for (size_t i = 0; i < inputs.size(); ++i) {
            inputMap[inputs[i]] = inputValues[i];
        }

        return evaluateExpression(expression, inputMap);
    }

private:
    std::vector<std::string> tokenize(const std::string& expr) const {
        std::vector<std::string> tokens;
        std::string token;
        for (char ch : expr) {
            if (isspace(ch)) {
                continue;
            } else if (ch == '&' || ch == '|' || ch == '!' || ch == '(' || ch == ')') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                tokens.push_back(std::string(1, ch));
            } else {
                token += ch;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool applyOp(const std::string& op, bool left, bool right) const {
        if (op == "&") return left && right;
        if (op == "|") return left || right;
        return false;
    }

    bool evaluateExpression(const std::string& expr, const std::map<std::string, bool>& inputMap) const {
        std::vector<std::string> tokens = tokenize(expr);
        std::stack<bool> values;
        std::stack<std::string> ops;

        // std::cout << "Evaluating expression: " << expr << "\n";

        for (const std::string& token : tokens) {
        
            if (inputMap.find(token) != inputMap.end()) {
                values.push(inputMap.at(token));
            } else if (token == "(") {
                ops.push(token);
            } else if (token == ")") {
                while (!ops.empty() && ops.top() != "(") {
                    std::string op = ops.top();
                    ops.pop();

                    bool right = values.top();
                    values.pop();

                    bool left = values.top();
                    values.pop();

                    values.push(applyOp(op, left, right));
                }
                ops.pop();
            } else if (token == "&" || token == "|" || token == "!") {
                while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                    std::string op = ops.top();
                    ops.pop();

                    bool right = values.top();
                    values.pop();

                    if (op == "!") {
                        values.push(!right);
                    } else {
                        bool left = values.top();
                        values.pop();
                        values.push(applyOp(op, left, right));
                    }
                }
                ops.push(token);
            } else {
                std::cerr << "Error: Undefined variable or operator in expression: " << token << "\n";
                return false;
            }
        }

        while (!ops.empty()) {
            std::string op = ops.top();
            ops.pop();

            bool right = values.top();
            values.pop();

            if (op == "!") {
                values.push(!right);
            } else {
                bool left = values.top();
                values.pop();
                values.push(applyOp(op, left, right));
            }
        }

        if (values.size() != 1) {
            std::cerr << "Error: Invalid expression, stack size is not 1 after evaluation\n";
            return false;
        }

        return values.top();
    }

    int precedence(const std::string& op) const {
        if (op == "!") return 3;
        if (op == "&") return 2;
        if (op == "|") return 1;
        return 0;
    }
};

std::map<std::string, LogicCircuit> circuits;

bool defineCircuit(const std::string& definition) {

    size_t definePos = definition.find("DEFINE");
    size_t colonPos = definition.find(": \"");
    size_t nameEndPos = definition.find('(');
    size_t inputsEndPos = definition.find(')', nameEndPos);
    size_t exprStartPos = colonPos + 3; // Skip past ": \""
    size_t exprEndPos = definition.rfind('"');

    if (definePos == std::string::npos || colonPos == std::string::npos ||
        nameEndPos == std::string::npos || inputsEndPos == std::string::npos ||
        exprStartPos == std::string::npos || exprEndPos == std::string::npos) {
        std::cerr << "Error: Invalid DEFINE command format.\n";
        return false;
    }

    std::string name = definition.substr(definePos + 7, nameEndPos - (definePos + 7));
    std::string inputs = definition.substr(nameEndPos + 1, inputsEndPos - nameEndPos - 1);
    std::string expression = definition.substr(exprStartPos, exprEndPos - exprStartPos);

    std::vector<std::string> inputNames;
    std::istringstream inputStream(inputs);
    std::string input;
    while (std::getline(inputStream, input, ',')) {
        input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end()); 
        inputNames.push_back(input);
    }

    for (const auto& in : inputNames) {
        if (expression.find(in) == std::string::npos) {
            std::cerr << "Error: input " << in << " not defined in expression.\n";
            return false;
        }
    }

    circuits[name] = LogicCircuit(name, inputNames, expression);
    std::cout << "Defined circuit: " << name << " with inputs: " << inputs << " and expression: " << expression << "\n";
    std::cout << "Current circuits:\n";
    for (std::map<std::string, LogicCircuit>::const_iterator it = circuits.begin(); it != circuits.end(); ++it) {
        std::cout << "Name: " << it->first << ", Inputs: ";
        for (const auto& in : it->second.inputs) {
            std::cout << in << " ";
        }
        std::cout << ", Expression: " << it->second.expression << "\n";
    }
    return true;
}

bool runCircuit(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, name;
    iss >> cmd >> name;

    if (cmd != "RUN") return false;

    name = name.substr(0, name.find('('));
    std::string values = command.substr(command.find('(') + 1);
    values = values.substr(0, values.find(')'));

    std::vector<bool> inputValues;
    std::istringstream valueStream(values);
    std::string value;
    while (std::getline(valueStream, value, ',')) {
        value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
        inputValues.push_back(value == "1");
    }

    // std::cout << "Parsed input values: ";
    // for (const bool& val : inputValues) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    auto it = circuits.find(name);
    if (it == circuits.end()) {
        std::cerr << "Error: circuit " << name << " not defined.\n";
        return false;
    }

    if (inputValues.size() != it->second.inputs.size()) {
        std::cerr << "Error: Number of input values does not match number of inputs for circuit " << name << "\n";
        std::cerr << "Expected: " << it->second.inputs.size() << ", Provided: " << inputValues.size() << "\n";
        return false;
    }

    std::cout << "Running circuit: " << name << "\n";
    std::cout << "Input values: ";
    for (const auto& val : inputValues) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    bool result = it->second.evaluate(inputValues);
    std::cout << "Result: " << result << "\n";
    return true;
}


bool allCircuit(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, name;
    iss >> cmd >> name;

    if (cmd != "ALL") return false;

    if (name.empty()) {
        std::cerr << "Error: No circuit name provided.\n";
        return false;
    }

    auto it = circuits.find(name);
    if (it == circuits.end()) {
        std::cerr << "Error: circuit " << name << " not defined.\n";
        return false;
    }

    const auto& circuit = it->second;
    size_t numInputs = circuit.inputs.size();
    size_t numCombinations = 1 << numInputs;

    std::cout << "Truth table for " << name << ":\n";
    for (const auto& input : circuit.inputs) {
        std::cout << input << " ";
    }
    std::cout << "res\n";

    for (size_t i = 0; i < numCombinations; ++i) {
        std::vector<bool> inputValues(numInputs);
        for (size_t j = 0; j < numInputs; ++j) {
            inputValues[j] = (i & (1 << j)) != 0;
        }

       
        for (const auto& val : inputValues) {
            std::cout << val << " ";
        }

        bool result = circuit.evaluate(inputValues);
        std::cout << result << "\n";
    }
    return true;
}

int main() {
    std::cout << "Welcome to the Digital Logic Circuit Simulator!\n";
    std::cout << "Available commands:\n";
    std::cout << "1. DEFINE <circuit_name>(<input1>, <input2>, ...): \"<expression>\"\n";
    std::cout << "   Example: DEFINE ic1(a, b): \"a & b\"\n";
    std::cout << "2. RUN <circuit_name>(<value1>, <value2>, ...)\n";
    std::cout << "   Example: RUN ic1(1, 0)\n";
    std::cout << "3. ALL <circuit_name>\n";
    std::cout << "   Example: ALL ic1\n";
    std::cout << "Enter your commands:\n";
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.find("DEFINE") == 0) {
            defineCircuit(line);
        } else if (line.find("RUN") == 0) {
            runCircuit(line);
        } else if (line.find("ALL") == 0) {
            allCircuit(line);
        } else {
            std::cerr << "Error: unknown command.\n";
        }
    }
    return 0;
}
