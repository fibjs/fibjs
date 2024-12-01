/*
 * path_match_parser.cpp
 *
 *  Created on: Dec 2, 2024
 *      Author: lion
 */

#include "path_match.h"
#include <regex>
#include <cctype>

namespace fibjs {

const std::regex GlobParser::letterRangePattern(R"(^([a-zA-Z])\.\.([a-zA-Z])\})");
const std::regex GlobParser::numberRangePattern(R"(^(-?[0-9]+)\.\.(-?[0-9]+)\})");

GlobNode GlobParser::parse(std::string_view pattern)
{
    auto [node, remaining] = parseSequence(pattern);
    return node;
}

std::pair<GlobNode, std::string_view> GlobParser::parseSequence(std::string_view input, bool inBrace)
{
    GlobNode node(GlobNodeType::Sequence);

    while (!input.empty()) {
        if (inBrace && (input[0] == ',' || input[0] == '}'))
            return { std::move(node), input };

        if (input[0] == '*') {

            size_t starCount = 1;
            while (starCount < input.length() && input[starCount] == '*')
                starCount++;

            if (starCount >= 2) {
                input.remove_prefix(starCount);

                while (!input.empty() && isPathSeparator_(input[0], isWindows))
                    input.remove_prefix(1);
                node.children.push_back(GlobNode(GlobNodeType::Globstar));
            } else {
                input.remove_prefix(1);
                node.children.push_back(GlobNode(GlobNodeType::Wildcard));
            }
        } else if (input[0] == '?') {
            input.remove_prefix(1);
            node.children.push_back(GlobNode(GlobNodeType::Question));
        } else if (input[0] == '[') {
            auto [charClass, remaining] = parseCharClass(input);
            node.children.push_back(std::move(charClass));
            input = remaining;
        } else if (input[0] == '{') {
            auto [brace, remaining] = parseBrace(input);
            node.children.push_back(std::move(brace));
            input = remaining;
        } else if (isPathSeparator_(input[0], isWindows)) {
            input.remove_prefix(1);
            node.children.push_back(GlobNode(GlobNodeType::Literal, std::string(1, isWindows ? '\\' : '/')));
        } else {
            std::string literal;
            while (!input.empty() && input[0] != '*' && input[0] != '?' && input[0] != '[' && input[0] != '{' && !isPathSeparator_(input[0], isWindows) && !(inBrace && (input[0] == ',' || input[0] == '}'))) {
                if (input[0] == '\\' && input.length() > 1) {
                    input.remove_prefix(1);
                    literal += input[0];
                } else {
                    literal += input[0];
                }
                input.remove_prefix(1);
            }
            if (!literal.empty()) {
                node.children.push_back(GlobNode(GlobNodeType::Literal, literal));
            }
        }
    }
    return { std::move(node), input };
}

std::pair<GlobNode, std::string_view> GlobParser::parseCharClass(std::string_view input)
{
    GlobNode node(GlobNodeType::CharClass);
    input.remove_prefix(1);

    if (!input.empty() && (input[0] == '!' || input[0] == '^')) {
        node.negated = true;
        input.remove_prefix(1);
    }

    std::string chars;
    while (!input.empty() && input[0] != ']') {
        if (input[0] == '\\' && input.length() > 1) {
            input.remove_prefix(1);
            chars += input[0];
        } else {
            chars += input[0];
        }
        input.remove_prefix(1);
    }

    if (!input.empty()) {
        input.remove_prefix(1);
    }

    node.value = chars;
    return { std::move(node), input };
}

std::pair<GlobNode, std::string_view> GlobParser::parseBrace(std::string_view input)
{
    input.remove_prefix(1);
    std::vector<GlobNode> alternatives;
    bool isRange = false;
    std::string rangeStart, rangeEnd;
    RangeType rangeType = RangeType::String;
    bool hasEmptyRange = false;

    if (!input.empty()) {
        std::string pattern(input);
        std::smatch match;

        if (std::regex_search(pattern, match, letterRangePattern)) {
            char start = match.str(1)[0];
            char end = match.str(2)[0];

            if ((std::isupper(start) == std::isupper(end)) && (std::tolower(start) <= std::tolower(end))) {
                rangeStart = std::string(1, start);
                rangeEnd = std::string(1, end);
                input.remove_prefix(match.length());
                isRange = true;
                rangeType = RangeType::String;
            }
        }

        else if (std::regex_search(pattern, match, numberRangePattern)) {
            std::string start = match.str(1);
            std::string end = match.str(2);
            rangeStart = start;
            rangeEnd = end;
            input.remove_prefix(match.length());
            isRange = true;
            rangeType = RangeType::Number;
        }
    }

    if (isRange) {
        GlobNode rangeNode(GlobNodeType::BraceRange);
        rangeNode.rangeStart = rangeStart;
        rangeNode.rangeEnd = rangeEnd;
        rangeNode.rangeType = rangeType;

        return { std::move(rangeNode), input };
    }

    while (true) {
        if (input.empty() || input[0] == '}' || input[0] == ',') {
            hasEmptyRange = true;
        } else {
            auto [subNode, remaining] = parseSequence(input, true);
            alternatives.push_back(std::move(subNode));
            input = remaining;
        }

        if (input.empty() || input[0] != ',')
            break;

        input.remove_prefix(1);
    }

    if (hasEmptyRange)
        alternatives.push_back(GlobNode(GlobNodeType::Literal, ""));

    if (!input.empty() && input[0] == '}')
        input.remove_prefix(1);

    return { GlobNode(GlobNodeType::Brace, "", std::move(alternatives)), input };
}

std::string GlobNode::typeToString() const
{
    switch (type) {
    case GlobNodeType::Literal:
        return "Literal";
    case GlobNodeType::Wildcard:
        return "Wildcard";
    case GlobNodeType::Globstar:
        return "Globstar";
    case GlobNodeType::CharClass:
        return "CharClass";
    case GlobNodeType::Question:
        return "Question";
    case GlobNodeType::Sequence:
        return "Sequence";
    case GlobNodeType::Brace:
        return "Brace";
    case GlobNodeType::BraceRange:
        return "BraceRange";
    default:
        return "Unknown";
    }
}

std::string GlobNode::dump(int indent) const
{
    std::string result;
    std::string indentStr(indent * 2, ' ');

    result += indentStr + typeToString();

    switch (type) {
    case GlobNodeType::Literal:
        result += ": \"" + value + "\"";
        break;
    case GlobNodeType::CharClass:
        result += (negated ? " (negated): " : ": ") + value;
        break;
    case GlobNodeType::BraceRange:
        result += ": " + rangeStart + ".." + rangeEnd;
        if (rangeType == RangeType::Number)
            result += " (numeric)";
        break;
    default:
        if (!value.empty())
            result += ": " + value;
        break;
    }

    result += "\n";

    for (const auto& child : children) {
        result += child.dump(indent + 1);
    }

    return result;
}

std::string GlobParser::dump(std::string_view pattern)
{
    GlobNode root = parse(pattern);
    return "Pattern: \"" + std::string(pattern) + "\"\n" + root.dump(0);
}

}
