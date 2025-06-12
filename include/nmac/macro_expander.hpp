#pragma once

#include "nmac.hpp"
#include <optional>
#include <stdexcept>

namespace nmac::macro {
    template<typename ...Rules>
    class Expander {
        static constexpr size_t rule_count = sizeof...(Rules);

        template<size_t I, typename Input>
        static auto try_match_rule(const Input& input) {
            if constexpr (I >= rule_count) {
                throw std::runtime_error("No matching macro rule found");
            } else {
                using Rule = std::tuple_element_t<I, std::tuple<Rules...>>;

                auto Pattern = Rule::parse_pattern();
            }
        }
    };
}