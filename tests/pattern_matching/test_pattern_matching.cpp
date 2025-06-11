#include "nmac/nmac.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

// Simple debug function to print information about an input vector
void print_input(const std::vector<std::string>& input) {
    std::cout << "Input vector contains " << input.size() << " elements:\n";
    for (size_t i = 0; i < input.size(); ++i) {
        std::cout << "  [" << i << "]: \"" << input[i] << "\"\n";
    }
}

// Helper function to print pattern structure
void print_pattern(const nmac::PatternNode& pattern, int indent = 0) {
    std::string indent_str(indent * 2, ' ');
    std::cout << indent_str << "Type: " << pattern.type
              << ", Content: '" << pattern.content
              << "', Position: " << pattern.source_position << std::endl;

    if (!pattern.children.empty()) {
        std::cout << indent_str << "Children (" << pattern.children.size() << "):" << std::endl;
        for (const auto& child : pattern.children) {
            print_pattern(child, indent + 1);
        }
    }
}

void test_pattern_parser() {
    // Test the enhanced pattern parser
    std::cout << "Testing enhanced pattern parser with basic pattern\n";
    nmac::PatternParser parser("$var1 + $var2");
    auto pattern = parser.parse();

    std::cout << "Parsed pattern structure:\n";
    print_pattern(pattern);

    // Verify the pattern has the expected structure
    assert(pattern.type == nmac::PatternNode::SEQUENCE);
    assert(pattern.children.size() == 3);

    // First child should be a variable
    assert(pattern.children[0].type == nmac::PatternNode::VARIABLE);
    assert(pattern.children[0].content == "var1");

    // Second child should be an operator
    assert(pattern.children[1].type == nmac::PatternNode::OPERATOR);
    assert(pattern.children[1].content == "+");

    // Third child should be a variable
    assert(pattern.children[2].type == nmac::PatternNode::VARIABLE);
    assert(pattern.children[2].content == "var2");

    // Test parsing with escaped characters
    std::cout << "\nTesting pattern with escaped characters\n";
    nmac::PatternParser parser2("$var1 \\+ $var2");
    auto pattern2 = parser2.parse();

    std::cout << "Parsed pattern with escaped characters:\n";
    print_pattern(pattern2);

    assert(pattern2.type == nmac::PatternNode::SEQUENCE);
    assert(pattern2.children.size() == 3);
    assert(pattern2.children[1].type == nmac::PatternNode::LITERAL);
    assert(pattern2.children[1].content == "+");

    // Test pattern with repetition
    std::cout << "\nTesting pattern with repetition\n";
    nmac::PatternParser parser3("$var1+ $var2");
    auto pattern3 = parser3.parse();

    std::cout << "Parsed pattern with repetition:\n";
    print_pattern(pattern3);

    assert(pattern3.type == nmac::PatternNode::SEQUENCE);
    assert(pattern3.children.size() == 2);
    assert(pattern3.children[0].type == nmac::PatternNode::REPETITION);
    assert(pattern3.children[0].content == "+");
    assert(pattern3.children[0].children.size() == 1);
    assert(pattern3.children[0].children[0].type == nmac::PatternNode::VARIABLE);
    assert(pattern3.children[0].children[0].content == "var1");

    // Test error reporting
    std::cout << "\nTesting error reporting\n";
    nmac::PatternParser parser4("$var1 + $");
    auto pattern4 = parser4.parse();

    if (parser4.has_error()) {
        std::cout << "Error detected: " << parser4.get_error_message()
                  << " at position " << parser4.get_error_position() << std::endl;
    }

    assert(parser4.has_error());
}

void test_pattern_matcher() {
    // Test basic matching with the enhanced matcher
    std::cout << "\nTesting enhanced pattern matching\n";

    // Create a pattern: "$var1 + $var2"
    nmac::PatternNode pattern(nmac::PatternNode::SEQUENCE);
    pattern.children.push_back(nmac::PatternNode(nmac::PatternNode::VARIABLE, "var1"));
    pattern.children.push_back(nmac::PatternNode(nmac::PatternNode::OPERATOR, "+"));
    pattern.children.push_back(nmac::PatternNode(nmac::PatternNode::VARIABLE, "var2"));

    // Input data
    std::vector<std::string> input = {"10", "+", "20"};
    print_input(input);

    // Create matcher
    nmac::PatternMatcher<std::vector<std::string>> matcher(pattern, input);

    // Match
    bool match_result = matcher.match();
    std::cout << "Match result: " << (match_result ? "true" : "false") << std::endl;

    if (!match_result && matcher.has_error()) {
        std::cout << "Match error: " << matcher.get_error_message()
                  << " at position " << matcher.get_error_position() << std::endl;
    }

    assert(match_result);

    // Check captures
    auto captures = matcher.get_captures();
    assert(captures.size() == 2);
    assert(captures[0].first == "var1" && captures[0].second == "10");
    assert(captures[1].first == "var2" && captures[1].second == "20");

    // Test matching with errors
    std::cout << "\nTesting matcher with error cases\n";

    std::vector<std::string> bad_input = {"10", "-", "20"};
    print_input(bad_input);

    nmac::PatternMatcher<std::vector<std::string>> bad_matcher(pattern, bad_input);
    bool bad_match_result = bad_matcher.match();

    std::cout << "Bad match result: " << (bad_match_result ? "true" : "false") << std::endl;

    if (!bad_match_result && bad_matcher.has_error()) {
        std::cout << "Bad match error: " << bad_matcher.get_error_message()
                  << " at position " << bad_matcher.get_error_position() << std::endl;
    }

    assert(!bad_match_result);
    assert(bad_matcher.has_error());
}

void test_repetition_matching() {
    std::cout << "\nTesting repetition matching\n";

    // Create a pattern with repetition: "$item+"
    nmac::PatternNode seq(nmac::PatternNode::SEQUENCE);
    nmac::PatternNode var(nmac::PatternNode::VARIABLE, "item");
    nmac::PatternNode rep(nmac::PatternNode::REPETITION, "+");
    rep.children.push_back(var);
    seq.children.push_back(rep);

    // Input with multiple items
    std::vector<std::string> input = {"a", "b", "c"};
    print_input(input);

    nmac::PatternMatcher<std::vector<std::string>> matcher(seq, input);
    bool match_result = matcher.match();

    std::cout << "Repetition match result: " << (match_result ? "true" : "false") << std::endl;

    if (match_result) {
        auto captures = matcher.get_captures();
        std::cout << "Captures (" << captures.size() << "):" << std::endl;
        for (const auto& capture : captures) {
            std::cout << "  " << capture.first << " = " << capture.second << std::endl;
        }
    } else if (matcher.has_error()) {
        std::cout << "Match error: " << matcher.get_error_message()
                  << " at position " << matcher.get_error_position() << std::endl;
    }

    assert(match_result);
    assert(matcher.get_captures().size() == 3);
}

int main() {
    std::cout << "Starting enhanced pattern parser test\n";
    test_pattern_parser();
    std::cout << "Enhanced pattern parser test completed\n";

    std::cout << "\nStarting enhanced pattern matcher test\n";
    test_pattern_matcher();
    std::cout << "Enhanced pattern matcher test completed\n";

    std::cout << "\nStarting repetition matching test\n";
    test_repetition_matching();
    std::cout << "Repetition matching test completed\n";

    return 0;
}
