/*
 * HttpCollectionTmpl.h
 *
 *  Created on: Jul 11, 2025
 *      Author: lion
 */

#pragma once

#include "utils.h"
#include <vector>
#include <set>
#include <utility>
#include "Iterator.h"

namespace fibjs {

template <typename BaseType, bool case_sensitive = false>
class HttpCollectionTmpl : public BaseType {
public:
    HttpCollectionTmpl(bool string_only)
        : m_string_only(string_only)
    {
        m_map.reserve(16); // reserve space instead of resize
    }

public:
    // HttpCollection_base

    result_t clear()
    {
        m_map.clear();
        return 0;
    }

    result_t first(exlib::string name, Variant& retVal)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];

            if (!cmp_key(_pair.first.c_str(), name.c_str())) {
                retVal = _pair.second;
                return 0;
            }
        }

        return CALL_RETURN_NULL;
    }

    result_t get(exlib::string name, Variant& retVal)
    {
        return first(name, retVal);
    }

    result_t all(exlib::string name, obj_ptr<NObject>& retVal)
    {
        if (!name.empty()) {
            obj_ptr<NArray> list;

            all(name, list);
            retVal = list;
        } else
            all(retVal);

        return 0;
    }

    result_t getAll(exlib::string name, obj_ptr<NArray>& retVal)
    {
        return all(name, retVal);
    }

    result_t append(exlib::string name, Variant value)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

        if (m_string_only && value.type() != Variant::VT_String) {
            exlib::string s;
            value.toString(s);
            value = s;
        }

        m_map.emplace_back(name, value);
        return 0;
    }

    result_t append(v8::Local<v8::Object> map)
    {
        v8::Local<v8::Context> context = map->GetCreationContextChecked();
        JSArray ks = map->GetPropertyNames(context);
        int32_t len = ks->Length();
        int32_t i;
        Isolate* isolate = Isolate::current(context);

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);
            JSValue v = map->Get(context, k);

            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            if (v->IsArray())
                append(isolate->toString(k), v.As<v8::Array>());
            else
                append(isolate->toString(k), (Variant)v);
        }

        return 0;
    }

    result_t append(exlib::string name, v8::Local<v8::Array> values)
    {
        v8::Local<v8::Context> context = values->GetCreationContextChecked();
        int32_t len = values->Length();
        int32_t i;

        for (i = 0; i < len; i++)
            append(name, (Variant)JSValue(values->Get(context, i)));

        return 0;
    }

    result_t append(v8::Local<v8::Array> entries)
    {
        Isolate* isolate = Isolate::current(entries);
        v8::Local<v8::Context> context = isolate->context();
        int32_t len = entries->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            v8::Local<v8::Value> entry = entries->Get(context, i).ToLocalChecked();
            if (!entry->IsArray())
                return CALL_E_BADVARTYPE;

            v8::Local<v8::Array> pair = entry.As<v8::Array>();
            if (pair->Length() != 2)
                return CALL_E_BADVARTYPE;

            exlib::string key = isolate->toString(pair->Get(context, 0).ToLocalChecked());
            Variant value = (Variant)pair->Get(context, 1).ToLocalChecked();

            append(key, value);
        }

        return 0;
    }

    result_t set(exlib::string name, Variant value)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

        remove(name);
        return append(name, value);
    }

    result_t set(v8::Local<v8::Object> map)
    {
        v8::Local<v8::Context> context = map->GetCreationContextChecked();
        JSArray ks = map->GetPropertyNames(context);
        int32_t len = ks->Length();
        int32_t i;
        Isolate* isolate = Isolate::current(context);

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);
            JSValue v = map->Get(context, k);

            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            if (v->IsArray())
                set(isolate->toString(k), v.As<v8::Array>());
            else
                set(isolate->toString(k), (Variant)v);
        }

        return 0;
    }

    result_t has(exlib::string name, bool& retVal)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

        retVal = false;
        for (size_t i = 0; i < m_map.size(); i++)
            if (!cmp_key(m_map[i].first.c_str(), name.c_str())) {
                retVal = true;
                break;
            }

        return 0;
    }

    result_t set(exlib::string name, v8::Local<v8::Array> values)
    {
        v8::Local<v8::Context> context = values->GetCreationContextChecked();
        int32_t len = values->Length();
        int32_t i;

        remove(name);
        for (i = 0; i < len; i++)
            append(name, (Variant)JSValue(values->Get(context, i)));

        return 0;
    }

    result_t remove(exlib::string name)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

        auto it = std::remove_if(m_map.begin(), m_map.end(),
            [&name](const pair& p) {
                return !cmp_key(p.first.c_str(), name.c_str());
            });
        m_map.erase(it, m_map.end());
        return 0;
    }

    result_t _delete(exlib::string name)
    {
        if (name.empty())
            return CALL_E_INVALIDARG;

        return remove(name);
    }

    result_t sort()
    {
        if (!m_map.empty())
            std::sort(m_map.begin(), m_map.end(), [](const pair& a, const pair& b) {
                return a.first < b.first;
            });

        return 0;
    }

    result_t forEach(v8::Local<v8::Function> callback)
    {
        return forEach(callback, BaseType::wrap());
    }

    result_t forEach(v8::Local<v8::Function> callback, v8::Local<v8::Value> thisArg)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];
            v8::Local<v8::Value> key = isolate->NewString(_pair.first);
            v8::Local<v8::Value> value = _pair.second;

            v8::Local<v8::Value> argv[] = { value, key, thisArg };
            v8::Local<v8::Value> result = callback->Call(context, thisArg, 3, argv).FromMaybe(v8::Local<v8::Value>());
            if (result.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }

        return 0;
    }

    result_t keys(obj_ptr<Iterator_base>& retVal)
    {
        retVal = new Iterator(this, [&](size_t index, v8::Local<v8::Value>& retVal) {
            if (index < m_map.size()) {
                Isolate* isolate = Isolate::current();
                retVal = isolate->NewString(m_map[index].first);
            }
        });
        return 0;
    }

    result_t values(obj_ptr<Iterator_base>& retVal)
    {
        retVal = new Iterator(this, [&](size_t index, v8::Local<v8::Value>& retVal) {
            if (index < m_map.size()) {
                retVal = m_map[index].second;
            }
        });
        return 0;
    }

    result_t entries(obj_ptr<Iterator_base>& retVal)
    {
        retVal = new Iterator(this, [&](size_t index, v8::Local<v8::Value>& retVal) {
            if (index < m_map.size()) {
                Isolate* isolate = Isolate::current();
                v8::Local<v8::Array> array = v8::Array::New(isolate->m_isolate);

                pair& _pair = m_map[index];
                array->Set(isolate->context(), 0, isolate->NewString(_pair.first)).IsJust();
                array->Set(isolate->context(), 1, _pair.second).IsJust();
                retVal = array;
            }
        });
        return 0;
    }

    result_t _named_getter(exlib::string property, Variant& retVal)
    {
        int32_t n = 0;
        Variant v;
        v8::Local<v8::Array> a;
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];

            if (!cmp_key(_pair.first.c_str(), property.c_str())) {
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

    result_t _named_enumerator(v8::Local<v8::Array>& retVal)
    {
        int32_t n;
        std::set<exlib::string> name_set;
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();

        retVal = v8::Array::New(isolate->m_isolate);
        for (size_t i = 0, n = 0; i < m_map.size(); i++) {
            exlib::string& name = m_map[i].first;
            if (name_set.insert(name).second)
                retVal->Set(context, n++, isolate->NewString(name)).IsJust();
        }

        return 0;
    }

    result_t _named_setter(exlib::string property, Variant newVal)
    {
        return set(property, newVal);
    }

    result_t _named_deleter(exlib::string property,
        v8::Local<v8::Boolean>& retVal)
    {
        size_t n = m_map.size();
        remove(property);
        return n > m_map.size();
    }

    result_t symbol_iterator(obj_ptr<Iterator_base>& retVal)
    {
        return entries(retVal);
    }

public:
    result_t all(exlib::string name, obj_ptr<NArray>& retVal)
    {
        obj_ptr<NArray> list = new NArray();

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];

            if (!cmp_key(_pair.first.c_str(), name.c_str()))
                list->append(_pair.second);
        }

        retVal = list;
        return 0;
    }

    result_t all(obj_ptr<NObject>& retVal)
    {
        obj_ptr<NObject> map = new NObject();

        map->enable_multi_value();

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];
            map->add(_pair.first, _pair.second);
        }

        retVal = map;
        return 0;
    }

public:
    inline static void decodeURI(const char* url, ssize_t sz, exlib::string& retVal, bool space = false)
    {
        if (sz < 0)
            sz = qstrlen(url);

        if (sz == 0) {
            retVal.resize(0);
            return;
        }

        ssize_t len, l;
        const char* src;
        unsigned char ch;
        char* bstr;
        exlib::string str;

        for (len = 0, src = url, l = sz; l > 0; src++, len++, l--) {
            ch = (unsigned char)*src;
            if (ch == '%' && l > 2 && qisxdigit(src[1]) && qisxdigit(src[2])) {
                src += 2;
                l -= 2;
            } else if ((ch == '%' || ch == '\\') && l > 5
                && (src[1] == 'u' || src[1] == 'U') && qisxdigit(src[2])
                && qisxdigit(src[3]) && qisxdigit(src[4]) && qisxdigit(src[5])) {
                char16_t wch = (qhex(src[2]) << 12) + (qhex(src[3]) << 8)
                    + (qhex(src[4]) << 4) + qhex(src[5]);

                len += utf8_strlen(&wch, 1) - 1;

                src += 5;
                l -= 5;
            }
        }

        str.resize(len);
        bstr = str.data();

        for (len = 0, src = url, l = sz; l > 0; src++, len++, l--) {
            ch = (unsigned char)*src;

            if (ch == '%' && l > 2 && qisxdigit(src[1]) && qisxdigit(src[2])) {
                *bstr++ = (qhex(src[1]) << 4) + qhex(src[2]);
                src += 2;
                l -= 2;
            } else if ((ch == '%' || ch == '\\') && l > 5
                && (src[1] == 'u' || src[1] == 'U') && qisxdigit(src[2])
                && qisxdigit(src[3]) && qisxdigit(src[4]) && qisxdigit(src[5])) {
                char16_t wch = (qhex(src[2]) << 12) + (qhex(src[3]) << 8)
                    + (qhex(src[4]) << 4) + qhex(src[5]);

                bstr += utf_convert(&wch, 1, bstr, 5);

                src += 5;
                l -= 5;
            } else if (space && ch == '+')
                *bstr++ = ' ';
            else
                *bstr++ = ch;
        }

        retVal = str;
    }

    inline static void decodeURI(exlib::string url, exlib::string& retVal, bool space = false)
    {
        decodeURI(url.c_str(), url.length(), retVal, space);
    }

    inline static void encodeURI(const char* url, ssize_t sz, exlib::string& retVal,
        const char* tab)
    {
        static const char* hex = "0123456789ABCDEF";

        if (sz < 0)
            sz = qstrlen(url);

        if (sz == 0) {
            retVal.resize(0);
            return;
        }

        ssize_t len, l;
        const char* src;
        unsigned char ch;
        char* bstr;
        exlib::string str;

        for (len = 0, src = url, l = sz; l > 0; len++, l--) {
            ch = (unsigned char)*src++;
            if (ch < 0x20 || ch >= 0x80 || tab[ch - 0x20] == ' ')
                len += 2;
        }

        str.resize(len);
        bstr = str.data();

        for (src = url, l = sz; l > 0; l--) {
            ch = (unsigned char)*src++;
            if (ch >= 0x20 && ch < 0x80 && tab[ch - 0x20] != ' ')
                *bstr++ = ch;
            else {
                *bstr++ = '%';

                *bstr++ = hex[(ch >> 4) & 15];
                *bstr++ = hex[ch & 15];
            }
        }

        retVal = str;
    }

    inline static void encodeURI(exlib::string url, exlib::string& retVal,
        const char* tab)
    {
        encodeURI(url.c_str(), url.length(), retVal, tab);
    }

public:
    result_t parse(exlib::string& str, const char* sep = "&", const char* eq = "=")
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
                decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strKey, true);
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
                    decodeURI(pstrTemp, (int32_t)(pstr - pstrTemp), strValue, true);
                else
                    strValue.clear();
            }

            if (nSize) {
                nSize -= sep_len;
                pstr += sep_len;
            }

            if (!strKey.empty())
                append(strKey, strValue);
        }

        return 0;
    }

private:
    static int cmp_key(const char* key, const char* key2)
    {
        return case_sensitive ? qstrcmp(key, key2) : qstricmp(key, key2);
    }

public:
    using pair = std::pair<exlib::string, Variant>;
    std::vector<pair> m_map;
    bool m_string_only;
};

}
