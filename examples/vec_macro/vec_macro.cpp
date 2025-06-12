#include "nmac/nmac.hpp"
#include <vector>
#include <variant>

// Example: Enhanced vec! macro with multiple patterns
namespace vec_macro {
    struct VecEmptyGenerator {
        // The original method with Input and Captures
        template<typename Input, typename Captures>
        static auto expand(const Input&, const Captures&) {
            return std::vector<int>{}; // Default to int, could be templated
        }

        // Add this method to satisfy MacroGeneratorType
        template<typename Tuple>
        static auto expand(const Tuple&) {
            return std::vector<int>{};
        }
    };

    struct VecListGenerator {
        // The original method with Input and Captures
        template<typename Input, typename Captures>
        static auto expand(const Input& input, const Captures& captures) {
            std::vector<int> result;
            // In practice, you'd parse the captured expressions
            result.reserve(captures.size());
            for (const auto& [name, token] : captures) {
                if (name == "expr") {
                    // Simplified: assume tokens contain integer values
                    result.push_back(42); // Placeholder
                }
            }
            return result;
        }

        // Add this method to satisfy MacroGeneratorType
        template<typename Tuple>
        static auto expand(const Tuple&) {
            return std::vector<int>{};
        }
    };

    struct VecRepeatGenerator {
        // The original method with Input and Captures
        template<typename Input, typename Captures>
        static auto expand(const Input& input, const Captures& captures) {
            // vec![expr; count] pattern
            int value = 42; // From captured expr
            int count = 5;  // From captured count
            return std::vector<int>(count, value);
        }

        // Add this method to satisfy MacroGeneratorType
        template<typename Tuple>
        static auto expand(const Tuple&) {
            return std::vector<int>{5, 42}; // Just a placeholder
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

    // This would demonstrate different vec! patterns
    // For now, we'll just create vectors directly
    auto empty_vec = std::vector<int>{};
    auto list_vec = std::vector<int>{1, 2, 3};
    auto repeat_vec = std::vector<int>(5, 42);

    std::cout << "Empty vector size: " << empty_vec.size() << "\n";
    std::cout << "List vector size: " << list_vec.size() << "\n";
    std::cout << "Repeat vector size: " << repeat_vec.size() << "\n";

    return 0;
}
