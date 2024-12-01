/*
 * path_match.h
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <regex>

namespace fibjs {

inline bool isPathSeparator_(char c, bool isWindows)
{
    return c == '/' || (isWindows && c == '\\');
}

enum class GlobNodeType {
    Literal,
    Wildcard,
    Globstar,
    CharClass,
    Question,
    Sequence,
    Brace,
    BraceRange
};

enum class RangeType {
    String,
    Number
};

std::string buildNumberRange(const std::string& startStr, const std::string& endStr);

class GlobNode {
public:
    GlobNodeType type;
    std::string value;
    std::vector<GlobNode> children;
    bool negated = false;
    std::string rangeStart;
    std::string rangeEnd;
    RangeType rangeType = RangeType::String;
    std::vector<GlobNode> next;

    GlobNode(GlobNodeType t, const std::string& v = "", std::vector<GlobNode> c = std::vector<GlobNode>())
        : type(t)
        , value(v)
        , children(std::move(c))
    {
    }

    std::string typeToString() const;

    std::string dump(int indent = 0) const;
};

class GlobParser {
public:
    GlobParser(bool isWin)
        : isWindows(isWin)
    {
    }

    GlobNode parse(std::string_view pattern);

    std::string dump(std::string_view pattern);

private:
    bool isWindows;
    static const std::regex letterRangePattern;
    static const std::regex numberRangePattern;

    std::pair<GlobNode, std::string_view> parseSequence(std::string_view input, bool inBrace = false);
    std::pair<GlobNode, std::string_view> parseCharClass(std::string_view input);
    std::pair<GlobNode, std::string_view> parseBrace(std::string_view input);
};

class RegexBuilder {
public:
    RegexBuilder(bool isWindows);
    std::string buildFromNode(const GlobNode& node);

private:
    std::string buildSequence(const GlobNode& node);
    std::string buildBrace(const GlobNode& node);
    std::string buildBraceRange(const GlobNode& node);
    std::string buildCharRange(char start, char end);
    std::string escapeRegex(const std::string& str);

private:
    bool isWindows_;
};

class RegexMatcher {
public:
    static bool match(const GlobNode& pattern, std::string_view path, bool isWindows);
};

}
