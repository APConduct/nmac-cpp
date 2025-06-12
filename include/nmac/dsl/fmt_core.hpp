#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <utility>
#include <stdexcept>

namespace nmac::dsl::fmt {
    class FormatCore {
    public:
        template<typename... Args>
        static std::string format(std::string_view fmt, Args&&... args) {
            std::string result(fmt);
            size_t pos = 0;

            ([&](const auto& arg) {
                std::ostringstream ss;
                ss << arg;
                std::string arg_str = ss.str();

                size_t placeholder_pos = result.find("{}", pos);
                if (placeholder_pos != std::string::npos) {
                    result.replace(placeholder_pos, 2, arg_str);
                    pos = placeholder_pos + arg_str.length();
                } else {
                    throw std::invalid_argument("Too many arguments for format string");
                }
            }(std::forward<Args>(args)), ...);

            if (result.find("{}", pos) != std::string::npos) {
                throw std::invalid_argument("Not enough arguments for format string");
            }
            return result;
        }

        // Compile-time validation of format string (requires C++20)
        template<std::string_view fmt>
        static constexpr bool validate_format() {
            size_t open_count = 0;
            bool in_placeholder = false;

            for (size_t i = 0; i < fmt.size(); ++i) {
                if (in_placeholder) {
                    if (fmt[i] == '}') {
                        in_placeholder = false;
                    } else if (fmt[i] != '{') {
                        // Invalid character in placeholder
                        return false;
                    }
                } else if (fmt[i] == '{') {
                    if (i + 1 < fmt.size() && fmt[i+1] == '{') {
                        // Escaped '{{' - skip the next character
                        ++i;
                    } else {
                        in_placeholder = true;
                        ++open_count;
                    }
                } else if (fmt[i] == '}') {
                    if (i + 1 < fmt.size() && fmt[i+1] == '}') {
                        // Escaped '}}' - skip the next character
                        ++i;
                    } else {
                        // Unmatched closing brace
                        return false;
                    }
                }
            }

            // Make sure all placeholders are closed
            return !in_placeholder;
        }

        // Count placeholders in a format string
        static constexpr size_t count_placeholders(std::string_view fmt) {
            size_t count = 0;
            size_t pos = 0;

            while ((pos = fmt.find("{}", pos)) != std::string_view::npos) {
                ++count;
                pos += 2;
            }

            return count;
        }
    };
}