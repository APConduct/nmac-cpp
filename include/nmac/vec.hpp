#pragma once

#include "nmac/nmac.hpp"
#include <vector>
#include <string>
#include <type_traits>
#include <iostream>
#include <sstream>

namespace nmac::dsl {
    /// Common type for inference
    template<typename First, typename... Rest>
    struct CommonType {
        using type = std::common_type_t<First, Rest...>;
    };

    /// Specialization for empty list
    template<>
    struct CommonType<> {
        using type = int;
    };

    /// Helper to deduce common type
    template<typename... Args>
    using common_type_t = typename CommonType<Args...>::type;

    struct VecEmptyGenerator {
        /// Version for pattern matching with captures
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures&) {
            return std::vector<int>{};
        }

        /// Version for MacroGeneratorType concept
        template<typename Tuple>
        static auto expand(const Tuple&) {
            return std::vector<int>{};
        }
    };

    /// List vector generator
    struct VecListGenerator {
        /// Version for pattern matching with captures
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures& captures) {
            std::vector<int> result;
            result.reserve(captures.size());

            for (const auto& [name, token] : captures) {
                if (name == "expr") {
                    try {
                        int value = 0;
                        if constexpr (requires {std::stoi(std::string(token.content)); }) {
                            value = std::stoi(std::string(token.content));
                        } else if constexpr (requires { std::stoi(token.value); }) {
                            value = std::stoi(token.value);
                        } else if constexpr (requires { std::stoi(token); }) {
                            value = std::stoi(token);
                        }
                        // TODO: Implement proper expression evaluation
                        result.push_back(value);
                    } catch (...) {
                        result.push_back(0); // Default value on error
                    }
                }
            }
            return result;
        }

        /// Version for MacroGeneratorType concept
        template<typename Tuple>
        static auto expand(const Tuple& tuple) {
            // Unpack tuple elements into a vector
            return std::apply([]<typename... Args>(const Args&... args) {
                std::vector<common_type_t<std::decay_t<Args>...>> result;
                result.reserve(sizeof...(args));
                (result.push_back(args), ...);
                return result;
            }, tuple);
        }
    };

    /// Repeat vector generator
    struct VecRepeatGenerator {
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures& captures) {
            int value = 0;
            int count = 0;

            for (const auto& [name, token] : captures) {
                try {
                    if (name == "expr") {
                        // Extract the value from the expression
                        if constexpr (requires { std::stoi(std::string(token.content)); }) {
                            value = std::stoi(std::string(token.content));
                        } else if constexpr (requires { std::stoi(token.value); }) {
                            value = std::stoi(token.value);
                        } else if constexpr (requires { std::stoi(token); }) {
                            value = std::stoi(token);
                        }
                    } else if (name == "count") {
                        // Extract count
                        if constexpr (requires { std::stoi(std::string(token.content)); }) {
                            count = std::stoi(std::string(token.content));
                        } else if constexpr (requires { std::stoi(token.value); }) {
                            count = std::stoi(token.value);
                        } else if constexpr (requires { std::stoi(token); }) {
                            count = std::stoi(token);
                        }
                    }
                } catch (...) {
                    // Ignore conversion errors
                }
            }
            return std::vector<int>(count, value);
        }

        // Version for MacroGeneratorType concept
        template<typename Tuple>
        static auto expand(const Tuple& tuple) {
            // Assume tuple has exactly two elements: value and count
            return std::apply([]<typename Value>(Value value, auto count) {
                return std::vector<std::decay_t<Value>>(count, value);
            }, tuple);
        }
    };

    // Macro rules
    using VecEmptyRule = nmac::MacroRule<"vec! [ ]", VecEmptyGenerator>;
    using VecListRule = nmac::MacroRule<"vec! [ $expr+ ]", VecListGenerator>;
    using VecRepeatRule = nmac::MacroRule<"vec! [ $expr ; $count ]", VecRepeatGenerator>;

    // Macro expander combining all rules
    using VecExpander = nmac::MacroExpander<VecEmptyRule, VecListRule, VecRepeatRule>;

    // =====================================================
    // User-friendly function API
    // =====================================================

    // Empty vector creator
    template<typename T = int>
    std::vector<T> vec() {
        return std::vector<T>{};
    }

    // Vector from list of elements
    template<typename... Args>
    auto vec(Args&&... args) {
        using element_type = common_type_t<std::decay_t<Args>...>;

        std::vector<element_type> result;
        result.reserve(sizeof...(args));
        (result.push_back(static_cast<element_type>(std::forward<Args>(args))), ...);

        return result;
    }

    // Repeat vector creator
    template<typename T>
    std::vector<T> vec_repeat(T value, size_t count) {
        return std::vector<T>(count, value);
    }

    // =====================================================
    // String literal operator implementation
    // =====================================================

    // Vector literal with compile-time format string
    template<nmac::ct_string Format>
    class VecLiteral {
        static constexpr auto format_view = Format.view();

    public:
        // Empty vector
        static auto eval() {
            if constexpr (format_view == "[]") {
                return vec<int>();
            } else {
                static_assert(format_view == "[]", "Invalid vec! format for an empty vector");
                return vec<int>(); // Default to empty vector if format is not recognized
            }
        }

        // Vector with elements or repeated element
        template<typename... Args>
        auto operator()(Args&&... args) const {
            if constexpr (sizeof...(Args) > 0) {
                return vec<int>();
            } else if constexpr (sizeof...(Args) == 2 && format_view.find(':') != std::string_view::npos) {
                // Repeat pattern
                return vec_repeat(std::forward<Args>(args)...);
            } else {
                // List pattern
                return vec(std::forward<Args>(args)...);
            }
        }
    };
}

// =====================================================
// User-facing macro/operator interfaces
// =====================================================

#define vec(...) \
    nmac::dsl::vec(__VA_ARGS__)

#define vec_repeat(value, count) \
    nmac::dsl::vec_repeat(value, count)

// String literal operator
template<nmac::ct_string Format>
constexpr auto operator""_vec() {
    return nmac::dsl::VecLiteral<Format>{};
}