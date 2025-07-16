/*
 * fs_glob.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#ifndef _WIN32
#define _FILE_OFFSET_BITS 64
#endif

#include "ifs/fs.h"
#include "path.h"
#include "AsyncUV.h"
#include "utils.h"
#include "Stat.h"
#include <set>
#include <algorithm>
#include <optional>
#include <vector>
#include <boost/preprocessor.hpp>
#include "match/path_match.h"

namespace fibjs {

#ifdef _WIN32
const bool isWindows = true;
#else
const bool isWindows = false;
#endif

// Structure to hold path and stat information
struct GlobResult {
    exlib::string path;
    obj_ptr<Stat> stat;

    GlobResult(const exlib::string& p)
        : path(p)
    {
    }
    GlobResult(const exlib::string& p, obj_ptr<Stat> s)
        : path(p)
        , stat(s)
    {
    }

    bool operator<(const GlobResult& other) const
    {
        return path < other.path;
    }
};

// Helper function to normalize path separators for the current platform
static exlib::string normalizePath(const exlib::string& path)
{
#ifdef _WIN32
    exlib::string result = path;
    // Replace all forward slashes with backslashes on Windows
    for (size_t i = 0; i < result.length(); ++i) {
        if (result.c_str()[i] == '/') {
            result.data()[i] = '\\';
        }
    }
    return result;
#else
    return path; // On Unix, forward slash is the standard
#endif
}

// Helper function to check if a path should be ignored
static bool shouldIgnore(const exlib::string& path, const std::vector<exlib::string>& excludePatterns)
{
    for (const auto& pattern : excludePatterns) {
        if (matchesGlob(path, pattern, isWindows)) {
            return true;
        }
    }
    return false;
}

// Helper function to check if a pattern contains **
static bool hasRecursivePattern(const exlib::string& pattern)
{
    return pattern.find("**") != exlib::string::npos;
}

// Helper function to check if a pattern starts with ./
static bool isRelativePattern(const exlib::string& pattern)
{
#ifdef _WIN32
    return pattern.substr(0, 2) == "./" || pattern.substr(0, 2) == ".\\";
#else
    return pattern.substr(0, 2) == "./";
#endif
}

// Helper function to check if a pattern is absolute
static bool isAbsolutePattern(const exlib::string& pattern)
{
    if (pattern.length() == 0)
        return false;

#ifdef _WIN32
    // Windows: check for drive letter (C:\ or C:/) or UNC path (\\server\share)
    if (pattern.length() >= 3 && pattern.c_str()[1] == ':' && isPathSlash(pattern.c_str()[2])) {
        return true; // Drive letter format like C:\ or C:/
    }
    if (pattern.length() >= 2 && pattern.c_str()[0] == '\\' && pattern.c_str()[1] == '\\') {
        return true; // UNC path like \\server\share
    }
    return false;
#else
    return pattern.c_str()[0] == PATH_SLASH;
#endif
}

// Helper function to normalize pattern
static exlib::string normalizePattern(const exlib::string& pattern)
{
    if (isRelativePattern(pattern)) {
        return pattern.substr(2); // Remove "./"
    }
    return pattern;
}

// Helper function to check if path contains directories
static bool hasDirectory(const exlib::string& pattern)
{
#ifdef _WIN32
    return pattern.find('/') != exlib::string::npos || pattern.find('\\') != exlib::string::npos;
#else
    return pattern.find('/') != exlib::string::npos;
#endif
}

// Helper function to get directory part of a pattern
static exlib::string getDirectoryPart(const exlib::string& pattern)
{
#ifdef _WIN32
    size_t lastSlash = pattern.find_last_of("/\\");
#else
    size_t lastSlash = pattern.find_last_of('/');
#endif
    if (lastSlash != exlib::string::npos) {
        return pattern.substr(0, lastSlash);
    }
    return "";
}

// Helper function to get filename part of a pattern
static exlib::string getFilenamePart(const exlib::string& pattern)
{
#ifdef _WIN32
    size_t lastSlash = pattern.find_last_of("/\\");
#else
    size_t lastSlash = pattern.find_last_of('/');
#endif
    if (lastSlash != exlib::string::npos) {
        return pattern.substr(lastSlash + 1);
    }
    return pattern;
}

// Helper function to split pattern into components
static std::vector<exlib::string> splitPattern(const exlib::string& pattern)
{
    std::vector<exlib::string> components;
    if (pattern.empty()) {
        return components;
    }

    size_t start = 0;
    size_t pos = 0;

    while (pos < pattern.length()) {
#ifdef _WIN32
        if (pattern.c_str()[pos] == '/' || pattern.c_str()[pos] == '\\') {
#else
        if (pattern.c_str()[pos] == '/') {
#endif
            if (pos > start) {
                components.push_back(pattern.substr(start, pos - start));
            }
            start = pos + 1;
        }
        pos++;
    }

    // Add the last component
    if (start < pattern.length()) {
        components.push_back(pattern.substr(start));
    } else if (start == pattern.length() && pattern.length() > 0) {
        // Pattern ends with '/', add empty component to preserve the trailing slash meaning
#ifdef _WIN32
        if (pattern.c_str()[pattern.length() - 1] == '/' || pattern.c_str()[pattern.length() - 1] == '\\') {
#else
        if (pattern.c_str()[pattern.length() - 1] == '/') {
#endif
            components.push_back("");
        }
    }

    return components;
}

// Helper function to find the first wildcard component index
static int findFirstWildcardComponent(const std::vector<exlib::string>& components)
{
    for (size_t i = 0; i < components.size(); ++i) {
        const exlib::string& component = components[i];

        // Use the unified wildcard checking function from path_match
        if (containsWildcards(component.c_str())) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Helper function to join path components
static exlib::string joinComponents(const std::vector<exlib::string>& components, int start, int end)
{
    if (start >= end || start >= static_cast<int>(components.size())) {
        return "";
    }

    exlib::string result = components[start];
    for (int i = start + 1; i < end && i < static_cast<int>(components.size()); ++i) {
        if (!components[i].empty()) {
            result += PATH_SLASH + components[i];
        } else if (i == end - 1) {
            // Empty component at the end means the pattern should end with '/'
            result += PATH_SLASH;
        }
    }
    return result;
}

// Helper function to create Stat object for a file
static obj_ptr<Stat> createStat(const exlib::string& fullPath)
{
    AutoReq req;
    int32_t ret = uv_fs_stat(NULL, &req, fullPath.c_str(), NULL);
    if (ret < 0)
        return NULL;

    obj_ptr<Stat> stat = new Stat();
    stat->fill(fullPath, &req.statbuf);
    return stat;
}

// Simplified recursive function to walk directory tree and collect matching files
static void walkDirectorySimple(
    const exlib::string& basePath,
    const exlib::string& currentPath,
    const exlib::string& pattern,
    const std::vector<exlib::string>& excludePatterns,
    std::set<GlobResult>& results,
    bool withFileTypes = false,
    int depth = 0,
    int maxDepth = 100)
{
    if (depth > maxDepth)
        return;

    // Special handling for patterns ending with "**"
    // When pattern ends with **, include directories that match the prefix
    if (pattern.length() >= 2 && pattern.substr(pattern.length() - 2) == "**") {
        // Check if we need to match a prefix pattern
#ifdef _WIN32
        if (pattern.length() > 2 && (pattern.c_str()[pattern.length() - 3] == '/' || pattern.c_str()[pattern.length() - 3] == '\\')) {
#else
        if (pattern.length() > 2 && pattern.c_str()[pattern.length() - 3] == '/') {
#endif
            // Pattern like "src/**" - extract prefix "src"
            exlib::string prefix = pattern.substr(0, pattern.length() - 3);

            // If current path matches prefix, include it in results
            if (currentPath == prefix || (currentPath.empty() && prefix.empty()) || matchesGlob(currentPath, prefix, isWindows)) {

                if (!shouldIgnore(currentPath.empty() ? "." : currentPath, excludePatterns)) {
                    exlib::string resultPath = currentPath.empty() ? "." : currentPath;
                    if (withFileTypes) {
                        exlib::string fullPath = basePath;
                        if (!currentPath.empty()) {
                            fullPath += PATH_SLASH + currentPath;
                        }
                        obj_ptr<Stat> stat = createStat(fullPath);
                        if (stat) {
                            results.insert(GlobResult(resultPath, stat));
                        } else {
                            results.insert(GlobResult(resultPath));
                        }
                    } else {
                        results.insert(GlobResult(resultPath));
                    }
                }
            }
        }
    }

    exlib::string fullPath = basePath;
    if (!currentPath.empty()) {
        fullPath += PATH_SLASH + currentPath;
    }

    AutoReq req;
    int32_t ret = uv_fs_scandir(NULL, &req, fullPath.c_str(), 0, NULL);
    if (ret < 0)
        return; // Directory doesn't exist or can't be read

    uv_dirent_t dirent;
    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
        exlib::string entryName = dirent.name;
        exlib::string relativePath = currentPath.empty() ? entryName : normalizePath(currentPath + PATH_SLASH + entryName);

        // Skip if ignored
        if (shouldIgnore(relativePath, excludePatterns)) {
            continue;
        }

        // Check if this path matches the pattern
        bool isMatch = false;

        // Special handling for patterns ending with '/' - only match directories
#ifdef _WIN32
        if (pattern.length() > 0 && (pattern.c_str()[pattern.length() - 1] == '/' || pattern.c_str()[pattern.length() - 1] == '\\')) {
#else
        if (pattern.length() > 0 && pattern.c_str()[pattern.length() - 1] == '/') {
#endif
            if (dirent.type == UV_DIRENT_DIR) {
                exlib::string dirPattern = pattern.substr(0, pattern.length() - 1);
                isMatch = matchesGlob(relativePath, dirPattern, isWindows);
            }
        } else {
            // Normal pattern matching
            isMatch = matchesGlob(relativePath, pattern, isWindows);
        }

        if (isMatch) {
            if (withFileTypes) {
                exlib::string entryFullPath = basePath;
                if (!currentPath.empty()) {
                    entryFullPath += PATH_SLASH + currentPath;
                }
                entryFullPath += PATH_SLASH + entryName;

                obj_ptr<Stat> stat = createStat(entryFullPath);
                if (stat) {
                    results.insert(GlobResult(relativePath, stat));
                } else {
                    results.insert(GlobResult(relativePath));
                }
            } else {
                results.insert(GlobResult(relativePath));
            }
        }

        // If it's a directory, continue walking
        if (dirent.type == UV_DIRENT_DIR) {
            walkDirectorySimple(basePath, relativePath, pattern, excludePatterns, results, withFileTypes, depth + 1, maxDepth);
        }
    }
}

class GlobOptions : public obj_base {
public:
    LOAD_OPTIONS(GlobOptions, (cwd)(exclude)(withFileTypes))

public:
    std::optional<exlib::string> cwd;
    std::optional<std::vector<exlib::string>> exclude;
    std::optional<bool> withFileTypes;
};

result_t fs_base::glob(std::vector<exlib::string>& patterns, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(opts);
        ac->m_ctx.resize(1);

        obj_ptr<GlobOptions> _opts;
        result_t hr = GlobOptions::load(isolate, opts, _opts);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<GlobOptions> _opts = (GlobOptions*)ac->m_ctx[0].object();

    // Get working directory from options or use current directory
    exlib::string cwd;
    if (_opts->cwd.has_value()) {
        cwd = _opts->cwd.value();
    } else {
        process_base::cwd(cwd);
    }
    os_normalize(cwd, cwd, true);

    std::set<GlobResult> results;
    retVal = new NArray();

    // Get exclude patterns from options
    std::vector<exlib::string> excludePatterns;
    if (_opts->exclude.has_value()) {
        excludePatterns = _opts->exclude.value();
    }

    // Check if withFileTypes is enabled
    bool withFileTypes = false;
    if (_opts->withFileTypes.has_value()) {
        withFileTypes = _opts->withFileTypes.value();
    }

    for (const auto& pattern : patterns) {
        if (pattern.empty()) {
            continue;
        }

        exlib::string normalizedPattern = normalizePattern(pattern);

        // Handle absolute patterns
        if (isAbsolutePattern(pattern)) {
            // Split the absolute pattern into components
            std::vector<exlib::string> components = splitPattern(normalizedPattern);
            if (components.empty()) {
                continue;
            }

            // Find the first component with wildcards
            int wildcardIndex = findFirstWildcardComponent(components);

            if (wildcardIndex == -1) {
                // No wildcards, check if file exists directly
                AutoReq req;
                int32_t ret = uv_fs_stat(NULL, &req, normalizedPattern.c_str(), NULL);
                if (ret >= 0) {
                    if (withFileTypes) {
                        obj_ptr<Stat> stat = createStat(normalizedPattern);
                        if (stat) {
                            results.insert(GlobResult(normalizedPattern, stat));
                        } else {
                            results.insert(GlobResult(normalizedPattern));
                        }
                    } else {
                        results.insert(GlobResult(normalizedPattern));
                    }
                }
            } else {
                // Has wildcards, extract base path and relative pattern
                exlib::string basePath;
                if (wildcardIndex == 0) {
                    // First component has wildcard, use root as base
                    basePath = "/";
                } else {
                    // Build base path from components before wildcard
                    basePath = "/" + joinComponents(components, 0, wildcardIndex);
                }

                // Build relative pattern from wildcard component onwards
                exlib::string relativePattern = joinComponents(components, wildcardIndex, components.size());

                // Use temporary results set for this pattern
                std::set<GlobResult> tempResults;
                walkDirectorySimple(basePath, "", relativePattern, excludePatterns, tempResults, withFileTypes);

                // Convert relative results to absolute
                for (const auto& result : tempResults) {
                    exlib::string absolutePath;
                    if (basePath == "/") {
                        absolutePath = "/" + result.path;
                    } else {
                        absolutePath = basePath + "/" + result.path;
                    }
                    if (result.stat) {
                        results.insert(GlobResult(absolutePath, result.stat));
                    } else {
                        results.insert(GlobResult(absolutePath));
                    }
                }
            }
            continue;
        }

        // Handle relative patterns
        walkDirectorySimple(cwd, "", normalizedPattern, excludePatterns, results, withFileTypes);
    }

    // Convert set to sorted array
    for (const auto& result : results) {
        if (withFileTypes && result.stat) {
            retVal->append((object_base*)result.stat);
        } else {
            retVal->append(result.path);
        }
    }

    return 0;
}

result_t fs_base::glob(exlib::string pattern, v8::Local<v8::Object> opts, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        std::vector<exlib::string> patterns;
        patterns.push_back(pattern);
        return glob(patterns, opts, retVal, ac);
    }

    std::vector<exlib::string> patterns;
    patterns.push_back(pattern);

    return glob(patterns, opts, retVal, ac);
}

}
