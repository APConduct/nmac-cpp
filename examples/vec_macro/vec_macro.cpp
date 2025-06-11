#include "nmac/nmac.hpp"
#include <iostream>
#include <vector>
#include <string>

// Define the three vec! macro patterns from the prototype
namespace vec_macro {
    struct VecEmptyGenerator {
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures&) {
            return std::vector<int>{}; // Empty vector
        }
    };

    struct VecListGenerator {
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures& captures) {
            std::vector<int> result;
            result.reserve(captures.size());

            for (const auto& [name, token] : captures) {
                if (name == "expr") {
                    // In a real implementation, we would parse the token
                    // For now, just use a simple conversion
                    try {
                        result.push_back(std::stoi(std::string(token)));
                    } catch (...) {
                        result.push_back(0); // Default value for conversion errors
                    }
                }
            }

            return result;
        }
    };

    struct VecRepeatGenerator {
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures& captures) {
            int value = 0;
            int count = 0;

            for (const auto& [name, token] : captures) {
                try {
                    if (name == "expr") {
                        value = std::stoi(std::string(token));
                    } else if (name == "count") {
                        count = std::stoi(std::string(token));
                    }
                } catch (...) {
                    // Ignore conversion errors
                }
            }

            return std::vector<int>(count, value);
        }
    };

    using VecEmpty = nmac::MacroRule<"vec! [ ]", VecEmptyGenerator>;
    using VecList = nmac::MacroRule<"vec! [ $expr+ ]", VecListGenerator>;
    using VecRepeat = nmac::MacroRule<"vec! [ $expr ; $count ]", VecRepeatGenerator>;

    using VecExpander = nmac::MacroExpander<VecEmpty, VecList, VecRepeat>;
}

int main() {
    // Example usage
    std::cout << "Vec Macro Example\n";

    // Would demonstrate different vec! patterns here
    // This would be implemented when the MacroExpander is fully working

    return 0;
}
