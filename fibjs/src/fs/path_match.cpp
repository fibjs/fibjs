#include "object.h"
#include "ifs/path.h"
#include "ifs/path_win32.h"
#include "ifs/path_posix.h"
#include "Runtime.h"
#include <string>
#include <vector>

namespace fibjs {

static bool isInCharClass(char ch, const std::string& pattern, size_t& i)
{
    bool negate = false;
    bool match = false;
    bool firstChar = true;

    // 跳过开头的 '['
    i++;

    // 检查是否是否定字符类
    if (i < pattern.length() && (pattern[i] == '!' || pattern[i] == '^')) {
        negate = true;
        i++;
    }

    // 处理连字符在开头或结尾的特殊情况
    if (i < pattern.length() && pattern[i] == '-') {
        if (ch == '-')
            match = true;
        firstChar = false;
    }

    for (; i < pattern.length() && pattern[i] != ']'; i++) {
        // 处理转义字符
        if (pattern[i] == '\\' && i + 1 < pattern.length()) {
            i++;
            if (pattern[i] == ch) {
                match = true;
            }
            continue;
        }

        // 处理连字符范围
        if (pattern[i] == '-' && i + 1 < pattern.length() && pattern[i + 1] != ']') {
            char rangeStart = firstChar ? pattern[i - 1] : pattern[i - 2];
            char rangeEnd = pattern[i + 1];
            if (ch >= rangeStart && ch <= rangeEnd) {
                match = true;
            }
            firstChar = false;
            i++;
        }
        // 普通字符匹配
        else if (pattern[i] == ch) {
            match = true;
        }
    }

    return negate ? !match : match;
}

static bool isPathSeparator(char ch, bool isWindows)
{
    return isWindows ? (ch == '\\' || ch == '/') : (ch == '/');
}

static bool matchPattern(const char* str, const char* pattern, bool isWindows)
{
    if (!str || !pattern)
        return false;

    while (*str || *pattern) {
        if (*pattern == '*') {
            if (*(pattern + 1) == '*') {
                // Handle ** (match zero or more directories)
                pattern += 2;
                // Skip any following path separators
                while (isPathSeparator(*pattern, isWindows))
                    pattern++;

                if (!*pattern)
                    return true;

                // Try to match at current position
                if (matchPattern(str, pattern, isWindows))
                    return true;

                // Try to match after skipping a path segment
                while (*str) {
                    if (isPathSeparator(*str, isWindows)) {
                        str++;
                        if (matchPattern(str, pattern, isWindows))
                            return true;
                    } else {
                        str++;
                    }
                }
                return false;
            } else {
                // Handle * (match zero or more characters within segment)
                pattern++;
                const char* star_str = str;
                while (*star_str && !isPathSeparator(*star_str, isWindows)) {
                    if (matchPattern(star_str, pattern, isWindows))
                        return true;
                    star_str++;
                }
                if (matchPattern(star_str, pattern, isWindows))
                    return true;
                return false;
            }
        } else if (*pattern == '[') {
            if (!*str)
                return false;
            std::string pat(pattern);
            size_t i = 0;
            if (!isInCharClass(*str, pat, i))
                return false;
            pattern += i + 1;
            str++;
        } else if (*pattern == '!' && pattern > pattern && isPathSeparator(*(pattern - 1), isWindows)) {
            // Handle negation (e.g., */!bar/*)
            pattern++;
            const char* excl_str = str;
            while (*excl_str && !isPathSeparator(*excl_str, isWindows))
                excl_str++;
            size_t len = excl_str - str;
            if (strncmp(str, pattern, len) == 0)
                return false;
            pattern += len;
            str = excl_str;
        } else if (*pattern == *str || *pattern == '?') {
            if (!*str)
                return false;
            pattern++;
            str++;
        } else if (isPathSeparator(*pattern, isWindows) && !*str && (*(pattern + 1) == '*' || !*(pattern + 1))) {
            pattern++;
        } else if (isWindows && isPathSeparator(*pattern, true) && isPathSeparator(*str, true)) {
            // For Windows, treat both / and \ as path separators
            pattern++;
            str++;
        } else {
            return false;
        }
    }

    return !*str && !*pattern;
}

result_t path_win32_base::matchesGlob(exlib::string path, exlib::string pattern, bool& retVal)
{
    if (pattern.empty()) {
        retVal = path.empty();
        return 0;
    }

    retVal = matchPattern(path.c_str(), pattern.c_str(), true);
    return 0;
}

result_t path_posix_base::matchesGlob(exlib::string path, exlib::string pattern, bool& retVal)
{
    if (pattern.empty()) {
        retVal = path.empty();
        return 0;
    }

    retVal = matchPattern(path.c_str(), pattern.c_str(), false);
    return 0;
}

} // namespace fibjs
