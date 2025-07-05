/*
 * path_match_parser.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"

namespace fibjs {

// Parser implementation

void MinimatchPattern::tokenize(const std::string& pattern)
{
    tokens_.clear();

    for (size_t i = 0; i < pattern.length();) {
        char c = pattern[i];

        switch (c) {
        case '*':
            // Check for *(... extglob pattern first
            if (i + 1 < pattern.length() && pattern[i + 1] == '(') {
                Token token(TokenType::EXTGLOB);
                if (parseExtglob(pattern, i, token)) {
                    tokens_.push_back(std::move(token));
                    break;
                }
                // If parsing fails, fall through to regular * handling
            }

            if (i + 1 < pattern.length() && pattern[i + 1] == '*') {
                // Check for /** or **/ or standalone **
                bool isGlobstar = true;
                if (i > 0 && !isPathSeparator(pattern[i - 1])) {
                    isGlobstar = false;
                }
                if (i + 2 < pattern.length() && !isPathSeparator(pattern[i + 2])) {
                    isGlobstar = false;
                }

                if (isGlobstar) {
                    tokens_.emplace_back(TokenType::GLOBSTAR);
                    i += 2;
                } else {
                    // Treat as regular star
                    tokens_.emplace_back(TokenType::STAR);
                    i++;
                }
            } else {
                tokens_.emplace_back(TokenType::STAR);
                i++;
            }
            break;

        case '?':
            // Check for ?(... extglob pattern first
            if (i + 1 < pattern.length() && pattern[i + 1] == '(') {
                Token token(TokenType::EXTGLOB);
                if (parseExtglob(pattern, i, token)) {
                    tokens_.push_back(std::move(token));
                    break;
                }
                // If parsing fails, fall through to regular ? handling
            }

            tokens_.emplace_back(TokenType::QUESTION);
            i++;
            break;

        case '[': {
            Token token(TokenType::CHAR_CLASS);
            if (parseCharClass(pattern, i, token)) {
                tokens_.push_back(std::move(token));
            } else {
                // Treat as literal if parsing fails
                tokens_.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
        } break;

        case '/':
        case '\\':
            if (isPathSeparator(c)) {
                // On Windows, skip consecutive path separators to normalize them
                if (isWindows_) {
                    // Skip all consecutive separators
                    while (i < pattern.length() && isPathSeparator(pattern[i])) {
                        i++;
                    }
                    tokens_.emplace_back(TokenType::PATH_SEP, "/");
                } else {
                    tokens_.emplace_back(TokenType::PATH_SEP, std::string(1, c));
                    i++;
                }
            } else {
                // Literal backslash on POSIX
                tokens_.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
            break;

        case '@':
        case '!':
        case '+': {
            // Check for extglob patterns: @(...), !(...), ?(...), +(...), *(...)
            if (i + 1 < pattern.length() && pattern[i + 1] == '(') {
                Token token(TokenType::EXTGLOB);
                if (parseExtglob(pattern, i, token)) {
                    tokens_.push_back(std::move(token));
                } else {
                    // Treat as literal if parsing fails
                    tokens_.emplace_back(TokenType::LITERAL, std::string(1, c));
                    i++;
                }
            } else {
                // Treat as literal
                tokens_.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
        } break;

        case '{': {
            // Check for brace expansion patterns: {a,b,c}, {1..3}, {01..03}
            Token token(TokenType::BRACE);
            if (parseBraceExpansion(pattern, i, token)) {
                tokens_.push_back(std::move(token));
            } else {
                // Treat as literal if parsing fails
                tokens_.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
        } break;

        default: {
            // Check for ?(... and *(... patterns
            if ((c == '?' || c == '*') && i + 1 < pattern.length() && pattern[i + 1] == '(') {
                Token token(TokenType::EXTGLOB);
                if (parseExtglob(pattern, i, token)) {
                    tokens_.push_back(std::move(token));
                    break;
                }
            }

            // Collect literal characters
            std::string literal;
            while (i < pattern.length()) {
                char ch = pattern[i];
                if (ch == '*' || ch == '?' || ch == '[' || ch == '{' || isPathSeparator(ch) || ch == '@' || ch == '!' || ch == '+') {
                    break;
                }
                // Check for potential extglob patterns
                if ((ch == '?' || ch == '*') && i + 1 < pattern.length() && pattern[i + 1] == '(') {
                    break;
                }
                literal += ch;
                i++;
            }
            if (!literal.empty()) {
                tokens_.emplace_back(TokenType::LITERAL, literal);
            }
        } break;
        }
    }
}

bool MinimatchPattern::parseCharClass(const std::string& pattern, size_t& pos, Token& token)
{
    if (pos >= pattern.length() || pattern[pos] != '[') {
        return false;
    }

    size_t start = pos + 1;
    bool negated = false;

    // Check for negation
    if (start < pattern.length() && (pattern[start] == '^' || pattern[start] == '!')) {
        negated = true;
        start++;
    }

    // Find the closing bracket
    size_t end = start;
    bool foundClose = false;

    // Handle special case where ] is first character
    if (end < pattern.length() && pattern[end] == ']') {
        end++;
    }

    while (end < pattern.length()) {
        if (pattern[end] == ']') {
            foundClose = true;
            break;
        }
        end++;
    }

    if (!foundClose || end <= start) {
        return false; // Invalid character class
    }

    token.value = pattern.substr(start, end - start);
    token.negated = negated;
    pos = end + 1; // Move past the closing ]

    return true;
}

bool MinimatchPattern::parseExtglob(const std::string& pattern, size_t& pos, Token& token)
{
    if (pos >= pattern.length()) {
        return false;
    }

    char extType = pattern[pos];

    // Check for valid extglob patterns: @(...), !(...), ?(...), +(...), *(...)
    if (pos + 1 >= pattern.length() || pattern[pos + 1] != '(') {
        return false;
    }

    // Set the extglob type in token value
    token.value = std::string(1, extType);

    size_t start = pos + 2; // Skip past the opening parenthesis
    size_t current = start;
    int parenthesesDepth = 1;

    // Find the matching closing parenthesis, handling nested parentheses
    while (current < pattern.length() && parenthesesDepth > 0) {
        if (pattern[current] == '(') {
            parenthesesDepth++;
        } else if (pattern[current] == ')') {
            parenthesesDepth--;
        }
        current++;
    }

    if (parenthesesDepth != 0) {
        return false; // Unmatched parentheses
    }

    // Extract the content inside parentheses (excluding the closing ')')
    std::string content = pattern.substr(start, current - start - 1);

    // Parse alternatives separated by |
    std::vector<std::string> alternatives;
    std::string currentAlt;
    int nestedDepth = 0;

    for (size_t i = 0; i < content.length(); i++) {
        char c = content[i];
        if (c == '(') {
            nestedDepth++;
            currentAlt += c;
        } else if (c == ')') {
            nestedDepth--;
            currentAlt += c;
        } else if (c == '|' && nestedDepth == 0) {
            // Found a top-level alternative separator
            alternatives.push_back(currentAlt);
            currentAlt.clear();
        } else {
            currentAlt += c;
        }
    }

    // Add the last alternative
    alternatives.push_back(currentAlt); // Create child tokens for each alternative
    for (const auto& alt : alternatives) {
        if (alt.empty()) {
            // Empty alternative - create an empty token list
            std::vector<Token> emptyAlt;
            token.alternatives.push_back(emptyAlt);
        } else {
            // Parse the alternative as a complete sub-pattern
            std::vector<Token> altTokens = parseAlternativePattern(alt);
            token.alternatives.push_back(altTokens);
        }
    }
    pos = current; // Move past the closing parenthesis

    return true;
}

std::vector<Token> MinimatchPattern::parseAlternativePattern(const std::string& pattern)
{
    std::vector<Token> tokens;

    if (pattern.empty()) {
        tokens.emplace_back(TokenType::LITERAL, "");
        return tokens;
    }

    // Parse the pattern using the same tokenizing logic
    for (size_t i = 0; i < pattern.length();) {
        char c = pattern[i];

        switch (c) {
        case '*':
            tokens.emplace_back(TokenType::STAR);
            i++;
            break;

        case '?':
            tokens.emplace_back(TokenType::QUESTION);
            i++;
            break;

        case '[': {
            Token token(TokenType::CHAR_CLASS);
            if (parseCharClass(pattern, i, token)) {
                tokens.push_back(std::move(token));
            } else {
                // Treat as literal if parsing fails
                tokens.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
        } break;

        case '@':
        case '!':
        case '+': {
            // Check for nested extglob patterns
            if (i + 1 < pattern.length() && pattern[i + 1] == '(') {
                Token token(TokenType::EXTGLOB);
                if (parseExtglob(pattern, i, token)) {
                    tokens.push_back(std::move(token));
                } else {
                    // Treat as literal if parsing fails
                    tokens.emplace_back(TokenType::LITERAL, std::string(1, c));
                    i++;
                }
            } else {
                // Treat as literal
                tokens.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
        } break;

        case '{': {
            // Check for nested brace expansion patterns
            Token token(TokenType::BRACE);
            if (parseBraceExpansion(pattern, i, token)) {
                tokens.push_back(std::move(token));
            } else {
                // Treat as literal if parsing fails
                tokens.emplace_back(TokenType::LITERAL, std::string(1, c));
                i++;
            }
        } break;

        default: {
            // Check for ?(... and *(... patterns (nested extglobs)
            if ((c == '?' || c == '*') && i + 1 < pattern.length() && pattern[i + 1] == '(') {
                Token token(TokenType::EXTGLOB);
                if (parseExtglob(pattern, i, token)) {
                    tokens.push_back(std::move(token));
                    break;
                }
            }

            // Collect literal characters
            std::string literal;
            while (i < pattern.length()) {
                char ch = pattern[i];
                if (ch == '*' || ch == '?' || ch == '[' || ch == '{' || ch == '@' || ch == '!' || ch == '+') {
                    break;
                }
                // Check for potential extglob patterns
                if ((ch == '?' || ch == '*') && i + 1 < pattern.length() && pattern[i + 1] == '(') {
                    break;
                }
                literal += ch;
                i++;
            }
            if (!literal.empty()) {
                tokens.emplace_back(TokenType::LITERAL, literal);
            }
        } break;
        }
    }

    return tokens;
}

bool MinimatchPattern::parseBraceExpansion(const std::string& pattern, size_t& pos, Token& token)
{
    if (pos >= pattern.length() || pattern[pos] != '{') {
        return false;
    }

    size_t start = pos + 1; // Skip opening brace
    size_t current = start;
    int braceDepth = 1;

    // Find the matching closing brace, handling nested braces
    while (current < pattern.length() && braceDepth > 0) {
        if (pattern[current] == '{') {
            braceDepth++;
        } else if (pattern[current] == '}') {
            braceDepth--;
        }
        current++;
    }

    if (braceDepth != 0) {
        return false; // Unmatched braces
    }

    // Extract the content inside braces (excluding the closing '}')
    std::string content = pattern.substr(start, current - start - 1);

    // Check for numeric sequence patterns like {1..3} or {01..03}
    size_t dotPos = content.find("..");
    if (dotPos != std::string::npos) {
        std::string startStr = content.substr(0, dotPos);
        std::string endStr = content.substr(dotPos + 2);

        // Try to parse as numeric sequence
        bool isNumeric = true;
        for (char c : startStr) {
            if (!std::isdigit(c)) {
                isNumeric = false;
                break;
            }
        }
        for (char c : endStr) {
            if (!std::isdigit(c)) {
                isNumeric = false;
                break;
            }
        }

        if (isNumeric && !startStr.empty() && !endStr.empty()) {
            int startNum = std::stoi(startStr);
            int endNum = std::stoi(endStr);
            bool zeroPadded = (startStr.length() > 1 && startStr[0] == '0') || (endStr.length() > 1 && endStr[0] == '0');
            size_t padWidth = std::max(startStr.length(), endStr.length());

            // Generate numeric sequence (support both forward and reverse ranges)
            int step = (startNum <= endNum) ? 1 : -1;
            for (int i = startNum; (step > 0) ? (i <= endNum) : (i >= endNum); i += step) {
                std::string numStr;
                if (zeroPadded) {
                    numStr = std::to_string(i);
                    while (numStr.length() < padWidth) {
                        numStr = "0" + numStr;
                    }
                } else {
                    numStr = std::to_string(i);
                }

                std::vector<Token> altTokens;
                altTokens.emplace_back(TokenType::LITERAL, numStr);
                token.alternatives.push_back(altTokens);
            }

            pos = current; // Move past the closing brace
            return true;
        }

        // Try to parse as alphabetic sequence like {a..c}
        if (startStr.length() == 1 && endStr.length() == 1 && std::isalpha(startStr[0]) && std::isalpha(endStr[0])) {
            char startChar = startStr[0];
            char endChar = endStr[0];

            // Generate alphabetic sequence (support both forward and reverse ranges)
            int step = (startChar <= endChar) ? 1 : -1;
            for (char c = startChar; (step > 0) ? (c <= endChar) : (c >= endChar); c += step) {
                std::vector<Token> altTokens;
                altTokens.emplace_back(TokenType::LITERAL, std::string(1, c));
                token.alternatives.push_back(altTokens);
            }

            pos = current; // Move past the closing brace
            return true;
        }
    }

    // Parse as comma-separated alternatives like {a,b,c}
    std::vector<std::string> alternatives;
    std::string currentAlt;
    int nestedDepth = 0;

    for (size_t i = 0; i < content.length(); i++) {
        char c = content[i];
        if (c == '{') {
            nestedDepth++;
            currentAlt += c;
        } else if (c == '}') {
            nestedDepth--;
            currentAlt += c;
        } else if (c == ',' && nestedDepth == 0) {
            // Found a top-level alternative separator
            alternatives.push_back(currentAlt);
            currentAlt.clear();
        } else {
            currentAlt += c;
        }
    }

    // Add the last alternative
    alternatives.push_back(currentAlt);

    // If there's only one alternative, treat as literal (no expansion)
    if (alternatives.size() <= 1) {
        return false;
    }

    // Create tokens for each alternative
    for (const auto& alt : alternatives) {
        std::vector<Token> altTokens;
        if (alt.empty()) {
            // Empty alternative
            altTokens.emplace_back(TokenType::LITERAL, "");
        } else {
            // Parse the alternative recursively to handle nested braces
            altTokens = parseAlternativePattern(alt);
        }
        token.alternatives.push_back(altTokens);
    }

    pos = current; // Move past the closing brace
    return true;
}

}
