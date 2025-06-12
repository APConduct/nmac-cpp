#pragma once

#include "nmac.hpp"
#include <optional>
#include <stdexcept>

namespace nmac {
    namespace macro{
        template <typename... Rules>
       class Expander {
            static constexpr size_t rule_count = sizeof...(Rules);

            template <size_t I, typename Input>
            static auto try_match_rule(const Input& input) {
                if constexpr (I >= rule_count) {
                    throw std::runtime_error("No matching macro rule found");
                }
                else {
                    using Rule = std::tuple_element_t<I, std::tuple<Rules...>>;

                    auto pattern = Rule::parse_pattern();

                    PatternMatcher matcher(pattern, input);
                    if (matcher.match()) {
                        return Rule::generator::expand(input, matcher.get_captures());
                    } else {
                        return try_match_rule<I + 1>(input);
                    }
                }
            }
        public:
            template<typename Input>
            static auto expand(const Input& input) {
                try {
                    return try_match_rule<0>(input);
                } catch (const std::exception& e) {
                    // TODO: Handle errors more gracefully
                    throw;
                }
            }

            template<typename Input>
            static auto try_expand(const Input& input) -> std::optional<decltype(try_match_rule<0>(std::declval<Input>()))> {
                try {
                    return try_match_rule<0>(input);
                } catch (const std::exception& e) {
                    return std::nullopt;
                }
            }
        };
    }
}
