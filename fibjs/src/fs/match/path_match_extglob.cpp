/*
 * path_match_extglob.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"

namespace fibjs {

// Extglob handling implementation

bool MinimatchPattern::matchExtglob(std::string_view text, const Token& token, size_t tokenIndex, size_t& textIndex) const
{
    if (token.value.empty()) {
        return false;
    }

    char extType = token.value[0];

    switch (extType) {
    case '@': {
        // @(...) - match exactly one of the alternatives

        // Handle empty alternatives case
        if (token.alternatives.empty()) {
            return true; // Empty extglob matches empty string
        }

        for (size_t i = 0; i < token.alternatives.size(); i++) {
            size_t savedIndex = textIndex;

            // Try to match current alternative
            if (matchAlternativeTokens(text, token.alternatives[i], textIndex)) {
                return true; // textIndex is already updated by matchAlternativeTokens
            }

            textIndex = savedIndex; // Restore position for next attempt
        }
        return false;
    }
    case '?': {
        // ?(...) - match zero or one occurrence
        size_t savedIndex = textIndex;

        // Try each alternative once
        for (size_t i = 0; i < token.alternatives.size(); i++) {
            textIndex = savedIndex;
            if (matchAlternativeTokens(text, token.alternatives[i], textIndex)) {
                return true;
            }
        }

        // If no alternative matched, try zero match
        // For zero match, we don't advance textIndex
        textIndex = savedIndex;
        return true; // Always succeeds for ?(...)
    }

    case '+': {
        // +(...) - match one or more occurrences
        return matchExtglobRepeat(text, token.alternatives, textIndex, 1, SIZE_MAX);
    }

    case '*': {
        // *(...) - match zero or more occurrences
        return matchExtglobRepeat(text, token.alternatives, textIndex, 0, SIZE_MAX);
    }

    case '!': {
        // !(...) - match anything except the alternatives
        size_t savedIndex = textIndex;

        for (size_t i = 0; i < token.alternatives.size(); i++) {
            textIndex = savedIndex;
            if (matchAlternativeTokens(text, token.alternatives[i], textIndex)) {
                return false; // Found a match, so negation fails
            }
        }

        // No alternatives matched, so we need to consume one character
        if (textIndex < text.length()) {
            textIndex++;
            return true;
        }

        return false;
    }

    default:
        return false;
    }
}

bool MinimatchPattern::matchAlternativeTokens(std::string_view text, const std::vector<Token>& tokens, size_t& textIndex) const
{
    // Handle empty token list (empty alternative)
    if (tokens.empty()) {
        return true; // Empty alternative always matches
    }

    // For single token, handle directly
    if (tokens.size() == 1) {
        const Token& token = tokens[0];

        switch (token.type) {
        case TokenType::LITERAL:
            return matchLiteral(text, token.value, textIndex);
        case TokenType::STAR:
            // For single star in alternative, still use greedy matching
            // This is appropriate when the star is the complete alternative
            return matchStarInAlternative(text, textIndex);
        case TokenType::QUESTION:
            return matchQuestion(text, textIndex);
        case TokenType::CHAR_CLASS:
            return matchCharClass(text, token.value, token.negated, textIndex);
        case TokenType::EXTGLOB:
            return matchExtglob(text, token, 0, textIndex);
        default:
            return false;
        }
    }

    // For multiple tokens, we need to match them in sequence
    return matchTokenSequence(text, tokens, 0, textIndex);
}

bool MinimatchPattern::matchTokenSequence(std::string_view text, const std::vector<Token>& tokens, size_t tokenIndex, size_t& textIndex) const
{
    // Base case: all tokens matched
    if (tokenIndex >= tokens.size()) {
        return true;
    }

    const Token& token = tokens[tokenIndex];
    size_t savedIndex = textIndex;

    switch (token.type) {
    case TokenType::LITERAL: {
        if (matchLiteral(text, token.value, textIndex)) {
            if (matchTokenSequence(text, tokens, tokenIndex + 1, textIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    case TokenType::STAR: {
        // Calculate maximum possible match length
        size_t maxMatch = textIndex;
        while (maxMatch < text.length() && !isPathSeparatorInText(text[maxMatch])) {
            maxMatch++;
        }

        // If this is the last token, match greedily (longest first)
        if (tokenIndex + 1 >= tokens.size()) {
            textIndex = maxMatch;
            return true;
        }

        // Try from shortest to longest (non-greedy, backtrack if needed)
        // This allows following tokens to have a chance to match
        for (size_t len = 0; len <= maxMatch - textIndex; len++) {
            size_t tryIndex = textIndex + len;

            if (matchTokenSequence(text, tokens, tokenIndex + 1, tryIndex)) {
                textIndex = tryIndex;
                return true;
            }
        }

        textIndex = savedIndex;
        return false;
    }

    case TokenType::QUESTION: {
        if (matchQuestion(text, textIndex)) {
            if (matchTokenSequence(text, tokens, tokenIndex + 1, textIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    case TokenType::CHAR_CLASS: {
        if (matchCharClass(text, token.value, token.negated, textIndex)) {
            if (matchTokenSequence(text, tokens, tokenIndex + 1, textIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    case TokenType::EXTGLOB: {
        if (matchExtglob(text, token, 0, textIndex)) {
            if (matchTokenSequence(text, tokens, tokenIndex + 1, textIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    case TokenType::BRACE: {
        // Handle brace expansion
        for (const auto& alternative : token.alternatives) {
            size_t braceIndex = textIndex;
            if (matchAlternativeTokens(text, alternative, braceIndex)) {
                if (matchTokenSequence(text, tokens, tokenIndex + 1, braceIndex)) {
                    textIndex = braceIndex;
                    return true;
                }
            }
        }
        textIndex = savedIndex;
        return false;
    }

    default:
        return false;
    }
}

bool MinimatchPattern::matchStarInAlternative(std::string_view text, size_t& textIndex) const
{
    size_t startIndex = textIndex;

    // For * in alternatives when it's the only token, match all remaining characters
    // This is used when * is a standalone token in an alternative
    while (textIndex < text.length() && !isPathSeparatorInText(text[textIndex])) {
        textIndex++;
    }

    // * always succeeds (can match zero characters)
    return true;
}

bool MinimatchPattern::matchExtglobRepeat(std::string_view text, const std::vector<std::vector<Token>>& alternatives, size_t& textIndex, size_t minCount, size_t maxCount) const
{
    size_t matchCount = 0;
    size_t savedIndex = textIndex;

    // For zero minimum count (like *()), we always succeed even if no matches
    if (minCount == 0 && maxCount > 0) {
        // Try to match as many as possible, but zero is OK
        while (matchCount < maxCount && textIndex < text.length()) {
            bool foundMatch = false;
            size_t iterStart = textIndex;

            // Try each alternative
            for (size_t i = 0; i < alternatives.size(); i++) {
                textIndex = iterStart;
                if (matchAlternativeTokens(text, alternatives[i], textIndex)) {
                    foundMatch = true;
                    matchCount++;
                    break;
                }
            }

            if (!foundMatch) {
                break;
            }

            // Prevent infinite loop on empty matches
            if (textIndex == iterStart) {
                break;
            }
        }
        return true; // Always succeed for *(...)
    }

    // For non-zero minimum count (like +(...))
    while (matchCount < maxCount && textIndex < text.length()) {
        bool foundMatch = false;
        size_t iterStart = textIndex;

        // Try each alternative
        for (size_t i = 0; i < alternatives.size(); i++) {
            textIndex = iterStart;
            if (matchAlternativeTokens(text, alternatives[i], textIndex)) {
                foundMatch = true;
                matchCount++;
                break;
            }
        }

        if (!foundMatch) {
            break;
        }

        // Prevent infinite loop on empty matches
        if (textIndex == iterStart) {
            break;
        }
    }

    if (matchCount >= minCount) {
        return true;
    }

    textIndex = savedIndex;
    return false;
}

bool MinimatchPattern::matchStarInExtglob(std::string_view text, size_t& textIndex) const
{
    // * in extglob should match zero or more characters (not including path separators)
    // But unlike regular *, we need to be more careful about backtracking
    // For now, let's match greedily but the caller should handle backtracking
    size_t originalIndex = textIndex;

    // Try to match zero characters first (this allows * to match nothing)
    // This will be handled by the caller's backtracking logic

    // Match characters until we hit a path separator or end of string
    while (textIndex < text.length() && !isPathSeparatorInText(text[textIndex])) {
        textIndex++;
    }

    return true; // Always succeeds, even if it matches zero characters
}

bool MinimatchPattern::matchExtglobZero(std::string_view text, size_t tokenIndex, size_t& textIndex) const
{
    // For zero matches, we don't consume any text, just return true
    return true;
}

bool MinimatchPattern::matchExtglobWithBacktracking(std::string_view text, const Token& token, size_t tokenIndex, size_t& textIndex) const
{
    if (token.value.empty()) {
        return false;
    }

    char extType = token.value[0];

    if (extType == '?') {
        // ?(...) - match zero or one occurrence with backtracking
        size_t savedIndex = textIndex;

        // Try one occurrence first - match one of the alternatives
        for (size_t i = 0; i < token.alternatives.size(); i++) {
            textIndex = savedIndex;
            if (matchAlternativeTokens(text, token.alternatives[i], textIndex)) {
                // After matching alternative, try remaining tokens
                if (matchTokens(text, tokenIndex + 1, textIndex)) {
                    return true;
                }
            }
        }

        // Try zero occurrences - just try remaining tokens without advancing
        textIndex = savedIndex;
        if (matchTokens(text, tokenIndex + 1, textIndex)) {
            return true;
        }

        textIndex = savedIndex;
        return false;
    } else if (extType == '*') {
        // *(...) - match zero or more occurrences with backtracking
        size_t savedIndex = textIndex;

        // Try zero occurrences first - just try remaining tokens
        if (matchTokens(text, tokenIndex + 1, textIndex)) {
            return true;
        }

        // Try one or more occurrences
        textIndex = savedIndex;
        return matchExtglobRepeat(text, token.alternatives, textIndex, 1, SIZE_MAX) && matchTokens(text, tokenIndex + 1, textIndex);
    } else if (extType == '@') {
        // @(...) - match exactly one alternative, then let remaining tokens match
        size_t savedIndex = textIndex;

        // Try each alternative
        for (size_t i = 0; i < token.alternatives.size(); i++) {
            textIndex = savedIndex;

            // Use specialized matching for alternatives with backtracking
            if (tryAlternativeWithBacktrack(text, token.alternatives[i], textIndex, tokenIndex + 1)) {
                return true;
            } else {
            }
        }

        textIndex = savedIndex;
        return false;
    }

    // For other extglob types, use regular matching
    size_t newTextIndex = textIndex;
    if (matchExtglob(text, token, tokenIndex, newTextIndex)) {
        textIndex = newTextIndex;
        return matchTokens(text, tokenIndex + 1, textIndex);
    }
    return false;
}

bool MinimatchPattern::tryAlternativeWithBacktrack(std::string_view text, const std::vector<Token>& alternative, size_t& textIndex, size_t nextTokenIndex) const
{
    // Try to match alternative with deep backtracking
    return matchTokenSequenceWithFullBacktrack(text, alternative, 0, textIndex, nextTokenIndex);
}

bool MinimatchPattern::matchTokenSequenceWithFullBacktrack(std::string_view text, const std::vector<Token>& tokens, size_t tokenIndex, size_t& textIndex, size_t nextTokenIndex) const
{
    // Base case: all tokens in alternative matched
    if (tokenIndex >= tokens.size()) {
        // Try to match remaining tokens after this alternative
        size_t savedIndex = textIndex;
        if (matchTokens(text, nextTokenIndex, textIndex)) {
            return true;
        }
        textIndex = savedIndex;
        return false;
    }

    const Token& token = tokens[tokenIndex];
    size_t savedIndex = textIndex;

    switch (token.type) {
    case TokenType::LITERAL: {
        if (matchLiteral(text, token.value, textIndex)) {
            if (matchTokenSequenceWithFullBacktrack(text, tokens, tokenIndex + 1, textIndex, nextTokenIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    case TokenType::STAR: {
        // Calculate maximum possible match length
        size_t maxMatch = textIndex;
        while (maxMatch < text.length() && !isPathSeparatorInText(text[maxMatch])) {
            maxMatch++;
        }

        // Try from shortest to longest
        for (size_t len = 0; len <= maxMatch - textIndex; len++) {
            size_t tryIndex = textIndex + len;

            if (matchTokenSequenceWithFullBacktrack(text, tokens, tokenIndex + 1, tryIndex, nextTokenIndex)) {
                textIndex = tryIndex;
                return true;
            }
        }

        textIndex = savedIndex;
        return false;
    }

    case TokenType::QUESTION: {
        if (matchQuestion(text, textIndex)) {
            if (matchTokenSequenceWithFullBacktrack(text, tokens, tokenIndex + 1, textIndex, nextTokenIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    case TokenType::CHAR_CLASS: {
        if (matchCharClass(text, token.value, token.negated, textIndex)) {
            if (matchTokenSequenceWithFullBacktrack(text, tokens, tokenIndex + 1, textIndex, nextTokenIndex)) {
                return true;
            }
        }
        textIndex = savedIndex;
        return false;
    }

    default:
        return false;
    }
}

}
