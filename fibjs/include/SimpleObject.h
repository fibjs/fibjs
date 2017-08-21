/*
 * SimpleObject.h
 *
 *  Created on: Aug 21, 2017
 *      Author: lion
 */

#ifndef SIMPLE_OBJECT_H_
#define SIMPLE_OBJECT_H_

#include "object.h"

namespace fibjs {

class SimpleObject : public object_base {

public:
    void add(exlib::string key, Variant value)
    {
        m_keys.push_back(key);
        m_values.append(value);
    }

public:
    // object_base
    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

        for (int32_t i = 0; i < m_keys.size(); i++) {
            obj->Set(isolate->NewFromUtf8(m_keys[i]), m_values[i]);
        }

        retVal = obj;
        return 0;
    }

private:
    std::vector<exlib::string> m_keys;
    QuickArray<Variant> m_values;
};

} /* namespace fibjs */

#endif