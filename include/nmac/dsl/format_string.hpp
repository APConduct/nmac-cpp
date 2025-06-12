#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace nmac::dsl {

    // Simple formatter for string interpolation
    class FormatString {
    public:
        // Format a string with variadic arguments
        template<typename... Args>
        static std::string format(const std::string& fmt, Args&&... args) {
            std::string result = fmt;

            // Call helper with each argument
            format_helper(result, 0, std::forward<Args>(args)...);

            return result;
        }

        // Print formatted string to stdout
        template<typename... Args>
        static void println(const std::string& fmt, Args&&... args) {
            std::cout << format(fmt, std::forward<Args>(args)...) << std::endl;
        }

    private:
        // Base case for recursion
        static void format_helper(std::string& /*result*/, size_t /*pos*/) {
            // Nothing to do when there are no more arguments
        }

        // Replace the next placeholder with the first argument
        template<typename T, typename... Rest>
        static void format_helper(std::string& result, size_t pos, T&& arg, Rest&&... rest) {
            // Convert argument to string
            std::ostringstream ss;
            ss << arg;
            std::string arg_str = ss.str();

            // Find and replace next placeholder
            size_t placeholder_pos = result.find("{}", pos);
            if (placeholder_pos != std::string::npos) {
                result.replace(placeholder_pos, 2, arg_str);
                pos = placeholder_pos + arg_str.length();
            }

            // Process remaining arguments
            format_helper(result, pos, std::forward<Rest>(rest)...);
        }
    };

} // namespace nmac::dsl