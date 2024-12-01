/*
 * path_match_number.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"
#include <string>
#include <vector>
#include <sstream>

namespace fibjs {

static size_t findFirstDifferentDigitIndex(const std::string& str1, const std::string& str2)
{
    size_t index;
    for (index = 0; index < str1.length(); index++)
        if (str1[index] != str2[index])
            return index;
    return index;
}

static std::string buildRange(char startChar, char endChar)
{
    if (startChar == endChar)
        return std::string(1, startChar);
    if (startChar + 1 == endChar)
        return "[" + std::string(1, startChar) + std::string(1, endChar) + "]";
    if (startChar + 2 == endChar)
        return "[" + std::string(1, startChar) + std::string(1, startChar + 1) + std::string(1, endChar) + "]";

    return "[" + std::string(1, startChar) + "-" + std::string(1, endChar) + "]";
}

static std::string buildSegment(const std::string& str, size_t position, char startChar, char endChar)
{
    std::string pattern = str.substr(0, position);
    pattern += buildRange(startChar, endChar);
    for (size_t i = position + 1; i < str.length(); i++)
        pattern += "[0-9]";
    return pattern;
}

static std::string buildUnsignedNumberRange(const std::string& startStr, const std::string& endStr)
{
    size_t maxLength = std::max(startStr.length(), endStr.length());
    std::string paddedStart = startStr;
    std::string paddedEnd = endStr;

    if (startStr.length() < maxLength) {
        paddedStart = std::string(maxLength - startStr.length(), '0') + startStr;
    }
    if (endStr.length() < maxLength) {
        paddedEnd = std::string(maxLength - endStr.length(), '0') + endStr;
    }

    if (paddedStart == paddedEnd)
        return startStr;

    if (paddedStart > paddedEnd)
        std::swap(paddedStart, paddedEnd);

    std::vector<std::string> patterns;
    size_t currentPos = maxLength - 1;

    while (currentPos > findFirstDifferentDigitIndex(paddedStart, paddedEnd)) {
        if (paddedStart[currentPos] != '0') {
            patterns.push_back(buildSegment(paddedStart, currentPos, paddedStart[currentPos], '9'));

            size_t prevPos = currentPos - 1;
            while (prevPos < paddedStart.length() && ++paddedStart[prevPos] > '9') {
                paddedStart[prevPos] = '0';
                --prevPos;
            }

            paddedStart[currentPos] = '0';
        }
        currentPos--;
    }

    currentPos = findFirstDifferentDigitIndex(paddedStart, paddedEnd);
    if (currentPos == maxLength) {
        patterns.push_back(paddedEnd);
    } else {
        while (currentPos < maxLength - 1) {
            if (paddedEnd[currentPos] != '0')
                patterns.push_back(buildSegment(paddedEnd, currentPos, paddedStart[currentPos], paddedEnd[currentPos] - 1));
            currentPos++;
        }
        patterns.push_back(buildSegment(paddedEnd, currentPos, paddedStart[currentPos], paddedEnd[currentPos]));
    }

    if (patterns.empty())
        return "";

    if (patterns.size() == 1)
        return "(" + patterns[0] + ")";

    std::ostringstream result;
    result << "((";
    for (size_t i = 0; i < patterns.size(); i++) {
        if (i > 0)
            result << ")|(";
        result << patterns[i];
    }
    result << "))";

    return result.str();
}

std::string buildNumberRange(const std::string& startStr, const std::string& endStr)
{
    if (startStr.empty() || endStr.empty())
        return "";

    bool startNegative = startStr[0] == '-';
    bool endNegative = endStr[0] == '-';

    size_t maxWidth = std::max(
        startNegative ? startStr.length() - 1 : startStr.length(),
        endNegative ? endStr.length() - 1 : endStr.length());

    if (startNegative) {
        if (endNegative)
            return "-" + buildUnsignedNumberRange(startStr.substr(1), endStr.substr(1));

        std::string paddedOne = std::string(maxWidth - 1, '0') + "1";
        return "(-" + buildUnsignedNumberRange(startStr.substr(1), paddedOne) + "|" + buildUnsignedNumberRange(std::string(maxWidth, '0'), endStr) + ")";
    }

    if (endNegative)
        return buildNumberRange(endStr, startStr);

    return buildUnsignedNumberRange(startStr, endStr);
}

}