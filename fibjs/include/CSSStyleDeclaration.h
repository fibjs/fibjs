/*
 * CSSStyleDeclaration.h
 *
 *  Created on: Sep 3, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/CSSStyleDeclaration.h"
#include "ifs/XmlElement.h"

namespace fibjs {

class CSSStyleDeclaration : public CSSStyleDeclaration_base {
public:
    CSSStyleDeclaration(XmlElement_base* element)
        : m_element(element)
    {
    }

public:
    // CSSStyleDeclaration_base
    virtual result_t get_cssText(exlib::string& retVal);
    virtual result_t set_cssText(exlib::string newVal);
    virtual result_t get_length(int32_t& retVal);
    virtual result_t item(int32_t index, exlib::string& retVal);
    virtual result_t getPropertyValue(exlib::string property, exlib::string& retVal);
    virtual result_t getPropertyPriority(exlib::string property, exlib::string& retVal);
    virtual result_t setProperty(exlib::string property, exlib::string value, exlib::string priority);
    virtual result_t removeProperty(exlib::string property, exlib::string& retVal);
    virtual result_t _named_getter(exlib::string property, Variant& retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);
    virtual result_t _named_setter(exlib::string property, Variant newVal);
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal);

private:
    obj_ptr<XmlElement_base> m_element;
};

} /* namespace fibjs */
