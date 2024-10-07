
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

static std::string ldSoCacheContent;
static bool isCacheInitialized = false;

std::vector<std::string> searchSharedLibraries(const std::string& libraryName)
{
    std::vector<std::string> foundLibraries;

    if (!isCacheInitialized) {
        isCacheInitialized = true;
        std::ifstream ldSoCache("/etc/ld.so.cache", std::ios::binary);

        if (ldSoCache.is_open()) {
            ldSoCacheContent.assign((std::istreambuf_iterator<char>(ldSoCache)), std::istreambuf_iterator<char>());
        }
    }

    if (ldSoCacheContent.empty()) {
        return foundLibraries;
    }

    std::string escapedLibraryName = std::regex_replace(libraryName, std::regex("\\."), "\\.");
    std::regex libRegex("lib" + escapedLibraryName + R"([\d\.]*\.so(\.\d+)*)");
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

        for (const auto& lib : allLibraries) {
            std::cout << lib << std::endl;
        }

        return allLibraries.back();
    }

    return pkg;
}

#endif
