/*
 * CSSStyleDeclaration.cpp
 *
 *  Created on: Sep 3, 2026
 *      Author: lion
 */

#include "object.h"
#include "CSSStyleDeclaration.h"
#include "qstring.h"
#include <vector>

namespace fibjs {

DECLARE_MODULE(CSSStyleDeclaration);

namespace {

struct Decl {
    exlib::string name;
    exlib::string value;
    bool important;
};

bool is_css_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
}

void trim_css(exlib::string& s)
{
    size_t b = 0;
    size_t e = s.length();

    while (b < e && is_css_space(s[b]))
        b++;

    while (e > b && is_css_space(s[e - 1]))
        e--;

    if (b > 0 || e < s.length())
        s = s.substr(b, e - b);
}

bool is_custom_property(const exlib::string& name)
{
    return name.length() > 2 && name[0] == '-' && name[1] == '-';
}

bool name_equals(const exlib::string& a, const exlib::string& b)
{
    if (is_custom_property(a) || is_custom_property(b))
        return a == b;

    return !qstricmp(a.c_str(), b.c_str());
}

// Split a declaration block into top-level declarations. Handles quoted
// strings, url(...) and other parenthesized values, and strips comments.
void split_declarations(const exlib::string& text, std::vector<exlib::string>& segs)
{
    exlib::string cur;
    const char* p = text.c_str();
    size_t n = text.length();
    size_t i = 0;
    char quote = 0;
    int32_t depth = 0;

    cur.reserve(n);

    while (i < n) {
        char c = p[i];

        if (quote) {
            cur += c;

            if (c == '\\' && i + 1 < n) {
                cur += p[i + 1];
                i += 2;
                continue;
            }

            if (c == quote)
                quote = 0;

            i++;
            continue;
        }

        if (c == '\'' || c == '"') {
            quote = c;
            cur += c;
            i++;
            continue;
        }

        if (c == '/' && i + 1 < n && p[i + 1] == '*') {
            // Skip comment
            i += 2;
            while (i + 1 < n && !(p[i] == '*' && p[i + 1] == '/'))
                i++;

            i += 2;
            continue;
        }

        if (c == '(' || c == '[' || c == '{') {
            depth++;
            cur += c;
            i++;
            continue;
        }

        if (c == ')' || c == ']' || c == '}') {
            if (depth > 0)
                depth--;
            cur += c;
            i++;
            continue;
        }

        if (c == ';' && depth == 0) {
            trim_css(cur);
            if (!cur.empty()) {
                segs.push_back(cur);
                cur.clear();
            }
            i++;
            continue;
        }

        cur += c;
        i++;
    }

    trim_css(cur);
    if (!cur.empty())
        segs.push_back(cur);
}

// Split one declaration into property name and value at the first ':' that is
// not inside quotes or parentheses (e.g. url(http://...), data: URIs).
bool split_declaration(const exlib::string& seg, exlib::string& name, exlib::string& value)
{
    const char* p = seg.c_str();
    size_t n = seg.length();
    size_t i = 0;
    char quote = 0;
    int32_t depth = 0;

    while (i < n) {
        char c = p[i];

        if (quote) {
            if (c == '\\' && i + 1 < n) {
                i += 2;
                continue;
            }

            if (c == quote)
                quote = 0;

            i++;
            continue;
        }

        if (c == '\'' || c == '"') {
            quote = c;
            i++;
            continue;
        }

        if (c == '(' || c == '[' || c == '{') {
            depth++;
            i++;
            continue;
        }

        if (c == ')' || c == ']' || c == '}') {
            if (depth > 0)
                depth--;
            i++;
            continue;
        }

        if (c == ':' && depth == 0) {
            name = seg.substr(0, i);
            value = seg.substr(i + 1);

            trim_css(name);
            trim_css(value);

            return !name.empty();
        }

        i++;
    }

    // No colon: treat the whole segment as value with empty name
    name.clear();
    value = seg;
    trim_css(value);

    return false;
}

void parse_declarations(const exlib::string& text, std::vector<Decl>& out)
{
    std::vector<exlib::string> segs;
    exlib::string name;
    exlib::string value;

    split_declarations(text, segs);

    for (size_t i = 0; i < segs.size(); i++) {
        if (!split_declaration(segs[i], name, value))
            continue;

        // CSS property names are case-insensitive, custom properties are not
        if (!is_custom_property(name))
            exlib::qstrlwr(name);

        bool important = false;
        if (value.length() >= 10) {
            size_t start = value.length() - 10;
            if (!qstricmp(value.c_str() + start, "!important")) {
                important = true;
                value.resize(start);
                trim_css(value);
            }
        }

        if (value.empty())
            continue;

        // CSSOM parses a declaration block with cascade-like duplicate
        // handling (matching browsers):
        //  - a later plain declaration cannot override an earlier !important
        //    one (it is ignored);
        //  - otherwise the earlier declaration is removed and the new one is
        //    appended at the end of the declaration list.
        bool handled = false;
        for (size_t j = 0; j < out.size(); j++) {
            if (!name_equals(out[j].name, name))
                continue;

            if (out[j].important && !important) {
                // earlier !important wins, this declaration is ignored
            } else {
                out.erase(out.begin() + j);
                Decl d;
                d.name = name;
                d.value = value;
                d.important = important;
                out.push_back(d);
            }

            handled = true;
            break;
        }

        if (!handled) {
            Decl d;
            d.name = name;
            d.value = value;
            d.important = important;
            out.push_back(d);
        }
    }
}

void serialize_declarations(const std::vector<Decl>& decls, exlib::string& out)
{
    for (size_t i = 0; i < decls.size(); i++) {
        out += decls[i].name;
        out += ": ";
        out += decls[i].value;

        if (decls[i].important)
            out += " !important";

        out += "; ";
    }

    // CSSOM serializes a non-empty declaration block with a trailing ';'
    if (!decls.empty())
        out.resize(out.length() - 1);
}

// Map JS style property names (camelCase) to CSS property names (kebab-case).
// cssFloat is the CSSOM property name for the CSS "float" property.
exlib::string style_name_to_css(exlib::string name)
{
    if (name == "cssFloat")
        return "float";

    if (is_custom_property(name))
        return name;

    exlib::string out;
    const char* p = name.c_str();
    size_t n = name.length();

    out.reserve(n + 4);

    for (size_t i = 0; i < n; i++) {
        char c = p[i];

        if (c >= 'A' && c <= 'Z') {
            out += '-';
            out += (char)(c - 'A' + 'a');
        } else
            out += c;
    }

    return out;
}

} // namespace

result_t CSSStyleDeclaration::get_cssText(exlib::string& retVal)
{
    result_t hr = m_element->getAttribute("style", retVal);
    if (hr == CALL_RETURN_NULL)
        retVal.clear();

    return 0;
}

result_t CSSStyleDeclaration::set_cssText(exlib::string newVal)
{
    std::vector<Decl> decls;

    parse_declarations(newVal, decls);

    if (decls.empty())
        return m_element->removeAttribute("style");

    exlib::string text;
    serialize_declarations(decls, text);

    return m_element->setAttribute("style", text);
}

result_t CSSStyleDeclaration::get_length(int32_t& retVal)
{
    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    retVal = (int32_t)decls.size();
    return 0;
}

result_t CSSStyleDeclaration::item(int32_t index, exlib::string& retVal)
{
    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    if (index < 0 || index >= (int32_t)decls.size()) {
        retVal.clear();
        return 0;
    }

    retVal = decls[index].name;
    return 0;
}

result_t CSSStyleDeclaration::getPropertyValue(exlib::string property, exlib::string& retVal)
{
    retVal.clear();

    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    for (size_t i = 0; i < decls.size(); i++)
        if (name_equals(decls[i].name, property)) {
            retVal = decls[i].value;
            break;
        }

    return 0;
}

result_t CSSStyleDeclaration::getPropertyPriority(exlib::string property, exlib::string& retVal)
{
    retVal.clear();

    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    for (size_t i = 0; i < decls.size(); i++)
        if (name_equals(decls[i].name, property)) {
            if (decls[i].important)
                retVal = "important";
            break;
        }

    return 0;
}

result_t CSSStyleDeclaration::setProperty(exlib::string property, exlib::string value,
    exlib::string priority)
{
    // CSSOM: priority must be empty or "important", otherwise the call is a no-op
    if (!priority.empty() && qstricmp(priority.c_str(), "important"))
        return 0;

    // CSS property names are case-insensitive, custom property names are not
    if (!is_custom_property(property))
        exlib::qstrlwr(property);

    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    size_t found = (size_t)-1;
    for (size_t i = 0; i < decls.size(); i++)
        if (name_equals(decls[i].name, property)) {
            found = i;
            break;
        }

    if (value.empty()) {
        if (found != (size_t)-1)
            decls.erase(decls.begin() + found);
    } else if (found != (size_t)-1) {
        decls[found].value = value;
        decls[found].important = !qstricmp(priority.c_str(), "important");
    } else {
        Decl d;
        d.name = property;
        d.value = value;
        d.important = !qstricmp(priority.c_str(), "important");
        decls.push_back(d);
    }

    if (decls.empty())
        return m_element->removeAttribute("style");

    exlib::string out;
    serialize_declarations(decls, out);

    return m_element->setAttribute("style", out);
}

result_t CSSStyleDeclaration::removeProperty(exlib::string property, exlib::string& retVal)
{
    retVal.clear();

    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    size_t found = (size_t)-1;
    for (size_t i = 0; i < decls.size(); i++)
        if (name_equals(decls[i].name, property)) {
            found = i;
            break;
        }

    if (found == (size_t)-1)
        return 0;

    retVal = decls[found].value;
    decls.erase(decls.begin() + found);

    if (decls.empty())
        return m_element->removeAttribute("style");

    exlib::string out;
    serialize_declarations(decls, out);

    return m_element->setAttribute("style", out);
}

result_t CSSStyleDeclaration::_named_getter(exlib::string property, Variant& retVal)
{
    exlib::string name = style_name_to_css(property);

    exlib::string text;
    get_cssText(text);

    std::vector<Decl> decls;
    parse_declarations(text, decls);

    for (size_t i = 0; i < decls.size(); i++)
        if (name_equals(decls[i].name, name)) {
            retVal = decls[i].value;
            return 0;
        }

    // CSSOM returns an empty string for unset (supported) properties
    retVal = "";
    return 0;
}

result_t CSSStyleDeclaration::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = v8::Array::New(isolate->m_isolate);
    return 0;
}

result_t CSSStyleDeclaration::_named_setter(exlib::string property, Variant newVal)
{
    exlib::string value;

    Variant::Type t = newVal.type();
    if (t == Variant::VT_Undefined || t == Variant::VT_Null)
        return removeProperty(style_name_to_css(property), value);

    newVal.toString(value);
    if (value.empty())
        return removeProperty(style_name_to_css(property), value);

    return setProperty(style_name_to_css(property), value, "");
}

result_t CSSStyleDeclaration::_named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal)
{
    exlib::string value;

    removeProperty(style_name_to_css(property), value);

    retVal = v8::True(Isolate::current()->m_isolate);
    return 0;
}

} // namespace fibjs
