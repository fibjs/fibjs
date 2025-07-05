/*
 * path_match.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/path_win32.h"
#include "ifs/path_posix.h"
#include "path_match.h"

namespace fibjs {

// MinimatchPattern Implementation

MinimatchPattern::MinimatchPattern(const std::string& pattern, bool isWindows)
    : isWindows_(isWindows)
    , original_pattern_(pattern)
{
    if (pattern.empty()) {
        compiled_ = true;
        return;
    }

    tokenize(pattern);
    compiled_ = true;
}

// Convenience function
bool matchesGlob(std::string_view text, const std::string& pattern, bool isWindows)
{
    MinimatchPattern matcher(pattern, isWindows);
    return matcher.match(text);
}

// Update the existing functions to use the new implementation
static bool matchPattern(const char* str, const char* pattern, bool isWindows)
{
    return matchesGlob(str, pattern, isWindows);
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

}
