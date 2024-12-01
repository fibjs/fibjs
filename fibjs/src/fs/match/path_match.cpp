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

static bool matchPattern(const char* str, const char* pattern, bool isWindows)
{
    if (!str || !pattern)
        return false;

    GlobParser parser(isWindows);
    GlobNode ast = parser.parse(pattern);
    return RegexMatcher::match(ast, str, isWindows);
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
