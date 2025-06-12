#pragma once

#include "nmac/nmac.hpp"
#include "nmac/dsl/format_string.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace nmac::dsl {

    // Simple token for println macro
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

    // The generator for println macro
    struct PrintlnGenerator {
        template<typename Input>
        static auto expand(const Input& input) {
            // Extract the format string and arguments
            std::string format;
            std::vector<std::string> args;

            // In a real implementation, you'd parse the input tokens
            // Here, we'll just assume a simplified token stream

            // Return a function that does the printing
            return [=]() {
                // For a simple version, just print a placeholder message
                std::cout << "println! macro would format: " << format
                          << " with " << args.size() << " arguments" << std::endl;
            };
        }
    };

    // Simple function to directly format and print
    template<typename... Args>
    void println(const std::string& format, Args&&... args) {
        FormatString::println(format, std::forward<Args>(args)...);
    }

    // Simple function to format without printing
    template<typename... Args>
    std::string format(const std::string& format, Args&&... args) {
        return FormatString::format(format, std::forward<Args>(args)...);
    }

} // namespace nmac::dsl

// Simple macro for convenience
#define PRINTLN(fmt, ...) \
nmac::dsl::println(fmt, ##__VA_ARGS__)