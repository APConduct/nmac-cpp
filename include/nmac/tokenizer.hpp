#pragma once

#include "nmac.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

namespace nmac {
    class Tokenizer {
        std::string_view source;
        size_t pos = 0;
        size_t line = 1;
        size_t column = 0;

        static inline const std::unordered_set<std::string_view> keywords = {
            "vec", "println", "match", "if", "else", "for", "while", "return",
            "true", "false", "null", "struct", "class", "enum", "template", "auto"
        };

        static inline const std::unordered_map<char, TokenType> punct_tokens = {
            {'(', LPAREN},
            {')', RPAREN},
            {'[', LBRACE},
            {']', RBRACE},
            {',', COMMA},
            {';', SEMICOLON},
            {'+', PUNCT},
            {'-', PUNCT},
            {'*', PUNCT},
            {'/', PUNCT},
            {'=', PUNCT},
            {'<', PUNCT},
            {'>', PUNCT},
            {'!', PUNCT},
            {'&', PUNCT},
            {'|', PUNCT},
            {'%', PUNCT},
            {'^', PUNCT},
            {'~', PUNCT},
            {'?', PUNCT},
            {':', PUNCT}
        };

        char peek() const {
            return pos < source.size() ? source[pos] : '\0';
        }

        char advance() {
            char c = peek();
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            if (pos < source.size()) pos++;
            return c;
        }

        void skip_whitespace() {
            while (pos < source.size() && std::isspace(source[pos])) {
                advance();
            }
        }

        void skip_line_comment() {
            while (pos < source.size() && source[pos] != '\n') {
                advance();
            }
        }

        void skip_block_comment() {
            while (pos < source.size()) {
                if (peek() == '*' && pos + 1 < source.size() && source[pos + 1] == '/') {
                    advance(); // Skip '*'
                    advance(); // Skip '/'
                    return;
                }
                advance();
            }
        }

        Token scan_identifier() {
            size_t start = pos;
            size_t start_column = column;

            while (pos < source.size() &&
                   (std::isalnum(source[pos]) || source[pos] == '_')) {
                advance();
                   }

            std::string_view text = source.substr(start, pos - start);

            // Check if this is a keyword
            if (keywords.find(text) != keywords.end()) {
                return Token(KEYWORD, text, start_column);
            }

            return Token(IDENT, text, start_column);
        }

        Token scan_number() {
            size_t start = pos;
            size_t start_column = column;

            // Integer part
            while (pos < source.size() && std::isdigit(source[pos])) {
                advance();
            }

            // Decimal part
            if (peek() == '.' && pos + 1 < source.size() &&
                std::isdigit(source[pos + 1])) {
                advance(); // Skip '.'

                while (pos < source.size() && std::isdigit(source[pos])) {
                    advance();
                }
                }

            // Exponent part
            if ((peek() == 'e' || peek() == 'E') && pos + 1 < source.size()) {
                size_t next_pos = pos + 1;
                if (std::isdigit(source[next_pos]) ||
                    ((source[next_pos] == '+' || source[next_pos] == '-') &&
                     next_pos + 1 < source.size() && std::isdigit(source[next_pos + 1]))) {
                    advance(); // Skip 'e' or 'E'

                    if (peek() == '+' || peek() == '-') {
                        advance();
                    }

                    while (pos < source.size() && std::isdigit(source[pos])) {
                        advance();
                    }
                     }
            }

            std::string_view text = source.substr(start, pos - start);
            return Token(LITERAL, text, start_column);
        }

        Token scan_string() {
            size_t start = pos;
            size_t start_column = column;

            advance(); // Skip opening quote

            while (pos < source.size() && peek() != '"') {
                if (peek() == '\\' && pos + 1 < source.size()) {
                    advance(); // Skip escape character
                }
                advance();
            }

            if (pos >= source.size()) {
                throw std::runtime_error("Unterminated string literal");
            }

            advance(); // Skip closing quote

            std::string_view text = source.substr(start, pos - start);
            return Token(LITERAL, text, start_column);
        }

    public:
        explicit Tokenizer(std::string_view src) : source(src) {}


        std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (pos < source.size()) {
            skip_whitespace();

            if (pos >= source.size()) break;

            char c = peek();

            // Handle comments
            if (c == '/' && pos + 1 < source.size()) {
                if (source[pos + 1] == '/') {
                    advance(); // Skip '/'
                    advance(); // Skip '/'
                    skip_line_comment();
                    continue;
                } else if (source[pos + 1] == '*') {
                    advance(); // Skip '/'
                    advance(); // Skip '*'
                    skip_block_comment();
                    continue;
                }
            }

            if (std::isalpha(c) || c == '_') {
                tokens.push_back(scan_identifier());
            } else if (std::isdigit(c)) {
                tokens.push_back(scan_number());
            } else if (c == '"') {
                tokens.push_back(scan_string());
            } else {
                // Check for punctuation tokens
                auto it = punct_tokens.find(c);
                if (it != punct_tokens.end()) {
                    tokens.push_back(Token(it->second, source.substr(pos, 1), column));
                    advance();
                } else {
                    // Unknown character
                    throw std::runtime_error(std::string("Unexpected character: ") + c);
                }
            }
        }

        return tokens;
    }

    };
}