/*
 * DOMTokenList.cpp
 *
 *  Created on: Jan 5, 2026
 *      Author: lion
 */

#include "object.h"
#include "DOMTokenList.h"
#include "utf8.h"
#include <algorithm>

namespace fibjs {

DECLARE_MODULE(DOMTokenList);

void DOMTokenList::parseTokens(std::vector<exlib::string>& tokens)
{
    tokens.clear();

    exlib::string className;
    m_element->get_className(className);

    if (className.empty())
        return;

    const char* p = className.c_str();
    const char* end = p + className.length();

    while (p < end) {
        // Skip whitespace
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f'))
            p++;

        if (p >= end)
            break;

        // Find token end
        const char* start = p;
        while (p < end && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '\f')
            p++;

        if (p > start)
            tokens.push_back(exlib::string(start, p - start));
    }
}

void DOMTokenList::updateClassName(const std::vector<exlib::string>& tokens)
{
    exlib::string className;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (i > 0)
            className += ' ';
        className += tokens[i];
    }

    m_element->set_className(className);
}

result_t DOMTokenList::_indexed_getter(uint32_t index, exlib::string& retVal)
{
    return item((int32_t)index, retVal);
}

result_t DOMTokenList::get_length(int32_t& retVal)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);
    retVal = (int32_t)tokens.size();
    return 0;
}

result_t DOMTokenList::get_value(exlib::string& retVal)
{
    return m_element->get_className(retVal);
}

result_t DOMTokenList::item(int32_t index, exlib::string& retVal)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);

    if (index < 0 || index >= (int32_t)tokens.size())
        return CALL_RETURN_NULL;

    retVal = tokens[index];
    return 0;
}

result_t DOMTokenList::contains(exlib::string token, bool& retVal)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);

    retVal = std::find(tokens.begin(), tokens.end(), token) != tokens.end();
    return 0;
}

result_t DOMTokenList::add(OptArgs args)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);

    Isolate* isolate = holder();
    int32_t argc = args.Length();
    for (int32_t i = 0; i < argc; i++) {
        exlib::string token;
        result_t hr = GetArgumentValue(isolate, args[i], token);
        if (hr < 0)
            return hr;

        // Skip empty tokens and tokens with whitespace
        if (token.empty() || token.find_first_of(" \t\n\r\f") != exlib::string::npos)
            continue;

        // Add only if not already present
        if (std::find(tokens.begin(), tokens.end(), token) == tokens.end())
            tokens.push_back(token);
    }

    updateClassName(tokens);
    return 0;
}

result_t DOMTokenList::remove(OptArgs args)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);

    Isolate* isolate = holder();
    int32_t argc = args.Length();
    for (int32_t i = 0; i < argc; i++) {
        exlib::string token;
        result_t hr = GetArgumentValue(isolate, args[i], token);
        if (hr < 0)
            return hr;

        auto it = std::find(tokens.begin(), tokens.end(), token);
        if (it != tokens.end())
            tokens.erase(it);
    }

    updateClassName(tokens);
    return 0;
}

result_t DOMTokenList::toggle(exlib::string token, OptArgs args, bool& retVal)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);

    auto it = std::find(tokens.begin(), tokens.end(), token);
    bool exists = (it != tokens.end());

    // Check if force argument was provided
    if (args.Length() > 0) {
        bool force;
        result_t hr = GetArgumentValue(holder(), args[0], force);
        if (hr < 0)
            return hr;

        if (force) {
            // force=true: add if not exists
            if (!exists) {
                tokens.push_back(token);
                updateClassName(tokens);
            }
            retVal = true;
        } else {
            // force=false: remove if exists
            if (exists) {
                tokens.erase(it);
                updateClassName(tokens);
            }
            retVal = false;
        }
    } else {
        // No force argument: toggle
        if (exists) {
            tokens.erase(it);
            retVal = false;
        } else {
            tokens.push_back(token);
            retVal = true;
        }
        updateClassName(tokens);
    }

    return 0;
}

result_t DOMTokenList::replace(exlib::string oldToken, exlib::string newToken, bool& retVal)
{
    std::vector<exlib::string> tokens;
    parseTokens(tokens);

    auto it = std::find(tokens.begin(), tokens.end(), oldToken);
    if (it == tokens.end()) {
        retVal = false;
        return 0;
    }

    // Check if newToken already exists
    auto newIt = std::find(tokens.begin(), tokens.end(), newToken);
    if (newIt != tokens.end() && newIt != it) {
        // New token exists, just remove old one
        tokens.erase(it);
    } else {
        // Replace old with new
        *it = newToken;
    }

    updateClassName(tokens);
    retVal = true;
    return 0;
}

result_t DOMTokenList::toString(exlib::string& retVal)
{
    return get_value(retVal);
}

} /* namespace fibjs */
