#pragma once
#include "nmac/nmac.hpp"
#include "nmac/dsl/println_eval.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace nmac::dsl {

// Token type for println macro
struct PrintlnToken {
    enum class Type {
        IDENTIFIER,
        STRING_LITERAL,
        EXPRESSION,
        LPAREN,
        RPAREN,
        COMMA
    };

    Type type;
    std::string value;

    PrintlnToken(Type t, std::string v) : type(t), value(std::move(v)) {}
};

// The improved println generator with argument evaluation
struct PrintlnGenerator {
    template<typename Input, typename Captures>
    static auto expand(const Input& input, const Captures& captures) {
        std::string format;
        std::vector<Expression> expressions;

        // Extract format string and expressions
        for (const auto& [name, token] : captures) {
            if (name == "fmt") {
                // Clean up the format string
                std::string token_str = token.value;
                if (token_str.size() >= 2 && token_str.front() == '"' && token_str.back() == '"') {
                    format = token_str.substr(1, token_str.size() - 2);
                } else {
                    format = token_str;
                }
            } else if (name == "arg") {
                // Create Expression objects for each argument
                expressions.emplace_back(token.value);
            }
        }

        // Return a function that evaluates expressions and formats the output
        return [format, expressions]() {
            // Create an evaluation context
            EvaluationContext context;

            // In a real implementation, you would populate the context
            // with variables and functions from the surrounding scope

            // Evaluate each expression
            std::vector<std::string> evaluated_args;
            evaluated_args.reserve(expressions.size());

            for (const auto& expr : expressions) {
                try {
                    Value value = ExpressionEvaluator::evaluate(expr, context);
                    evaluated_args.push_back(value_to_string(value));
                } catch (const std::exception& e) {
                    std::cerr << "Error evaluating expression '" << expr.get_string()
                              << "': " << e.what() << std::endl;
                    evaluated_args.emplace_back("<error>");
                }
            }

            // Format the string
            std::string result = format;
            size_t pos = 0;

            for (const auto& arg : evaluated_args) {
                size_t placeholder_pos = result.find("{}", pos);
                if (placeholder_pos == std::string::npos) break;

                result.replace(placeholder_pos, 2, arg);
                pos = placeholder_pos + arg.length();
            }

            // Check if any placeholders remain
            if (result.find("{}", pos) != std::string::npos) {
                std::cerr << "Warning: Not enough arguments for format string" << std::endl;
            }

            // Print the result
            std::cout << result << std::endl;
        };
    }
};

// Define the println macro rule
using PrintlnRule = nmac::MacroRule<"println! ( $fmt $(, $arg)* )", PrintlnGenerator>;
using PrintlnMacro = nmac::MacroExpander<PrintlnRule>;

// Helper function to tokenize a println call
std::vector<PrintlnToken> tokenize_println(const std::string& format, const auto&... args) {
    // Create tokens for a println! macro invocation
    std::vector<PrintlnToken> tokens;
    tokens.emplace_back(PrintlnToken::Type::IDENTIFIER, "println!");
    tokens.emplace_back(PrintlnToken::Type::LPAREN, "(");
    tokens.emplace_back(PrintlnToken::Type::STRING_LITERAL, "\"" + format + "\"");

    // Add each argument
    ([&]() {
        std::ostringstream ss;
        ss << args;
        tokens.emplace_back(PrintlnToken::Type::COMMA, ",");
        tokens.emplace_back(PrintlnToken::Type::EXPRESSION, ss.str());
    }(), ...);

    tokens.emplace_back(PrintlnToken::Type::RPAREN, ")");
    return tokens;
}

} // namespace nmac::dsl