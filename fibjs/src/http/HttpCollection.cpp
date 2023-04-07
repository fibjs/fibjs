/*
 * HttpCollection.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpCollection.h"
#include "Url.h"
#include <string.h>
#include <set>

namespace fibjs {

size_t HttpCollection::size()
{
    size_t sz = 0;
    size_t i;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];
        sz += _pair.first.length() + _pair.second.length() + 4;
    }

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
    size_t i;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];
        exlib::string& n = _pair.first;
        exlib::string& v = _pair.second;

        cp(buf, sz, pos, n.c_str(), n.length());
        cp(buf, sz, pos, ": ", 2);
        cp(buf, sz, pos, v.c_str(), v.length());
        cp(buf, sz, pos, "\r\n", 2);
    }

    return pos;
}

result_t HttpCollection::clear()
{
    size_t i;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];
        _pair.first.clear();
        _pair.second.clear();
    }

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
    size_t i;

    retVal = false;
    for (i = 0; i < m_count; i++)
        if (!qstricmp(m_map[i].first.c_str(), name.c_str())) {
            retVal = true;
            break;
        }

    return 0;
}

result_t HttpCollection::first(exlib::string name, Variant& retVal)
{
    size_t i;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];

        if (!qstricmp(_pair.first.c_str(), name.c_str())) {
            retVal = _pair.second;
            return 0;
        }
    }

    return CALL_RETURN_NULL;
}

result_t HttpCollection::get(exlib::string name, Variant& retVal)
{
    return first(name, retVal);
}

result_t HttpCollection::all(exlib::string name, obj_ptr<NObject>& retVal)
{
    if (!name.empty()) {
        obj_ptr<NArray> list;

        all(name, list);
        retVal = list;
    } else
        all(retVal);

    return 0;
}

result_t HttpCollection::add(exlib::string name, Variant value)
{
    exlib::string s;

    value.toString(s);
    return add(name, s);
}

result_t HttpCollection::add(v8::Local<v8::Object> map)
{
    v8::Local<v8::Context> context = map->GetCreationContextChecked();
    JSArray ks = map->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    Isolate* isolate = holder();

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(context, i);
        JSValue v = map->Get(context, k);

        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (v->IsArray())
            add(isolate->toString(k), v8::Local<v8::Array>::Cast(v));
        else
            add(isolate->toString(k), (Variant)v);
    }

    return 0;
}

result_t HttpCollection::add(exlib::string name, v8::Local<v8::Array> values)
{
    v8::Local<v8::Context> context = values->GetCreationContextChecked();
    int32_t len = values->Length();
    int32_t i;

    for (i = 0; i < len; i++)
        add(name, (Variant)JSValue(values->Get(context, i)));

    return 0;
}

result_t HttpCollection::set(exlib::string name, Variant value)
{
    remove(name);
    return add(name, value);
}

result_t HttpCollection::set(v8::Local<v8::Object> map)
{
    v8::Local<v8::Context> context = map->GetCreationContextChecked();
    JSArray ks = map->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    Isolate* isolate = holder();

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(context, i);
        JSValue v = map->Get(context, k);

        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        if (v->IsArray())
            set(isolate->toString(k), v8::Local<v8::Array>::Cast(v));
        else
            set(isolate->toString(k), (Variant)v);
    }

    return 0;
}

result_t HttpCollection::set(exlib::string name, v8::Local<v8::Array> values)
{
    v8::Local<v8::Context> context = values->GetCreationContextChecked();
    int32_t len = values->Length();
    int32_t i;

    remove(name);
    for (i = 0; i < len; i++)
        add(name, (Variant)JSValue(values->Get(context, i)));

    return 0;
}

result_t HttpCollection::remove(exlib::string name)
{
    size_t i;
    int32_t p = 0;

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];

        if (qstricmp(_pair.first.c_str(), name.c_str())) {
            if (i != p)
                m_map[p] = _pair;

            p++;
        }
    }

    m_count = p;

    return 0;
}

result_t HttpCollection::_delete(exlib::string name)
{
    return remove(name);
}

result_t HttpCollection::sort()
{
    if (m_count)
        std::sort(m_map.begin(), m_map.begin() + m_count, [](pair& a, pair& b) {
            return a.first < b.first;
        });

    return 0;
}

result_t HttpCollection::keys(obj_ptr<NArray>& retVal)
{
    obj_ptr<NArray> _keys = new NArray();
    size_t i;

    for (i = 0; i < m_count; i++)
        _keys->append(m_map[i].first);

    retVal = _keys;

    return 0;
}

result_t HttpCollection::values(obj_ptr<NArray>& retVal)
{
    obj_ptr<NArray> _keys = new NArray();
    size_t i;

    for (i = 0; i < m_count; i++)
        _keys->append(m_map[i].second);

    retVal = _keys;
    return 0;
}

result_t HttpCollection::_named_getter(exlib::string property, Variant& retVal)
{
    size_t i;
    int32_t n = 0;
    Variant v;
    v8::Local<v8::Array> a;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    for (i = 0; i < m_count; i++) {
        pair& _pair = m_map[i];

        if (!qstricmp(_pair.first.c_str(), property.c_str())) {
            if (n == 0) {
                v = _pair.second;
                n = 1;
            } else {
                if (n == 1) {
                    a = v8::Array::New(isolate->m_isolate);
                    a->Set(context, 0, v).IsJust();
                    v = a;
                }

                Variant t = _pair.second;
                a->Set(context, n++, t).IsJust();
            }
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
    size_t i;
    int32_t n;
    std::set<exlib::string> name_set;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    retVal = v8::Array::New(isolate->m_isolate);
    for (i = 0, n = 0; i < m_count; i++) {
        exlib::string& name = m_map[i].first;
        if (name_set.insert(name).second)
            retVal->Set(context, n++, isolate->NewString(name)).IsJust();
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
    size_t n = m_count;
    remove(property);
    return n > m_count;
}

} /* namespace fibjs */
