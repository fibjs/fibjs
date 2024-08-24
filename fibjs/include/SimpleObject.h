/*
 * SimpleObject.h
 *
 *  Created on: Aug 21, 2017
 *      Author: lion
 */

#pragma once

#include <unordered_map>
#include <algorithm>

namespace fibjs {

class NObject : public object_base {
    DECLARE_CLASS(NObject);

public:
    class Value {
    public:
        Value(std::unordered_map<exlib::string, int32_t>::iterator pos, Variant val)
            : m_pos(pos)
            , m_val(val)
        {
        }

    public:
        std::unordered_map<exlib::string, int32_t>::iterator m_pos;
        Variant m_val;
    };

public:
    void add(exlib::string key, Variant value);
    result_t add(v8::Local<v8::Object> m)
    {
        v8::Local<v8::Context> context = m->GetCreationContextChecked();
        JSArray ks = m->GetPropertyNames(context);
        int32_t len = ks->Length();
        int32_t i;
        Isolate* isolate = holder();

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);
            add(isolate->toString(k), JSValue(m->Get(context, k)));
        }

        return 0;
    }

    result_t get(exlib::string key, Variant& retVal)
    {
        std::unordered_map<exlib::string, int32_t>::iterator it = m_keys.find(key);

        if (it == m_keys.end())
            return CALL_RETURN_NULL;

        retVal = m_values[it->second].m_val;
        return 0;
    }

    result_t remove(exlib::string key)
    {
        std::unordered_map<exlib::string, int32_t>::iterator it = m_keys.find(key);

        if (it == m_keys.end())
            return CALL_RETURN_NULL;

        int32_t deleted_pos = it->second;
        m_values.erase(m_values.begin() + deleted_pos);
        m_keys.erase(it);

        for (auto& i : m_keys) {
            if (i.second > deleted_pos)
                i.second--;
        }

        return 0;
    }

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        v8::Local<v8::Object> obj;

        valueOf(obj);
        retVal = obj;

        return 0;
    }

    result_t valueOf(v8::Local<v8::Object>& retVal)
    {
        Isolate* isolate = holder();
        v8::Local<v8::Context> context = isolate->context();

        if (retVal.IsEmpty())
            retVal = v8::Object::New(isolate->m_isolate);

        for (int32_t i = 0; i < (int32_t)m_values.size(); i++) {
            Value& v = m_values[i];
            retVal->Set(context, isolate->NewString(v.m_pos->first), v.m_val).IsJust();
        }

        return 0;
    }

    void enable_multi_value()
    {
        m_multi_value = true;
    }

public:
    std::unordered_map<exlib::string, int32_t> m_keys;
    std::vector<Value> m_values;
    bool m_multi_value = false;
};

class NArray : public NObject {
    DECLARE_CLASS(NArray);

public:
    void resize(size_t sz)
    {
        m_array.resize(sz);
    }

    void append(Variant value)
    {
        m_array.push_back(value);
    }

    template <typename T>
    result_t append_array(v8::Local<v8::Array> jsarray)
    {
        result_t hr;
        int32_t len = jsarray->Length();

        Isolate* isolate = holder();

        for (int32_t i = 0; i < len; i++) {
            T v;

            hr = GetConfigValue(isolate, jsarray, i, v);
            if (hr < 0)
                return hr;

            append(v);
        }

        return 0;
    }

    int32_t length() const
    {
        return (int32_t)m_array.size();
    }

    result_t _indexed_getter(int32_t idx, Variant& retVal) const
    {
        retVal = m_array[idx];
        return 0;
    }

    result_t sort()
    {
        std::sort(m_array.begin(), m_array.end(), compare1);
        return 0;
    }

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        v8::Local<v8::Array> arr;

        valueOf(arr);
        retVal = arr;

        return 0;
    }

    result_t valueOf(v8::Local<v8::Object>& retVal)
    {
        v8::Local<v8::Array> arr;

        valueOf(arr);
        retVal = arr;

        return 0;
    }

    result_t valueOf(v8::Local<v8::Array>& retVal)
    {
        Isolate* isolate = holder();
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
        v8::Local<v8::Object> o = arr;

        for (int32_t i = 0; i < (int32_t)m_array.size(); i++)
            arr->Set(context, i, m_array[i]).IsJust();

        retVal = arr;

        return NObject::valueOf(o);
    }

private:
    static bool compare1(const Variant& v1, const Variant& v2)
    {
        exlib::string s1, s2;

        v1.toString(s1);
        v2.toString(s2);

        return s1.compare(s2) < 0;
    }

public:
    std::vector<Variant> m_array;
};

class NMap : public NObject {
    DECLARE_CLASS(NMap);

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        v8::Local<v8::Object> obj;

        result_t hr = valueOf(obj);
        if (hr < 0)
            return hr;

        retVal = obj;

        return 0;
    }

    result_t valueOf(v8::Local<v8::Object>& retVal)
    {
        Isolate* isolate = holder();
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Map> obj;

        obj = v8::Map::New(isolate->m_isolate);
        retVal = obj;

        for (int32_t i = 0; i < (int32_t)m_values.size(); i++) {
            Value& v = m_values[i];
            obj->Set(context, isolate->NewString(v.m_pos->first), v.m_val).IsEmpty();
        }

        return 0;
    }
};

class NType : public object_base {

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        v8::Local<v8::Object> obj;

        result_t hr = valueOf(obj);
        if (hr < 0)
            return hr;

        retVal = obj;

        return 0;
    }

    result_t valueOf(v8::Local<v8::Object>& retVal)
    {
        Isolate* isolate = holder();
        v8::Local<v8::Object> obj;

        obj = v8::Object::New(isolate->m_isolate);

        fillMembers(isolate, obj);

        retVal = obj;
        return 0;
    }

public:
    virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
    {
    }

    virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
    {
    }
};

inline ClassInfo& NObject::class_info()
{
    static ClassData s_cd = {
        "NObject", false, NULL, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline ClassInfo& NArray::class_info()
{
    static ClassData s_cd = {
        "NArray", false, NULL, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &NObject::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline ClassInfo& NMap::class_info()
{
    static ClassData s_cd = {
        "NMap", false, NULL, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void NObject::add(exlib::string key, Variant value)
{
    std::unordered_map<exlib::string, int32_t>::iterator it = m_keys.find(key);

    if (it == m_keys.end()) {
        it = m_keys.insert(std::pair<exlib::string, int32_t>(key, (int32_t)m_values.size())).first;
        m_values.push_back(Value(it, value));
    } else {
        NObject::Value& v = m_values[it->second];
        if (m_multi_value && !v.m_val.isUndefined()) {
            obj_ptr<NArray> list = NArray::getInstance(v.m_val.object());

            if (!list) {
                list = new NArray();
                list->append(v.m_val);
                v.m_val = list;
            }

            list->append(value);
        } else
            v.m_val = value;
    }
}

} /* namespace fibjs */
