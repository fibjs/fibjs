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

template <typename BaseType>
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

            if (!qstricmp(_pair.first.c_str(), name.c_str())) {
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
            if (!qstricmp(m_map[i].first.c_str(), name.c_str())) {
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
                return !qstricmp(p.first.c_str(), name.c_str());
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

            if (!qstricmp(_pair.first.c_str(), name.c_str()))
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
    size_t size()
    {
        size_t sz = 0;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];
            sz += _pair.first.length() + _pair.second.string().length() + 4;
        }

        return sz;
    }

    void cp(char* buf, size_t sz, size_t& pos, const char* str, size_t szStr)
    {
        buf += pos;

        pos += szStr;
        if (pos > sz) {
            szStr -= pos - sz;
            pos = sz;
        }

        memcpy(buf, str, szStr);
    }

    size_t getData(char* buf, size_t sz)
    {
        size_t pos = 0;

        for (size_t i = 0; i < m_map.size(); i++) {
            pair& _pair = m_map[i];
            exlib::string& n = _pair.first;
            exlib::string v = _pair.second.string();

            cp(buf, sz, pos, n.c_str(), n.length());
            cp(buf, sz, pos, ": ", 2);
            cp(buf, sz, pos, v.c_str(), v.length());
            cp(buf, sz, pos, "\r\n", 2);
        }

        return pos;
    }

public:
    using pair = std::pair<exlib::string, Variant>;
    std::vector<pair> m_map;
    bool m_string_only;
};

}
