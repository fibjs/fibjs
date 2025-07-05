/*
 * path_match_utils.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"
#include <algorithm>
#include <cctype>

namespace fibjs {

// Utility functions implementation

bool MinimatchPattern::isCharInClass(char c, const std::string& charClass, bool negated) const
{
    if (charClass.empty()) {
        return false;
    }

    // Determine if character class matching should be case insensitive
    // On macOS and Windows, character ranges are typically case insensitive
    bool charClassCaseInsensitive = false;
#ifdef __APPLE__
    charClassCaseInsensitive = true;
#else
    charClassCaseInsensitive = isWindows_;
#endif

    for (size_t i = 0; i < charClass.length();) {
        if (i + 2 < charClass.length() && charClass[i + 1] == '-') {
            // Range like a-z
            char start = charClass[i];
            char end = charClass[i + 2];

            if (charClassCaseInsensitive) {
                char lc = std::tolower(c);
                char lstart = std::tolower(start);
                char lend = std::tolower(end);
                if (lc >= lstart && lc <= lend) {
                    return true;
                }
            } else {
                if (c >= start && c <= end) {
                    return true;
                }
            }

            i += 3;
        } else {
            // Single character
            if (charClassCaseInsensitive) {
                if (std::tolower(c) == std::tolower(charClass[i])) {
                    return true;
                }
            } else {
                if (c == charClass[i]) {
                    return true;
                }
            }
            i++;
        }
    }

    return false;
}

bool MinimatchPattern::isPathSeparator(char c) const
{
    // Always treat both / and \ as path separators to match Node.js behavior
    // Node.js path.matchesGlob treats \ as path separator in patterns on all platforms
    return c == '\\' || c == '/';
}

bool MinimatchPattern::isPathSeparatorInText(char c) const
{
    // In text matching, only treat / as path separator on POSIX
    // On Windows, treat both / and \ as path separators
    if (c == '/') {
        return true;
    }
    if (c == '\\') {
        return isWindows_;
    }
    return false;
}

bool MinimatchPattern::isSpecialChar(char c) const
{
    // Characters that have special meaning in glob patterns
    return c == '*' || c == '?' || c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}' || c == '^' || c == '!' || c == '/';
}

char MinimatchPattern::getPathSeparator() const
{
    return isWindows_ ? '\\' : '/';
}

bool MinimatchPattern::shouldMatchCase(char a, char b) const
{
    // Direct character matching should always be case sensitive
    // Only drive letters on Windows are case insensitive, but that's handled separately
    return a == b;
}

bool MinimatchPattern::shouldMatchCaseDriveLetter(char a, char b) const
{
    // Drive letters on Windows are case insensitive
    if (isWindows_) {
        return std::tolower(a) == std::tolower(b);
    }
    return a == b;
}

}
