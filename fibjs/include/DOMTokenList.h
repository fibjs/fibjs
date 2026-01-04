/*
 * DOMTokenList.h
 *
 *  Created on: Jun 1, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/DOMTokenList.h"
#include "ifs/XmlElement.h"
#include <vector>

namespace fibjs {

class DOMTokenList : public DOMTokenList_base {
public:
    DOMTokenList(XmlElement_base* element)
        : m_element(element)
    {
    }

public:
    // DOMTokenList_base
    virtual result_t _indexed_getter(uint32_t index, exlib::string& retVal);
    virtual result_t get_length(int32_t& retVal);
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t item(int32_t index, exlib::string& retVal);
    virtual result_t contains(exlib::string token, bool& retVal);
    virtual result_t add(OptArgs tokens);
    virtual result_t remove(OptArgs tokens);
    virtual result_t toggle(exlib::string token, OptArgs force, bool& retVal);
    virtual result_t replace(exlib::string oldToken, exlib::string newToken, bool& retVal);
    virtual result_t toString(exlib::string& retVal);

private:
    void parseTokens(std::vector<exlib::string>& tokens);
    void updateClassName(const std::vector<exlib::string>& tokens);

private:
    obj_ptr<XmlElement_base> m_element;
};

} /* namespace fibjs */
