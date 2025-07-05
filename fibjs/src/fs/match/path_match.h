/*
 * path_match.h
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <cstddef>

namespace fibjs {

// Forward declarations
class MinimatchPattern;

// Token types for pattern parsing
enum class TokenType {
    LITERAL, // Regular string literal
    STAR, // * - matches any sequence of characters except path separator
    QUESTION, // ? - matches any single character except path separator
    GLOBSTAR, // ** - matches any sequence including path separators
    CHAR_CLASS, // [abc] or [a-z] - character class
    EXTGLOB, // @(...), !(...), ?(...), +(...), *(...) - extended globs
    BRACE, // {a,b,c} - brace expansion
    PATH_SEP // / or \ - path separator
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    std::vector<Token> children; // For nested patterns like extglobs
    std::vector<std::vector<Token>> alternatives; // For extglob alternatives grouping
    bool negated = false; // For negated character classes [^abc]

    Token(TokenType t, const std::string& v = "")
        : type(t)
        , value(v)
    {
    }
};

// Main pattern matching class
class MinimatchPattern {
private:
    std::vector<Token> tokens_;
    bool isWindows_;
    std::string original_pattern_;
    bool compiled_ = false;

    // Internal parsing methods
    void tokenize(const std::string& pattern);
    bool matchTokens(std::string_view text, size_t tokenIndex = 0, size_t textIndex = 0) const;
    bool matchLiteral(std::string_view text, const std::string& literal, size_t& textIndex) const;
    bool matchStar(std::string_view text, size_t tokenIndex, size_t& textIndex) const;
    bool matchGlobstar(std::string_view text, size_t tokenIndex, size_t& textIndex) const;
    bool matchQuestion(std::string_view text, size_t& textIndex, size_t tokenIndex = 0) const;
    bool matchCharClass(std::string_view text, const std::string& charClass, bool negated, size_t& textIndex) const;

    // Character class parsing
    bool parseCharClass(const std::string& pattern, size_t& pos, Token& token);
    bool isCharInClass(char c, const std::string& charClass, bool negated) const;

    // Extglob parsing
    bool parseExtglob(const std::string& pattern, size_t& pos, Token& token);
    std::vector<Token> parseAlternativePattern(const std::string& pattern);

    // Brace expansion parsing
    bool parseBraceExpansion(const std::string& pattern, size_t& pos, Token& token);
    bool matchExtglob(std::string_view text, const Token& token, size_t tokenIndex, size_t& textIndex) const;
    bool matchExtglobWithBacktracking(std::string_view text, const Token& token, size_t tokenIndex, size_t& textIndex) const;
    bool matchAlternativeTokens(std::string_view text, const std::vector<Token>& tokens, size_t& textIndex) const;
    bool matchTokenSequence(std::string_view text, const std::vector<Token>& tokens, size_t tokenIndex, size_t& textIndex) const;
    bool matchTokenSequenceWithFullBacktrack(std::string_view text, const std::vector<Token>& tokens, size_t tokenIndex, size_t& textIndex, size_t nextTokenIndex) const;
    bool matchExtglobRepeat(std::string_view text, const std::vector<std::vector<Token>>& alternatives, size_t& textIndex, size_t minCount, size_t maxCount) const;
    bool matchStarInExtglob(std::string_view text, size_t& textIndex) const;
    bool matchStarInAlternative(std::string_view text, size_t& textIndex) const;
    bool matchExtglobZero(std::string_view text, size_t tokenIndex, size_t& textIndex) const;
    bool tryAlternativeWithBacktrack(std::string_view text, const std::vector<Token>& alternative, size_t& textIndex, size_t nextTokenIndex) const;

    // Utility methods
    bool isPathSeparator(char c) const;
    bool isPathSeparatorInText(char c) const;
    bool isSpecialChar(char c) const;
    char getPathSeparator() const;
    bool shouldMatchCase(char a, char b) const;
    bool containsHiddenPathSegments(std::string_view text, size_t startIndex, size_t endIndex = SIZE_MAX) const;

public:
    explicit MinimatchPattern(const std::string& pattern, bool isWindows);
    ~MinimatchPattern() = default;

    // Disable copy for now (can be implemented later if needed)
    MinimatchPattern(const MinimatchPattern&) = delete;
    MinimatchPattern& operator=(const MinimatchPattern&) = delete;

    // Move semantics
    MinimatchPattern(MinimatchPattern&&) = default;
    MinimatchPattern& operator=(MinimatchPattern&&) = default;

    // Main matching method
    bool match(std::string_view text) const;

    // Getters
    const std::string& getPattern() const { return original_pattern_; }
    bool isWindows() const { return isWindows_; }
    bool isCompiled() const { return compiled_; }
};

// Convenience function for direct matching (equivalent to minimatch.minimatch())
bool matchesGlob(std::string_view text, const std::string& pattern, bool isWindows);

}
