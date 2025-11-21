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
#include "DirEntry.h"
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

// Helper function to check if pattern ends with path separator
static bool endsWithPathSeparator(const exlib::string& pattern)
{
    if (pattern.empty()) {
        return false;
    }
    return isPathSeparator(pattern[pattern.length() - 1]);
}

// Helper function to get pattern without trailing path separator
static exlib::string removeTrailingPathSeparator(const exlib::string& pattern)
{
    if (pattern.empty() || !endsWithPathSeparator(pattern)) {
        return pattern;
    }
    return pattern.substr(0, pattern.length() - 1);
}

// Structure to hold path and dirent information
struct GlobResult {
    exlib::string path;
    obj_ptr<DirEntry> dirent;

    GlobResult(const exlib::string& p)
        : path(p)
    {
    }
    GlobResult(const exlib::string& p, obj_ptr<DirEntry> d)
        : path(p)
        , dirent(d)
    {
    }

    bool operator<(const GlobResult& other) const
    {
        return path < other.path;
    }
};

// Helper function to normalize path separators for the current platform
// Use path module's normalize function for better path handling
static exlib::string normalizePath(const exlib::string& path)
{
    exlib::string normalized;
    result_t hr = path_base::normalize(path, normalized);
    return (hr >= 0) ? normalized : path;
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
// Use path module's functionality for better relative path detection
static bool isRelativePattern(const exlib::string& pattern)
{
    if (pattern.length() < 2) {
        return false;
    }

    // Use path module to normalize and check if pattern starts with current directory reference
    if (pattern.substr(0, 2) == "./"
#ifdef _WIN32
        || pattern.substr(0, 2) == ".\\"
#endif
    ) {
        return true;
    }

    // Also check using path module's resolve functionality to detect relative patterns
    exlib::string normalized;
    result_t hr = path_base::normalize(pattern, normalized);
    if (hr >= 0) {
        // If normalized path is different and doesn't start with /, it might be relative
        bool isAbs = false;
        result_t absCheck = path_base::isAbsolute(normalized, isAbs);
        if (absCheck >= 0 && !isAbs && normalized.length() >= 2) {
            return normalized.substr(0, 2) == "./"
#ifdef _WIN32
                || normalized.substr(0, 2) == ".\\"
#endif
                ;
        }
    }

    return false;
}

// Helper function to check if a pattern is absolute
// Use path module's isAbsolute function for better detection
static bool isAbsolutePattern(const exlib::string& pattern)
{
    if (pattern.length() == 0)
        return false;

    bool retVal;
    result_t hr = path_base::isAbsolute(pattern, retVal);
    return (hr >= 0) ? retVal : false;
}

#ifdef _WIN32
// Helper function to check if a pattern is UNC path (Windows only)
static bool isUNCPath(const exlib::string& pattern)
{
    return pattern.length() >= 2 && pattern[0] == '\\' && pattern[1] == '\\';
}

// Helper function to check if a pattern has a drive letter (Windows only)
static bool hasDriveLetter(const exlib::string& pattern)
{
    return pattern.length() >= 3 && qisascii(pattern[0]) && pattern[1] == ':' && isPathSlash(pattern[2]);
}
#endif

// Helper function to normalize pattern
// Use path module's normalize function for better pattern handling
static exlib::string normalizePattern(const exlib::string& pattern)
{
    if (isRelativePattern(pattern)) {
        // Remove "./" or ".\" prefix for relative patterns
        if (pattern.length() >= 2 && (pattern.substr(0, 2) == "./"
#ifdef _WIN32
                || pattern.substr(0, 2) == ".\\"
#endif
                )) {
            exlib::string withoutPrefix = pattern.substr(2);
            // Use path module to normalize the remaining pattern
            exlib::string normalized;
            result_t hr = path_base::normalize(withoutPrefix, normalized);
            return (hr >= 0) ? normalized : withoutPrefix;
        }
    }

    // For non-relative patterns, still normalize them using path module
    exlib::string normalized;
    result_t hr = path_base::normalize(pattern, normalized);
    return (hr >= 0) ? normalized : pattern;
}

// Helper function to check if path contains directories
// Use path module's dirname function for better directory detection
static bool hasDirectory(const exlib::string& pattern)
{
    if (pattern.empty()) {
        return false;
    }

    // Use path module's dirname to check if pattern has directory components
    exlib::string dirname;
    result_t hr = path_base::dirname(pattern, dirname);
    if (hr >= 0) {
        // If dirname is not "." and not empty, then the pattern contains directories
        return !dirname.empty() && dirname != ".";
    }

    // Fallback to manual check if path module fails
#ifdef _WIN32
    return pattern.find('/') != exlib::string::npos || pattern.find('\\') != exlib::string::npos;
#else
    return pattern.find('/') != exlib::string::npos;
#endif
}

// Helper function to get directory part of a pattern
// Use path module's dirname function for better accuracy
static exlib::string getDirectoryPart(const exlib::string& pattern)
{
    exlib::string dirname;
    result_t hr = path_base::dirname(pattern, dirname);
    if (hr >= 0 && !dirname.empty() && dirname != ".") {
        return dirname;
    }
    return "";
}

// Helper function to get filename part of a pattern
// Use path module's basename function for better accuracy
static exlib::string getFilenamePart(const exlib::string& pattern)
{
    exlib::string basename;
    result_t hr = path_base::basename(pattern, "", basename);
    return (hr >= 0) ? basename : pattern;
}

// Helper function to split pattern into components
// Use path module's normalize to handle complex path structures
static std::vector<exlib::string> splitPattern(const exlib::string& pattern)
{
    std::vector<exlib::string> components;
    if (pattern.empty()) {
        return components;
    }

    // First normalize the pattern to handle various path formats consistently
    exlib::string normalized;
    result_t hr = path_base::normalize(pattern, normalized);
    const exlib::string& workingPattern = (hr >= 0) ? normalized : pattern;

    size_t start = 0;
    size_t pos = 0;

#ifdef _WIN32
    // Special handling for UNC paths - skip the initial "\\" and treat it as a single entity
    if (isUNCPath(workingPattern)) {
        start = 2; // Skip the UNC prefix \\
        pos = 2;
    }
#endif

    // Use path module's path separator detection for better cross-platform compatibility
    while (pos < workingPattern.length()) {
        char c = workingPattern[pos];

        if (isPathSlash(c)) {
            if (pos > start) {
                components.push_back(workingPattern.substr(start, pos - start));
            }
            start = pos + 1;
        }
        pos++;
    }

    // Add the last component
    if (start < workingPattern.length()) {
        components.push_back(workingPattern.substr(start));
    } else if (start == workingPattern.length() && workingPattern.length() > 0) {
        // Pattern ends with separator, add empty component to preserve trailing separator meaning
        char lastChar = workingPattern[workingPattern.length() - 1];
        if (isPathSlash(lastChar)) {
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

// Helper function to create DirEntry object for a file
static obj_ptr<DirEntry> createDirEntry(const exlib::string& name, const exlib::string& parentPath, uint32_t mode)
{
    obj_ptr<DirEntry> dirent = new DirEntry();
    dirent->fill(name, parentPath, mode);
    return dirent;
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
        // Check if we need to match a prefix pattern using path separator helper
        if (pattern.length() > 2 && isPathSeparator(pattern[pattern.length() - 3])) {
            // Pattern like "src/**" - extract prefix "src"
            exlib::string prefix = pattern.substr(0, pattern.length() - 3);

            // If current path matches prefix, include it in results
            if (currentPath == prefix || (currentPath.empty() && prefix.empty()) || matchesGlob(currentPath, prefix, isWindows)) {

                if (!shouldIgnore(currentPath.empty() ? "." : currentPath, excludePatterns)) {
                    exlib::string resultPath = currentPath.empty() ? "." : currentPath;
                    if (withFileTypes) {
                        exlib::string name;
                        exlib::string parent;
                        if (!currentPath.empty()) {
                            path_base::basename(currentPath, "", name);
                            path_base::dirname(currentPath, parent);
                            if (parent.empty())
                                parent = basePath;
                            else
                                os_join(basePath, parent, parent);
                        } else {
                            path_base::basename(basePath, "", name);
                            path_base::dirname(basePath, parent);
                        }
                        // For ** patterns, we know these are directories
                        obj_ptr<DirEntry> dirent = createDirEntry(name, parent, UV_DIRENT_DIR);
                        if (dirent) {
                            results.insert(GlobResult(resultPath, dirent));
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

    exlib::string fullPath;
    if (!currentPath.empty()) {
        // Use path module's os_join for proper cross-platform path construction
        os_join(basePath, currentPath, fullPath);
    } else {
        fullPath = basePath;
    }

    AutoReq req;
    int32_t ret = uv_fs_scandir(NULL, &req, fullPath.c_str(), 0, NULL);
    if (ret < 0)
        return; // Directory doesn't exist or can't be read

    uv_dirent_t dirent;
    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
        exlib::string entryName = dirent.name;
        exlib::string relativePath;

        if (currentPath.empty()) {
            relativePath = entryName;
        } else {
            // Use path module's os_join for proper cross-platform path construction
            os_join(currentPath, entryName, relativePath);
            // Normalize the result to ensure consistent path format
            exlib::string normalized;
            result_t hr = path_base::normalize(relativePath, normalized);
            if (hr >= 0) {
                relativePath = normalized;
            }
        }

        // Skip if ignored
        if (shouldIgnore(relativePath, excludePatterns)) {
            continue;
        }

        // Check if this path matches the pattern
        bool isMatch = false;

        // Special handling for patterns ending with path separator - only match directories
        if (endsWithPathSeparator(pattern)) {
            if (dirent.type == UV_DIRENT_DIR) {
                exlib::string dirPattern = removeTrailingPathSeparator(pattern);
                isMatch = matchesGlob(relativePath, dirPattern, isWindows);
            }
        } else {
            // Normal pattern matching
            isMatch = matchesGlob(relativePath, pattern, isWindows);
        }

        if (isMatch) {
            if (withFileTypes) {
                exlib::string entryFullPath;
                if (!currentPath.empty()) {
                    // Use path module's os_join for proper cross-platform path construction
                    os_join(basePath, currentPath, entryFullPath);
                } else {
                    entryFullPath = basePath;
                }

                obj_ptr<DirEntry> direntObj = createDirEntry(entryName, entryFullPath, dirent_type_to_mode(dirent.type));
                if (direntObj) {
                    results.insert(GlobResult(relativePath, direntObj));
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
                        exlib::string name, parent;
                        path_base::basename(normalizedPattern, "", name);
                        path_base::dirname(normalizedPattern, parent);

                        // Use st_mode directly
                        uint32_t mode = req.statbuf.st_mode;

                        obj_ptr<DirEntry> dirent = createDirEntry(name, parent, mode);
                        if (dirent) {
                            results.insert(GlobResult(normalizedPattern, dirent));
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
#ifdef _WIN32
                    // First component has wildcard, we need to handle drive letter
                    // Extract drive letter from normalized pattern if present
                    if (hasDriveLetter(normalizedPattern)) {
                        basePath = normalizedPattern.substr(0, 3); // e.g., "C:\"
                    } else if (isUNCPath(normalizedPattern)) {
                        // UNC path - find the first component (\\server\share)
                        size_t pos = 2;
                        int backslashCount = 0;
                        while (pos < normalizedPattern.length() && backslashCount < 2) {
                            if (isPathSlash(normalizedPattern[pos])) {
                                backslashCount++;
                            }
                            pos++;
                        }
                        basePath = normalizedPattern.substr(0, pos - 1);
                    } else {
                        basePath = "\\"; // Fallback
                    }
#else
                    // First component has wildcard, use root as base
                    basePath = "/";
#endif
                } else {
                    // Build base path from components before wildcard
                    // Use path module's vector-based join for proper cross-platform path construction
                    std::vector<exlib::string> baseComponents;

#ifdef _WIN32
                    // Handle Windows drive letter
                    if (hasDriveLetter(normalizedPattern)) {
                        // For patterns like "D:\path\*.js", components are ["D:", "path", "*.js"]
                        // Build vector with drive letter and path components
                        baseComponents.push_back(components[0] + "\\");
                        for (int i = 1; i < wildcardIndex; ++i) {
                            baseComponents.push_back(components[i]);
                        }

                        // Use os_join for cross-platform path construction
                        result_t hr = os_join(baseComponents, basePath);
                        if (hr < 0) {
                            basePath = "";
                        }
                    } else if (isUNCPath(normalizedPattern)) {
                        // UNC path - manually construct to preserve \\ prefix
                        // Cannot use os_join because it may normalize away the UNC prefix
                        basePath = "\\\\";
                        for (int i = 0; i < wildcardIndex; ++i) {
                            if (i > 0) {
                                basePath += "\\";
                            }
                            basePath += components[i];
                        }
                    } else {
                        // Regular absolute path
                        baseComponents.push_back("\\");
                        for (int i = 0; i < wildcardIndex; ++i) {
                            baseComponents.push_back(components[i]);
                        }

                        // Use os_join for cross-platform path construction
                        result_t hr = os_join(baseComponents, basePath);
                        if (hr < 0) {
                            basePath = "";
                        }
                    }
#else
                    // POSIX path - start with root
                    baseComponents.push_back("/");
                    for (int i = 0; i < wildcardIndex; ++i) {
                        baseComponents.push_back(components[i]);
                    }

                    // Use os_join for cross-platform path construction
                    result_t hr = os_join(baseComponents, basePath);
                    if (hr < 0) {
                        // This should not happen with os_join, but keep as safety fallback
                        basePath = ""; // Handle error case
                    }
#endif
                }

                // Build relative pattern from wildcard component onwards
                std::vector<exlib::string> relativeComponents(components.begin() + wildcardIndex, components.end());
                exlib::string relativePattern;
                os_join(relativeComponents, relativePattern);

                // Preserve trailing path separator from original pattern if it exists
                if (endsWithPathSeparator(normalizedPattern)) {
                    relativePattern += "/";
                }

                // Use temporary results set for this pattern
                std::set<GlobResult> tempResults;
                walkDirectorySimple(basePath, "", relativePattern, excludePatterns, tempResults, withFileTypes);

                // Convert relative results to absolute using os_join
                for (const auto& result : tempResults) {
                    exlib::string absolutePath;

                    // Use os_join for cross-platform absolute path construction
                    result_t hr = os_join(basePath, result.path, absolutePath);

                    // Fallback to manual construction if path module fails
                    if (hr < 0) {
                        // This should rarely happen with os_join
                        absolutePath = basePath + result.path; // Simple fallback
                    }

                    if (result.dirent) {
                        results.insert(GlobResult(absolutePath, result.dirent));
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
        if (withFileTypes && result.dirent) {
            retVal->append((object_base*)result.dirent);
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
