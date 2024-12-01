/*
 * path_match_regex.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"
#include <regex>
#include <sstream>

namespace fibjs {

RegexBuilder::RegexBuilder(bool isWindows)
    : isWindows_(isWindows)
{
}

std::string RegexBuilder::buildFromNode(const GlobNode& node)
{
    switch (node.type) {
    case GlobNodeType::Literal:
        return escapeRegex(node.value);
    case GlobNodeType::Wildcard:
        return isWindows_ ? "[^\\\\/]*?" : "[^/]*?";
    case GlobNodeType::Globstar:
        return ".*";
    case GlobNodeType::CharClass: {
        std::stringstream ss;
        ss << "[";
        if (node.negated)
            ss << "^";

        if (!node.value.empty() && node.value[0] == '-') {
            ss << "-";
            ss << escapeRegex(node.value.substr(1));
        } else if (!node.value.empty() && node.value.back() == '-') {
            ss << escapeRegex(node.value.substr(0, node.value.length() - 1));
            ss << "-";
        } else {
            ss << escapeRegex(node.value);
        }
        ss << "]";
        return ss.str();
    }
    case GlobNodeType::Question:
        return isWindows_ ? "[^\\\\/]" : "[^/]";
    case GlobNodeType::Sequence:
        return buildSequence(node);
    case GlobNodeType::Brace:
        return buildBrace(node);
    case GlobNodeType::BraceRange:
        return buildBraceRange(node);
    default:
        return "";
    }
}

std::string RegexBuilder::buildSequence(const GlobNode& node)
{
    std::stringstream ss;
    for (const auto& child : node.children) {
        ss << buildFromNode(child);
    }
    return ss.str();
}

std::string RegexBuilder::buildBrace(const GlobNode& node)
{
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < node.children.size(); ++i) {
        if (i > 0)
            ss << "|";
        ss << buildFromNode(node.children[i]);
    }
    ss << ")";
    return ss.str();
}

std::string RegexBuilder::buildBraceRange(const GlobNode& node)
{
    if (node.rangeType == RangeType::Number)
        return buildNumberRange(node.rangeStart, node.rangeEnd);

    return buildCharRange(node.rangeStart[0], node.rangeEnd[0]);
}

std::string RegexBuilder::buildCharRange(char start, char end)
{
    std::stringstream ss;
    ss << "[" << start << "-" << end << "]";
    return ss.str();
}

std::string RegexBuilder::escapeRegex(const std::string& str)
{
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
        case '.':
        case '^':
        case '$':
        case '*':
        case '+':
        case '?':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '|':
            ss << '\\' << c;
            break;
        case '\\':
            if (!isWindows_) {
                ss << '\\' << c;
                break;
            }
        case '/':
            ss << (isWindows_ ? "[/\\\\]" : "/");
            break;
        default:
            ss << c;
        }
    }
    return ss.str();
}

bool RegexMatcher::match(const GlobNode& pattern, std::string_view path, bool isWindows)
{
    RegexBuilder builder(isWindows);
    std::string regexStr = "^" + builder.buildFromNode(pattern) + "$";
    std::regex re(regexStr);
    return std::regex_match(std::string(path), re);
}

}
