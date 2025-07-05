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

// Helper function to check if a path should be ignored
static bool shouldIgnore(const exlib::string& path, const std::vector<exlib::string>& excludePatterns)
{
    for (const auto& pattern : excludePatterns) {
        if (matchesGlob(path, pattern)) {
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
    return pattern.substr(0, 2) == "./";
}

// Helper function to check if a pattern is absolute
static bool isAbsolutePattern(const exlib::string& pattern)
{
    return pattern.length() > 0 && pattern.c_str()[0] == '/';
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
    return pattern.find('/') != exlib::string::npos;
}

// Helper function to get directory part of a pattern
static exlib::string getDirectoryPart(const exlib::string& pattern)
{
    size_t lastSlash = pattern.find_last_of('/');
    if (lastSlash != exlib::string::npos) {
        return pattern.substr(0, lastSlash);
    }
    return "";
}

// Helper function to get filename part of a pattern
static exlib::string getFilenamePart(const exlib::string& pattern)
{
    size_t lastSlash = pattern.find_last_of('/');
    if (lastSlash != exlib::string::npos) {
        return pattern.substr(lastSlash + 1);
    }
    return pattern;
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

// Recursive function to walk directory tree and collect matching files
static void walkDirectory(
    const exlib::string& basePath,
    const exlib::string& currentPath,
    const exlib::string& pattern,
    const std::vector<exlib::string>& excludePatterns,
    std::set<GlobResult>& results,
    bool withFileTypes = false,
    bool recursive = false,
    int depth = 0,
    int maxDepth = 100)
{
    if (depth > maxDepth)
        return;

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
        exlib::string relativePath = currentPath.empty() ? entryName : (currentPath + PATH_SLASH + entryName);

        // Skip if ignored
        if (shouldIgnore(relativePath, excludePatterns)) {
            continue;
        }

        // For recursive patterns, use the full relative path
        // For non-recursive patterns, only use the entry name for matching
        exlib::string matchPath = recursive ? relativePath : entryName;

        // Special handling for **/.filename patterns - they should not match in root directory
        bool skipRootMatch = false;
        if (recursive && depth == 0 && pattern.substr(0, 3) == "**/" && pattern.find('*', 3) == exlib::string::npos) {
            // Only skip for patterns like "**/.env", not for "**/*.js"
            skipRootMatch = true;
        }

        // Check if this entry matches the pattern
        bool isMatch = false;
        if (!skipRootMatch) {
            if (pattern.length() > 0 && pattern.c_str()[pattern.length() - 1] == '/') {
                // Pattern ends with '/', only match directories
                if (dirent.type == UV_DIRENT_DIR) {
                    exlib::string dirPattern = pattern.substr(0, pattern.length() - 1);
                    isMatch = matchesGlob(entryName, dirPattern);
                }
            } else {
                // Normal pattern matching
                isMatch = matchesGlob(matchPath, pattern);
            }
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

        // If it's a directory and we're doing recursive search, continue walking
        if (dirent.type == UV_DIRENT_DIR && recursive) {
            walkDirectory(basePath, relativePath, pattern, excludePatterns, results, true, depth + 1, maxDepth);
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
            // For absolute patterns, we need to check if the file exists
            exlib::string dirPart = getDirectoryPart(pattern);
            exlib::string filePart = getFilenamePart(pattern);

            if (hasRecursivePattern(pattern)) {
                // Recursive absolute pattern
                walkDirectory(dirPart, "", filePart, excludePatterns, results, withFileTypes, true);
            } else {
                // Non-recursive absolute pattern
                AutoReq req;
                int32_t ret = uv_fs_scandir(NULL, &req, dirPart.c_str(), 0, NULL);
                if (ret >= 0) {
                    uv_dirent_t dirent;
                    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
                        if (matchesGlob(dirent.name, filePart)) {
                            exlib::string fullPath = dirPart + PATH_SLASH + dirent.name;
                            if (withFileTypes) {
                                obj_ptr<Stat> stat = createStat(fullPath);
                                if (stat) {
                                    results.insert(GlobResult(fullPath, stat));
                                } else {
                                    results.insert(GlobResult(fullPath));
                                }
                            } else {
                                results.insert(GlobResult(fullPath));
                            }
                        }
                    }
                }
            }
            continue;
        }

        // Handle patterns with directories (e.g., "src/*.js", "src/**/*.js")
        if (hasDirectory(normalizedPattern)) {
            // Special case: patterns like "*/" should be handled at root level
            if (normalizedPattern.length() > 0 && normalizedPattern.c_str()[normalizedPattern.length() - 1] == '/' && getDirectoryPart(normalizedPattern).find('/') == exlib::string::npos) {
                // Pattern like "*/" - match directories in current directory
                AutoReq req;
                int32_t ret = uv_fs_scandir(NULL, &req, cwd.c_str(), 0, NULL);
                if (ret >= 0) {
                    uv_dirent_t dirent;
                    exlib::string dirPattern = normalizedPattern.substr(0, normalizedPattern.length() - 1);
                    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
                        if (dirent.type == UV_DIRENT_DIR && matchesGlob(dirent.name, dirPattern)) {
                            if (!shouldIgnore(dirent.name, excludePatterns)) {
                                if (withFileTypes) {
                                    exlib::string fullPath = cwd + PATH_SLASH + dirent.name;
                                    obj_ptr<Stat> stat = createStat(fullPath);
                                    if (stat) {
                                        results.insert(GlobResult(dirent.name, stat));
                                    } else {
                                        results.insert(GlobResult(dirent.name));
                                    }
                                } else {
                                    results.insert(GlobResult(dirent.name));
                                }
                            }
                        }
                    }
                }
            } else if (hasRecursivePattern(normalizedPattern)) {
                // Recursive pattern: walk the entire directory tree
                walkDirectory(cwd, "", normalizedPattern, excludePatterns, results, withFileTypes, true);
            } else {
                // Non-recursive directory pattern: only look in specific directory
                exlib::string dirPart = getDirectoryPart(normalizedPattern);
                exlib::string filePart = getFilenamePart(normalizedPattern);
                walkDirectory(cwd, dirPart, filePart, excludePatterns, results, withFileTypes, false);
            }
        } else {
            // Simple pattern without directories
            if (hasRecursivePattern(normalizedPattern)) {
                // Recursive pattern without directory: search everywhere
                walkDirectory(cwd, "", normalizedPattern, excludePatterns, results, withFileTypes, true);
            } else {
                // Non-recursive pattern: only look in current directory
                AutoReq req;
                int32_t ret = uv_fs_scandir(NULL, &req, cwd.c_str(), 0, NULL);
                if (ret >= 0) {
                    uv_dirent_t dirent;
                    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
                        if (matchesGlob(dirent.name, normalizedPattern)) {
                            // Check if it should be ignored
                            if (!shouldIgnore(dirent.name, excludePatterns)) {
                                if (withFileTypes) {
                                    exlib::string fullPath = cwd + PATH_SLASH + dirent.name;
                                    obj_ptr<Stat> stat = createStat(fullPath);
                                    if (stat) {
                                        results.insert(GlobResult(dirent.name, stat));
                                    } else {
                                        results.insert(GlobResult(dirent.name));
                                    }
                                } else {
                                    results.insert(GlobResult(dirent.name));
                                }
                            }
                        }
                    }
                }
            }
        }
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
