/*
 * HttpCollection.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpCollection.h"
#include "List.h"
#include "Map.h"
#include "Url.h"
#include <string.h>
#include <set>

namespace fibjs {

size_t HttpCollection::size()
{
    size_t sz = 0;
    int32_t i;

    for (i = 0; i < m_count; i++)
        sz += m_names[i].length() + m_values[i].length() + 4;

    return sz;
}

inline void cp(char* buf, size_t sz, size_t& pos, const char* str, size_t szStr)
{
    buf += pos;

    pos += szStr;
    if (pos > sz) {
        szStr -= pos - sz;
        pos = sz;
    }

    memcpy(buf, str, szStr);
}

size_t HttpCollection::getData(char* buf, size_t sz)
{
    size_t pos = 0;
    int32_t i;

    for (i = 0; i < m_count; i++) {
        exlib::string& n = m_names[i];
        exlib::string& v = m_values[i];

        cp(buf, sz, pos, n.c_str(), n.length());
        cp(buf, sz, pos, ": ", 2);
        cp(buf, sz, pos, v.c_str(), v.length());
        cp(buf, sz, pos, "\r\n", 2);
    }

    return pos;
}

result_t HttpCollection::clear()
{
    m_count = 0;
    return 0;
}

result_t HttpCollection::parse(exlib::string& str, const char* sep, const char* eq)
{
    const char* pstr = str.c_str();
    int32_t nSize = (int32_t)str.length();
    const char* pstrTemp;
    exlib::string strKey, strValue;
    int32_t sep_len = (int32_t)qstrlen(sep);
    int32_t eq_len = (int32_t)qstrlen(eq);
    bool found_eq;

    while (nSize) {
        pstrTemp = pstr;
        found_eq = false;

        while (nSize) {
            if (!qstrcmp(pstr, sep, sep_len))
                break;

            if (!qstrcmp(pstr, eq, eq_len)) {
                found_eq = true;
                break;
            }

            pstr++;
            nSize--;
        }

        if (pstr > pstrTemp)
            Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strKey, true);
        else
            strKey.clear();

        if (nSize && found_eq) {
            nSize -= eq_len;
            pstr += eq_len;
        }

        pstrTemp = pstr;
        while (nSize && qstrcmp(pstr, sep, sep_len)) {
            pstr++;
            nSize--;
        }

        if (!strKey.empty()) {
            if (pstr > pstrTemp)
                Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strValue, true);
            else
                strValue.clear();
        }

        if (nSize) {
            nSize -= sep_len;
            pstr += sep_len;
        }

        if (!strKey.empty())
            add(strKey, strValue);
    }

    return 0;
}

result_t HttpCollection::parseCookie(exlib::string& str)
{
    const char* pstr = str.c_str();
    int32_t nSize = (int32_t)str.length();
    const char* pstrTemp;
    exlib::string strKey, strValue;

    while (nSize) {
        while (nSize && *pstr == ' ') {
            pstr++;
            nSize--;
        }

        pstrTemp = pstr;
        while (nSize && *pstr != '=' && *pstr != ';') {
            pstr++;
            nSize--;
        }

        if (pstr > pstrTemp)
            Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strKey, true);
        else
            strKey.clear();

        if (nSize && *pstr == '=') {
            nSize--;
            pstr++;
        }

        pstrTemp = pstr;
        while (nSize && *pstr != ';') {
            pstr++;
            nSize--;
        }

        if (!strKey.empty()) {
            if (pstr > pstrTemp)
                Url::decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strValue, true);
            else
                strValue.clear();
        }

        if (nSize) {
            nSize--;
            pstr++;
        }

        if (!strKey.empty())
            add(strKey, strValue);
    }

    return 0;
}

result_t HttpCollection::has(exlib::string name, bool& retVal)
{
    int32_t i;

    retVal = false;
    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str())) {
            retVal = true;
            break;
        }

    return 0;
}

result_t HttpCollection::first(exlib::string name, Variant& retVal)
{
    int32_t i;

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str())) {
            retVal = m_values[i];
            return 0;
        }

    return CALL_RETURN_NULL;
}

result_t HttpCollection::all(exlib::string name, obj_ptr<List_base>& retVal)
{
    obj_ptr<List> list;
    int32_t i;

    list = new List();

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str()))
            list->append(m_values[i]);

    retVal = list;
    return 0;
}

result_t HttpCollection::add(exlib::string name, Variant value)
{
    exlib::string s;

    value.toString(s);

    m_names[m_count] = name;
    m_values[m_count] = s;
    m_count++;

    return 0;
}

result_t HttpCollection::add(Map_base* map)
{
    return ((Map*)map)->map(this, &HttpCollection::add);
}

result_t HttpCollection::set(exlib::string name, Variant value)
{
    int32_t i;
    bool bFound = false;

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), name.c_str())) {
            exlib::string s;

            value.toString(s);

            m_values[i] = s;
            bFound = true;
            break;
        }

    if (bFound) {
        int32_t p = ++i;

        for (; i < m_count; i++)
            if (qstricmp(m_names[i].c_str(), name.c_str())) {
                if (i != p) {
                    m_names[p] = m_names[i];
                    m_values[p] = m_values[i];
                }

                p++;
            }

        m_count = p;
    } else
        return add(name, value);

    return 0;
}

result_t HttpCollection::set(Map_base* map)
{
    return ((Map*)map)->map(this, &HttpCollection::set);
}

result_t HttpCollection::remove(exlib::string name)
{
    int32_t i;
    int32_t p = 0;

    for (i = 0; i < m_count; i++)
        if (qstricmp(m_names[i].c_str(), name.c_str())) {
            if (i != p) {
                m_names[p] = m_names[i];
                m_values[p] = m_values[i];
            }

            p++;
        }

    m_count = p;

    return 0;
}

result_t HttpCollection::_named_getter(exlib::string property, Variant& retVal)
{
    int32_t i;
    int32_t n = 0;
    Variant v;
    v8::Local<v8::Array> a;

    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_names[i].c_str(), property.c_str())) {
            if (n == 0) {
                v = m_values[i];
                n = 1;
            } else {
                if (n == 1) {
                    Isolate* isolate = holder();
                    a = v8::Array::New(isolate->m_isolate);
                    a->Set(0, v);
                    v = a;
                }

                Variant t = m_values[i];
                a->Set(n++, t);
            }
        }

    if (n > 0) {
        retVal = v;
        return 0;
    }

    return CALL_RETURN_NULL;
}

result_t HttpCollection::_named_enumerator(v8::Local<v8::Array>& retVal)
{
    int32_t i, n;
    std::set<exlib::string> name_set;
    Isolate* isolate = holder();

    retVal = v8::Array::New(isolate->m_isolate);
    for (i = 0, n = 0; i < m_count; i++) {
        exlib::string& name = m_names[i];
        if (name_set.insert(name).second)
            retVal->Set(n++, isolate->NewString(name));
    }

    return 0;
}

result_t HttpCollection::_named_setter(exlib::string property, Variant newVal)
{
    return set(property, newVal);
}

result_t HttpCollection::_named_deleter(exlib::string property,
    v8::Local<v8::Boolean>& retVal)
{
    int32_t n = m_count;
    remove(property);
    return n > m_count;
}

} /* namespace fibjs */
