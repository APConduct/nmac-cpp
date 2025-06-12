#pragma once

#include "nmac/nmac.hpp"
#include "nmac/dsl/fmt_core.hpp"
#include <iostream>

namespace nmac::dsl::literals {

    // Format string literal
    template<nmac::ct_string Format>
    class FormatLiteral {
        static constexpr auto format_view = Format.view();

    public:
        // Call operator with variadic arguments
        template<typename... Args>
        auto operator()(Args&&... args) const {
            static constexpr size_t placeholder_count = fmt::FormatCore::count_placeholders(format_view);
            static constexpr size_t arg_count = sizeof...(Args);

            // Compile-time validation
            if constexpr (placeholder_count != arg_count) {
                static_assert(placeholder_count == arg_count,
                             "Number of format placeholders must match number of arguments");
            }

            // Return a callable that does the actual formatting
            return [... args = std::forward<Args>(args)]() {
                try {
                    std::string result = fmt::FormatCore::format(format_view, args...);
                    std::cout << result << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Format error: " << e.what() << std::endl;
                }
            };
        }

        // Overload for formatting without printing
        template<typename... Args>
        std::string format(Args&&... args) const {
            static constexpr size_t placeholder_count = fmt::FormatCore::count_placeholders(format_view);
            static constexpr size_t arg_count = sizeof...(Args);

            // Compile-time validation
            if constexpr (placeholder_count != arg_count) {
                static_assert(placeholder_count == arg_count,
                             "Number of format placeholders must match number of arguments");
            }

            return fmt::FormatCore::format(format_view, std::forward<Args>(args)...);
        }
    };

}

// User-defined literal operator
template<nmac::ct_string Format>
constexpr auto operator""_println() {
    return nmac::dsl::literals::FormatLiteral<Format>{};
}

// User-defined literal operator for formatting
template<nmac::ct_string Format>
constexpr auto operator""_format() {
    return nmac::dsl::literals::FormatLiteral<Format>{};
}