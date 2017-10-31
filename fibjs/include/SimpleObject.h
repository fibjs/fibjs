/*
 * SimpleObject.h
 *
 *  Created on: Aug 21, 2017
 *      Author: lion
 */

#ifndef SIMPLE_OBJECT_H_
#define SIMPLE_OBJECT_H_

#include <algorithm>

namespace fibjs {

class NObject : public object_base {

public:
    void add(exlib::string key, Variant value)
    {
        m_keys.push_back(key);
        m_values.push_back(value);
    }

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Object> obj;

        obj = v8::Object::New(isolate->m_isolate);

        for (int32_t i = 0; i < (int32_t)m_keys.size(); i++)
            obj->Set(isolate->NewString(m_keys[i]), m_values[i]);

        retVal = obj;
        return 0;
    }

protected:
    std::vector<exlib::string> m_keys;
    std::vector<Variant> m_values;
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
        v8::Local<v8::Object> obj;
        v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
        obj = arr;

        for (int32_t i = 0; i < (int32_t)m_array.size(); i++)
            arr->Set(i, m_array[i]);

        for (int32_t i = 0; i < (int32_t)m_keys.size(); i++)
            obj->Set(isolate->NewString(m_keys[i]), m_values[i]);

        retVal = obj;
        return 0;
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