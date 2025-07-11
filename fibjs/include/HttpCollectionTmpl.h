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

namespace fibjs {

template <typename BaseType>
class HttpCollectionTmpl : public BaseType {
public:
    HttpCollectionTmpl(bool string_only)
        : m_count(0)
        , m_string_only(string_only)
    {
        m_map.resize(16);
    }

public:
    // HttpCollection_base

    result_t clear()
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

    result_t first(exlib::string name, Variant& retVal)
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

    result_t add(exlib::string name, Variant value)
    {
        if (m_map.size() < m_count + 1)
            m_map.resize(m_count + 1);

        if (m_string_only && value.type() != Variant::VT_String) {
            exlib::string s;
            value.toString(s);
            value = s;
        }

        m_map[m_count] = pair(name, value);
        m_count++;

        return 0;
    }

    result_t add(v8::Local<v8::Object> map)
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
                add(isolate->toString(k), v.As<v8::Array>());
            else
                add(isolate->toString(k), (Variant)v);
        }

        return 0;
    }

    result_t add(exlib::string name, v8::Local<v8::Array> values)
    {
        v8::Local<v8::Context> context = values->GetCreationContextChecked();
        int32_t len = values->Length();
        int32_t i;

        for (i = 0; i < len; i++)
            add(name, (Variant)JSValue(values->Get(context, i)));

        return 0;
    }

    result_t set(exlib::string name, Variant value)
    {
        remove(name);
        return add(name, value);
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
        size_t i;

        retVal = false;
        for (i = 0; i < m_count; i++)
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
            add(name, (Variant)JSValue(values->Get(context, i)));

        return 0;
    }

    result_t remove(exlib::string name)
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

    result_t _delete(exlib::string name)
    {
        return remove(name);
    }

    result_t sort()
    {
        if (m_count)
            std::sort(m_map.begin(), m_map.begin() + m_count, [](pair& a, pair& b) {
                return a.first < b.first;
            });

        return 0;
    }

    result_t keys(obj_ptr<NArray>& retVal)
    {
        obj_ptr<NArray> _keys = new NArray();
        size_t i;

        for (i = 0; i < m_count; i++)
            _keys->append(m_map[i].first);

        retVal = _keys;

        return 0;
    }

    result_t values(obj_ptr<NArray>& retVal)
    {
        obj_ptr<NArray> _keys = new NArray();
        size_t i;

        for (i = 0; i < m_count; i++)
            _keys->append(m_map[i].second);

        retVal = _keys;
        return 0;
    }

    result_t _named_getter(exlib::string property, Variant& retVal)
    {
        size_t i;
        int32_t n = 0;
        Variant v;
        v8::Local<v8::Array> a;
        Isolate* isolate = Isolate::current();
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

    result_t _named_enumerator(v8::Local<v8::Array>& retVal)
    {
        size_t i;
        int32_t n;
        std::set<exlib::string> name_set;
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();

        retVal = v8::Array::New(isolate->m_isolate);
        for (i = 0, n = 0; i < m_count; i++) {
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
        size_t n = m_count;
        remove(property);
        return n > m_count;
    }

public:
    result_t all(exlib::string name, obj_ptr<NArray>& retVal)
    {
        obj_ptr<NArray> list = new NArray();
        size_t i;

        for (i = 0; i < m_count; i++) {
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
        size_t i;

        map->enable_multi_value();

        for (i = 0; i < m_count; i++) {
            pair& _pair = m_map[i];
            map->add(_pair.first, _pair.second);
        }

        retVal = map;
        return 0;
    }

public:
    using pair = std::pair<exlib::string, Variant>;
    std::vector<pair> m_map;
    bool m_string_only;
    size_t m_count;
};

}
