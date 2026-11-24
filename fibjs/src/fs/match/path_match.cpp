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
#include "LruCache.h"
#include <memory>

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

// Compiled patterns are immutable, so the most recently used ones are kept in
// an LRU: path.matchesGlob() is commonly called in a loop with the same
// pattern, and tokenizing it on every call costs more than the match itself.
// Callers that match many paths against one pattern should still compile it
// once (MinimatchPattern is cheap to reuse); the cache mainly serves the
// per-call API and short-lived patterns.
static LruCache<std::shared_ptr<MinimatchPattern>>& pattern_cache()
{
    static LruCache<std::shared_ptr<MinimatchPattern>> s_cache(256, 0, 0);
    return s_cache;
}

// Convenience function
bool matchesGlob(std::string_view text, const std::string& pattern, bool isWindows)
{
    exlib::string key(pattern.data(), pattern.size());
    // The platform flag is part of the key: the same text tokenizes and
    // compares differently for POSIX and Windows separators/case rules.
    key.push_back(isWindows ? '\x01' : '\x00');

    std::shared_ptr<MinimatchPattern> matcher;
    LruCache<std::shared_ptr<MinimatchPattern>>::Resolver resolver =
        [](exlib::string& cached_key, std::shared_ptr<MinimatchPattern>& value) {
            value = std::make_shared<MinimatchPattern>(
                std::string(cached_key.data(), cached_key.size() - 1),
                cached_key[cached_key.size() - 1] == '\x01');
            return true;
        };

    if (!pattern_cache().lookup(key, matcher, resolver))
        return false;

    return matcher->match(text);
}

// Static utility method implementation
bool MinimatchPattern::hasWildcards(const std::string& pattern)
{
    if (pattern.empty()) {
        return false;
    }

    for (char c : pattern) {
        // Exclude path separators since they're not wildcards in this context
        if (c != '/' && c != '\\' && isSpecialChar(c)) {
            return true;
        }
    }

    return false;
}

// Utility function to check if a pattern contains any wildcard characters
bool containsWildcards(const std::string& pattern)
{
    // Delegate to the MinimatchPattern static method
    return MinimatchPattern::hasWildcards(pattern);
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
        return result_t();
    }

    retVal = matchPattern(path.c_str(), pattern.c_str(), true);
    return result_t();
}

result_t path_posix_base::matchesGlob(exlib::string path, exlib::string pattern, bool& retVal)
{
    if (pattern.empty()) {
        retVal = path.empty();
        return result_t();
    }

    retVal = matchPattern(path.c_str(), pattern.c_str(), false);
    return result_t();
}

}
