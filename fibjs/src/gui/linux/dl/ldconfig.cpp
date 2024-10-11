/*
 * ldconfig.h
 *
 *  Created on: Oct 7, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include "ldconfig.h"

#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <iostream>

static std::string s_ldSoCacheContent;

std::vector<std::string> searchSharedLibraries(const std::string& libraryName)
{
    if (s_ldSoCacheContent.empty()) {
        std::ifstream ldSoCache("/etc/ld.so.cache", std::ios::binary);
        if (ldSoCache.is_open())
            s_ldSoCacheContent.assign((std::istreambuf_iterator<char>(ldSoCache)), std::istreambuf_iterator<char>());
    }

    std::string ldSoCacheContent;
    ldSoCacheContent.assign(s_ldSoCacheContent.c_str(), s_ldSoCacheContent.size());

    std::vector<std::string> foundLibraries;
    if (ldSoCacheContent.empty())
        return foundLibraries;

    std::string escapedLibraryName = "lib" + std::regex_replace(libraryName, std::regex("\\."), "\\.") + R"([\d\.]*\.so(\.\d+)*)";
    std::regex libRegex(escapedLibraryName);
    std::smatch match;
    std::string content = ldSoCacheContent;

    while (std::regex_search(content, match, libRegex)) {
        foundLibraries.push_back(match.str());
        content = match.suffix().str();
    }

    return foundLibraries;
}

std::string ldconfig(const char* pkg)
{
    std::vector<std::string> allLibraries = searchSharedLibraries(pkg);

    if (!allLibraries.empty()) {
        std::sort(allLibraries.begin(), allLibraries.end());
        return allLibraries.back();
    }

    return pkg;
}

#endif
