/*
 * SimpleObject.h
 *
 *  Created on: Aug 21, 2017
 *      Author: lion
 */

#ifndef SIMPLE_OBJECT_H_
#define SIMPLE_OBJECT_H_

#include <map>
#include <algorithm>

namespace fibjs {

class NObject : public object_base {

public:
    void add(exlib::string key, Variant value)
    {
        std::map<exlib::string, Variant>::iterator it = m_datas.find(key);

        if (it == m_datas.end())
            m_datas.insert(std::pair<exlib::string, VariantEx>(key, value));
        else
            it->second = value;
    }

    result_t add(v8::Local<v8::Object> m)
    {
        v8::Local<v8::Array> ks = m->GetPropertyNames();
        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            v8::Local<v8::Value> k = ks->Get(i);
            add(*v8::String::Utf8Value(k), m->Get(k));
        }

        return 0;
    }

    result_t get(exlib::string key, Variant& retVal)
    {
        std::map<exlib::string, Variant>::iterator it = m_datas.find(key);

        if (it == m_datas.end())
            return CALL_RETURN_NULL;

        retVal = it->second;
        return 0;
    }

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = Isolate::current();
        std::map<exlib::string, Variant>::iterator iter;
        v8::Local<v8::Object> obj;

        if (retVal.IsEmpty()) {
            obj = v8::Object::New(isolate->m_isolate);
            retVal = obj;
        } else
            obj = v8::Local<v8::Object>::Cast(retVal);

        for (iter = m_datas.begin(); iter != m_datas.end(); iter++)
            obj->Set(isolate->NewString(iter->first), iter->second);

        return 0;
    }

public:
    std::map<exlib::string, Variant> m_datas;
};

class NArray : public NObject {
public:
    void resize(size_t sz)
    {
        m_array.resize(sz);
    }

    void append(Variant value)
    {
        m_array.push_back(value);
    }

    result_t get_length(int32_t& retVal)
    {
        retVal = (int32_t)m_array.size();
        return 0;
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

protected:
    std::vector<Variant> m_array;
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

} /* namespace fibjs */

#endif