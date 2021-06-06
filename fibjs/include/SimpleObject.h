/*
 * SimpleObject.h
 *
 *  Created on: Aug 21, 2017
 *      Author: lion
 */

#pragma once

#include <map>
#include <algorithm>

namespace fibjs {

class NObject : public object_base {
    DECLARE_CLASS(NObject);

public:
    class Value {
    public:
        Value(std::map<exlib::string, int32_t>::iterator pos, Variant val)
            : m_pos(pos)
            , m_val(val)
        {
        }

    public:
        std::map<exlib::string, int32_t>::iterator m_pos;
        VariantEx m_val;
    };

public:
    NObject(bool multi_value)
        : m_multi_value(multi_value)
    {
    }

public:
    void add(exlib::string key, Variant value);
    result_t add(v8::Local<v8::Object> m)
    {
        JSArray ks = m->GetPropertyNames(m->CreationContext());
        int32_t len = ks->Length();
        int32_t i;
        Isolate* isolate = holder();

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(i);
            add(isolate->toString(k), m->Get(k));
        }

        return 0;
    }

    result_t get(exlib::string key, Variant& retVal)
    {
        std::map<exlib::string, int32_t>::iterator it = m_keys.find(key);

        if (it == m_keys.end())
            return CALL_RETURN_NULL;

        retVal = m_values[it->second].m_val;
        return 0;
    }

    result_t remove(exlib::string key)
    {
        std::map<exlib::string, int32_t>::iterator it = m_keys.find(key);

        if (it == m_keys.end())
            return CALL_RETURN_NULL;

        m_values[it->second].m_val.clear();
        return 0;
    }

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Object> obj;

        if (retVal.IsEmpty()) {
            obj = v8::Object::New(isolate->m_isolate);
            retVal = obj;
        } else
            obj = v8::Local<v8::Object>::Cast(retVal);

        for (int32_t i = 0; i < (int32_t)m_values.size(); i++) {
            Value& v = m_values[i];
            obj->Set(isolate->NewString(v.m_pos->first), v.m_val);
        }

        return 0;
    }

public:
    std::map<exlib::string, int32_t> m_keys;
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

    int32_t length()
    {
        return (int32_t)m_array.size();
    }

    result_t _indexed_getter(int32_t idx, Variant& retVal)
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
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);

        for (int32_t i = 0; i < (int32_t)m_array.size(); i++)
            arr->Set(i, m_array[i]);

        retVal = arr;

        return NObject::valueOf(retVal);
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
    std::vector<VariantEx> m_array;
};

class NType : public object_base {

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = Isolate::current();
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

inline void NObject::add(exlib::string key, Variant value)
{
    std::map<exlib::string, int32_t>::iterator it = m_keys.find(key);

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
