/*
 * DOMStringMap.h
 *
 *  Created on: Sep 3, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/DOMStringMap.h"
#include "ifs/XmlElement.h"

namespace fibjs {

class DOMStringMap : public DOMStringMap_base {
public:
    DOMStringMap(XmlElement_base* element)
        : m_element(element)
    {
    }

public:
    // DOMStringMap_base
    virtual result_t _indexed_getter(uint32_t index, Variant& retVal);
    virtual result_t _named_getter(exlib::string property, Variant& retVal);
    virtual result_t _named_setter(exlib::string property, Variant newVal);
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal);
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal);

private:
    obj_ptr<XmlElement_base> m_element;
};

} /* namespace fibjs */
