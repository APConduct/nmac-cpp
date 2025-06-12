#pragma once

#include "nmac/nmac.hpp"
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <variant>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>

namespace nmac::dsl {

// Forward declarations
class Expression;
class ExpressionEvaluator;

// Token types for println expressions
enum class TokenType {
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    OPERATOR,
    FUNCTION_CALL,
    OPEN_PAREN,
    CLOSE_PAREN,
    COMMA,
    DOT
};

// Token for expression parsing
struct ExprToken {
    TokenType type;
    std::string value;

    ExprToken(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};

// Value type for evaluated expressions
using Value = std::variant<
    std::nullptr_t,       // null
    bool,                 // boolean
    int,                  // integer
    double,               // floating point
    std::string,          // string
    std::vector<Value>    // array/list
    // Could add more types as needed
>;

// Type representing a captured expression
class Expression {
private:
    std::string expr_string;
    std::vector<ExprToken> tokens;

public:
    explicit Expression(std::string expr) : expr_string(std::move(expr)) {
        // Basic tokenization of the expression would happen here
        // This is a simplified example
        tokens.emplace_back(TokenType::IDENTIFIER, expr_string);
    }

    const std::string& get_string() const { return expr_string; }
    const std::vector<ExprToken>& get_tokens() const { return tokens; }
};

// Evaluation context that holds variables and functions
class EvaluationContext {
private:
    std::unordered_map<std::string, Value> variables;
    std::unordered_map<std::string, std::function<Value(std::vector<Value>)>> functions;

public:
    // Set a variable value
    void set_variable(const std::string& name, Value value) {
        variables[name] = std::move(value);
    }

    // Get a variable value
    Value get_variable(const std::string& name) const {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }

        // Try to convert the name to a literal
        try {
            // Try as int
            return std::stoi(name);
        } catch (...) {
            try {
                // Try as double
                return std::stod(name);
            } catch (...) {
                // If it's quoted, treat as string
                if (name.size() >= 2 && name.front() == '"' && name.back() == '"') {
                    return name.substr(1, name.size() - 2);
                }
            }
        }

        // Return null if not found
        return nullptr;
    }

    // Register a function
    template<typename F>
    void register_function(const std::string& name, F&& func) {
        functions[name] = std::forward<F>(func);
    }

    // Call a function
    Value call_function(const std::string& name, std::vector<Value> args) const {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return it->second(std::move(args));
        }
        throw std::runtime_error("Function not found: " + name);
    }
};

// Expression evaluator
class ExpressionEvaluator {
public:
    // Evaluate an expression in a context
    static Value evaluate(const Expression& expr, const EvaluationContext& context) {
        // This is a simplified implementation
        // In a real system, you would parse and evaluate the expression tokens

        const auto& tokens = expr.get_tokens();
        if (tokens.empty()) {
            return nullptr;
        }

        // If it's just an identifier, look it up in the context
        if (tokens.size() == 1 && tokens[0].type == TokenType::IDENTIFIER) {
            return context.get_variable(tokens[0].value);
        }

        // For more complex expressions, you would implement a proper parser and evaluator
        // This is just a placeholder
        throw std::runtime_error("Complex expression evaluation not implemented");
    }
};

// Convert a Value to a string
inline std::string value_to_string(const Value& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<Value>>) {
            std::ostringstream ss;
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << value_to_string(arg[i]);
            }
            ss << "]";
            return ss.str();
        } else {
            return "<unknown>";
        }
    }, value);
}

} // namespace nmac::dsl