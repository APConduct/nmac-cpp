#pragma once

#include "nmac/nmac.hpp"
#include "nmac/dsl/fmt_core.hpp"
#include <iostream>

namespace nmac::dsl::templates {
    // Template-based formatter with compile-time format string
    template<nmac::ct_string Format, typename... Args>
    class FormatTemplate {
        static constexpr auto format_view = Format.view();
        static constexpr size_t arg_count = sizeof...(Args);
        static constexpr size_t placeholder_count = fmt::FormatCore::count_placeholders(format_view);

    public:
        // Validate at compile time that we have the right number of arguments
        static_assert(placeholder_count == arg_count,
                     "Number of format placeholders must match number of arguments");

        // Print the formatted string
        static void println(Args&&... args) {
            try {
                std::string result = fmt::FormatCore::format(format_view, std::forward<Args>(args)...);
                std::cout << result << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Format error: " << e.what() << std::endl;
            }
        }

        // Format without printing
        static std::string format(Args&&... args) {
            return fmt::FormatCore::format(format_view, std::forward<Args>(args)...);
        }
    };

    // Helper template function for deduction
    template<nmac::ct_string Format, typename... Args>
    void println(Args&&... args) {
        FormatTemplate<Format, Args...>::println(std::forward<Args>(args)...);
    }

    // Helper template function for formatting without printing
    template<nmac::ct_string Format, typename... Args>
    std::string format(Args&&... args) {
        return FormatTemplate<Format, Args...>::format(std::forward<Args>(args)...);
    }
}