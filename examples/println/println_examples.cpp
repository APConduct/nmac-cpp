#include "nmac/dsl/println.hpp"
#include <iostream>
#include <string>

int main() {
    int answer = 42;
    std::string name = "World";
    double pi = 3.14159;

    std::cout << "== Basic println examples ==\n\n";

    // Using the function directly
    nmac::dsl::println("Hello, {}!", name);
    nmac::dsl::println("The answer is: {}", answer);
    nmac::dsl::println("Pi is approximately {}", pi);

    // Multiple arguments
    nmac::dsl::println("Hello, {}! The answer is: {}", name, answer);

    // Using the convenience macro
    PRINTLN("Using the PRINTLN macro: {}", answer);

    // Format without printing
    std::string formatted = nmac::dsl::format("Formatted: {}", pi);
    std::cout << "Result: " << formatted << std::endl;

    return 0;
}
