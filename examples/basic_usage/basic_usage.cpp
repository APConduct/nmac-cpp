#include "nmac/nmac.hpp"
#include <iostream>

int main() {
    // Create a vector using the macro
    auto vec = VEC_MACRO(1, 2, 3, 4, 5);
    std::cout << "Vector size: " << vec.size() << std::endl;

    // Pattern matching example
    int x = 42;
    MATCH(x,
        ARM(42, std::cout << "Found answer!" << "\n"),
        ARM(0, std::cout << "Found zero!" << "\n")
    );

    // Expression DSL example
    using namespace expression_dsl;
    constexpr auto expr = lit(10) + lit(20) + var<"x">();

    struct Context {
        [[nodiscard]] static constexpr int get(nmac::ct_string<2> name) {
            if (name.view() == "x") return 5;
            return 0;
        }
    };

    constexpr Context ctx;
    constexpr auto result = expr.eval(ctx); // Should be 35 at compile time

    std::cout << "Expression result: " << result << "\n";

    return 0;
}
