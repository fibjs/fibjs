/*
 * DOMStringMap.cpp
 *
 *  Created on: Sep 3, 2026
 *      Author: lion
 */

#include "object.h"
#include "DOMStringMap.h"
#include "ifs/xml.h"

namespace fibjs {

DECLARE_MODULE(DOMStringMap);

namespace {

// Convert data-xxx-yyy to xxxYyy (camelCase)
// Per MDN spec: dash followed by lowercase letter -> remove dash, uppercase letter
// Other dashes are preserved
exlib::string dataAttrToCamelCase(const exlib::string& name)
{
    // name starts with "data-", remove it
    exlib::string result;

    for (size_t i = 5; i < name.length(); i++) {
        char c = name[i];
        if (c == '-' && i + 1 < name.length()) {
            char next = name[i + 1];
            if (next >= 'a' && next <= 'z') {
                // Dash followed by lowercase: remove dash, uppercase the letter
                result += (next - 'a' + 'A');
                i++; // skip the next character as we've processed it
            } else {
                // Dash not followed by lowercase: keep the dash
                result += c;
            }
        } else {
            result += c;
        }
    }

    return result;
}

// Inverse of dataAttrToCamelCase: xxxYyy -> xxx-yyy
// Uppercase letters become "-" + lowercase; all other characters (including
// dashes) are preserved, so the round-trip is lossless.
exlib::string camelCaseToDataAttr(const exlib::string& key)
{
    exlib::string result;
    const char* p = key.c_str();
    size_t n = key.length();

    result.reserve(n + 4);

    for (size_t i = 0; i < n; i++) {
        char c = p[i];
        if (c >= 'A' && c <= 'Z') {
            result += '-';
            result += (char)(c - 'A' + 'a');
        } else {
            result += c;
        }
    }

    return result;
}

} // namespace

result_t DOMStringMap::_indexed_getter(uint32_t index, Variant& retVal)
{
    // dataset[123] maps to the data-123 attribute
    char buf[32];
    int32_t len = snprintf(buf, sizeof(buf), "%u", index);

    exlib::string name = "data-";
    name.append(buf, len);

    exlib::string value;
    result_t hr = m_element->getAttribute(name, value);
    if (hr < 0)
        return hr;
    if (hr == CALL_RETURN_NULL)
        return CALL_RETURN_NULL;

    retVal = value;
    return 0;
}

result_t DOMStringMap::_named_getter(exlib::string property, Variant& retVal)
{
    exlib::string name = "data-" + camelCaseToDataAttr(property);

    exlib::string value;
    result_t hr = m_element->getAttribute(name, value);
    if (hr < 0)
        return hr;
    if (hr == CALL_RETURN_NULL)
        return CALL_RETURN_NULL;

    retVal = value;
    return 0;
}

result_t DOMStringMap::_named_setter(exlib::string property, Variant newVal)
{
    exlib::string name = "data-" + camelCaseToDataAttr(property);
    exlib::string value;

    Variant::Type t = newVal.type();
    if (t == Variant::VT_Undefined || t == Variant::VT_Null)
        return m_element->removeAttribute(name);

    newVal.toString(value);
    if (value.empty())
        return m_element->removeAttribute(name);

    return m_element->setAttribute(name, value);
}

result_t DOMStringMap::_named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal)
{
    exlib::string name = "data-" + camelCaseToDataAttr(property);

    m_element->removeAttribute(name);

    retVal = v8::True(Isolate::current()->m_isolate);
    return 0;
}

result_t DOMStringMap::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    obj_ptr<XmlNamedNodeMap_base> attrs;
    m_element->get_attributes(attrs);

    int32_t len;
    attrs->get_length(len);

    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
    uint32_t idx = 0;

    for (int32_t i = 0; i < len; i++) {
        obj_ptr<XmlAttr_base> attr;
        if (attrs->item(i, attr) < 0)
            break;

        exlib::string name;
        attr->get_nodeName(name);

        if (name.length() > 5 && name.substr(0, 5) == "data-") {
            arr->Set(context, idx++, isolate->NewString(dataAttrToCamelCase(name))).IsJust();
        }
    }

    retVal = arr;
    return 0;
}

} // namespace fibjs
