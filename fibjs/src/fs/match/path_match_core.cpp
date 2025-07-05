/*
 * path_match_core.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"

namespace fibjs {

// Core matching logic implementation

bool MinimatchPattern::match(std::string_view text) const
{
    if (!compiled_) {
        return false;
    }

    // Handle empty pattern
    if (tokens_.empty()) {
        return text.empty();
    }

    return matchTokens(text, 0, 0);
}

bool MinimatchPattern::matchTokens(std::string_view text, size_t tokenIndex, size_t textIndex) const
{
    // Base case: consumed all tokens
    if (tokenIndex >= tokens_.size()) {
        return textIndex >= text.length();
    }

    // Base case: consumed all text but still have tokens
    if (textIndex >= text.length()) {
        // Special case: if we have a single * token and empty text, return false (Node.js behavior)
        if (tokens_.size() == 1 && tokens_[0].type == TokenType::STAR) {
            return false;
        }

        // Check if remaining tokens can match empty string
        for (size_t i = tokenIndex; i < tokens_.size(); i++) {
            const auto& token = tokens_[i];
            if (token.type == TokenType::STAR || token.type == TokenType::GLOBSTAR) {
                continue; // These can match empty string
            } else if (token.type == TokenType::BRACE) {
                // Check if BRACE has an empty alternative
                bool hasEmptyAlternative = false;
                for (const auto& alternative : token.alternatives) {
                    if (alternative.empty() || (alternative.size() == 1 && alternative[0].type == TokenType::LITERAL && alternative[0].value.empty())) {
                        hasEmptyAlternative = true;
                        break;
                    }
                }
                if (!hasEmptyAlternative) {
                    return false;
                }
            } else if (token.type == TokenType::EXTGLOB) {
                // Check if EXTGLOB has an empty alternative
                bool hasEmptyAlternative = false;
                for (const auto& alternative : token.alternatives) {
                    if (alternative.empty()) {
                        hasEmptyAlternative = true;
                        break;
                    }
                }
                if (!hasEmptyAlternative) {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    }

    const Token& token = tokens_[tokenIndex];

    switch (token.type) {
    case TokenType::LITERAL: {
        size_t newTextIndex = textIndex;
        if (matchLiteral(text, token.value, newTextIndex)) {
            return matchTokens(text, tokenIndex + 1, newTextIndex);
        }
        return false;
    }

    case TokenType::QUESTION: {
        size_t newTextIndex = textIndex;
        if (matchQuestion(text, newTextIndex)) {
            return matchTokens(text, tokenIndex + 1, newTextIndex);
        }
        return false;
    }

    case TokenType::STAR: {
        size_t newTextIndex = textIndex;
        return matchStar(text, tokenIndex, newTextIndex);
    }

    case TokenType::CHAR_CLASS: {
        size_t newTextIndex = textIndex;
        if (matchCharClass(text, token.value, token.negated, newTextIndex)) {
            return matchTokens(text, tokenIndex + 1, newTextIndex);
        }
        return false;
    }

    case TokenType::PATH_SEP: {
        if (textIndex < text.length() && isPathSeparatorInText(text[textIndex])) {
            // Skip one separator
            size_t newTextIndex = textIndex + 1;

            // On Windows, skip consecutive separators in text to normalize them
            if (isWindows_) {
                while (newTextIndex < text.length() && isPathSeparatorInText(text[newTextIndex])) {
                    newTextIndex++;
                }
            }

            return matchTokens(text, tokenIndex + 1, newTextIndex);
        }
        return false;
    }

    case TokenType::GLOBSTAR: {
        // Globstar (**) can match zero or more path segments
        size_t newTextIndex = textIndex;
        return matchGlobstar(text, tokenIndex, newTextIndex);
    }

    case TokenType::EXTGLOB: {
        const auto& token = tokens_[tokenIndex];

        // Special handling for ? and * extglobs that need backtracking
        if (!token.value.empty() && (token.value[0] == '?' || token.value[0] == '*')) {
            return matchExtglobWithBacktracking(text, token, tokenIndex, textIndex);
        }

        // For @ extglob, we also need backtracking when it contains wildcards
        if (!token.value.empty() && token.value[0] == '@') {
            // Check if any alternative contains wildcard that might need backtracking
            bool needsBacktracking = false;
            for (const auto& alternative : token.alternatives) {
                for (const auto& altToken : alternative) {
                    if (altToken.type == TokenType::STAR) {
                        needsBacktracking = true;
                        break;
                    }
                }
                if (needsBacktracking)
                    break;
            }

            if (needsBacktracking) {
                return matchExtglobWithBacktracking(text, token, tokenIndex, textIndex);
            }
        }

        size_t newTextIndex = textIndex;
        if (matchExtglob(text, token, tokenIndex, newTextIndex)) {
            return matchTokens(text, tokenIndex + 1, newTextIndex);
        }
        return false;
    }

    case TokenType::BRACE: {
        // Brace expansion: try each alternative
        for (size_t altIdx = 0; altIdx < token.alternatives.size(); altIdx++) {
            const auto& alternative = token.alternatives[altIdx];
            size_t newTextIndex = textIndex;
            if (matchAlternativeTokens(text, alternative, newTextIndex)) {
                if (matchTokens(text, tokenIndex + 1, newTextIndex)) {
                    return true;
                }
            }
        }
        return false;
    }

    default:
        return false;
    }
}

bool MinimatchPattern::matchLiteral(std::string_view text, const std::string& literal, size_t& textIndex) const
{
    if (textIndex + literal.length() > text.length()) {
        return false;
    }

    size_t literalIndex = 0;
    size_t currentTextIndex = textIndex;

    while (literalIndex < literal.length() && currentTextIndex < text.length()) {
        char textChar = text[currentTextIndex];
        char patternChar = literal[literalIndex];

        // Handle path separator matching on Windows
        if (isWindows_ && isPathSeparator(textChar) && isPathSeparator(patternChar)) {
            // On Windows, both / and \ are equivalent in patterns and text
            // Skip consecutive path separators in both text and pattern

            // Skip consecutive separators in text
            while (currentTextIndex < text.length() && isPathSeparator(text[currentTextIndex])) {
                currentTextIndex++;
            }

            // Skip consecutive separators in pattern
            while (literalIndex < literal.length() && isPathSeparator(literal[literalIndex])) {
                literalIndex++;
            }

            continue;
        }

        // Check for drive letter context on Windows (e.g., 'c:' vs 'C:')
        if (isWindows_ && literalIndex == 0 && currentTextIndex == 0 && literalIndex + 1 < literal.length() && currentTextIndex + 1 < text.length() && literal[literalIndex + 1] == ':' && text[currentTextIndex + 1] == ':' && ((textChar >= 'A' && textChar <= 'Z') || (textChar >= 'a' && textChar <= 'z')) && ((patternChar >= 'A' && patternChar <= 'Z') || (patternChar >= 'a' && patternChar <= 'z'))) {
            // This is a drive letter context on Windows - use case insensitive matching
            if (!shouldMatchCaseDriveLetter(textChar, patternChar)) {
                return false;
            }
        } else if (!shouldMatchCase(textChar, patternChar)) {
            return false;
        }

        currentTextIndex++;
        literalIndex++;
    }

    // Check if we consumed the entire literal
    if (literalIndex < literal.length()) {
        return false;
    }

    textIndex = currentTextIndex;
    return true;
}

bool MinimatchPattern::matchQuestion(std::string_view text, size_t& textIndex) const
{
    if (textIndex >= text.length()) {
        return false;
    }

    char c = text[textIndex];

    // ? should not match path separators
    if (isPathSeparatorInText(c)) {
        return false;
    }

    textIndex++;
    return true;
}

bool MinimatchPattern::matchGlobstar(std::string_view text, size_t tokenIndex, size_t& textIndex) const
{
    // Globstar (**) can match:
    // 1. Zero characters (but need to handle path separators correctly)
    // 2. Any sequence of characters including path separators

    // If we're at the end of the pattern, globstar matches everything remaining
    if (tokenIndex + 1 >= tokens_.size()) {
        textIndex = text.length();
        return true;
    }

    // Check if the next token is a path separator
    const Token& nextToken = tokens_[tokenIndex + 1];

    if (nextToken.type == TokenType::PATH_SEP) {
        // Pattern like a/**/c
        // First try to match zero segments (a//c -> a/c)
        if (tokenIndex + 2 < tokens_.size()) {
            size_t savedIndex = textIndex;
            if (matchTokens(text, tokenIndex + 2, textIndex)) {
                return true;
            }
            textIndex = savedIndex;
        }

        // Then try to match one or more segments
        for (size_t i = textIndex; i <= text.length(); i++) {
            if (i < text.length() && !isPathSeparatorInText(text[i])) {
                continue; // Skip non-separator characters
            }
            // Found a separator or end of string
            textIndex = i;
            if (matchTokens(text, tokenIndex + 1, textIndex)) {
                return true;
            }
        }
    } else {
        // No path separator after globstar, match any characters
        for (size_t i = textIndex; i <= text.length(); i++) {
            textIndex = i;
            if (matchTokens(text, tokenIndex + 1, textIndex)) {
                return true;
            }
        }
    }

    return false;
}

bool MinimatchPattern::matchStar(std::string_view text, size_t tokenIndex, size_t& textIndex) const
{
    // Special case: if * is the only token and text is empty, should return false (Node.js behavior)
    if (tokens_.size() == 1 && text.empty()) {
        return false;
    }

    // Try matching zero characters first (greedy backtracking)
    if (matchTokens(text, tokenIndex + 1, textIndex)) {
        return true;
    }

    // Try matching one or more characters
    while (textIndex < text.length()) {
        char c = text[textIndex];

        // * should not match path separators
        if (isPathSeparatorInText(c)) {
            break;
        }

        textIndex++;

        if (matchTokens(text, tokenIndex + 1, textIndex)) {
            return true;
        }
    }

    return false;
}

bool MinimatchPattern::matchCharClass(std::string_view text, const std::string& charClass, bool negated, size_t& textIndex) const
{
    if (textIndex >= text.length()) {
        return false;
    }

    char c = text[textIndex];
    bool matches = isCharInClass(c, charClass, false); // Don't apply negation here

    if (negated) {
        matches = !matches;
    }

    if (matches) {
        textIndex++;
        return true;
    }

    return false;
}

}
