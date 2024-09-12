/*
 * JSType.h
 *
 *  Created on: Sep 25, 2019
 *      Author: lion
 */

#pragma once

#include <v8/include/v8.h>
#include <Isolate.h>

namespace fibjs {

class JSValue : public v8::Local<v8::Value> {
public:
    JSValue()
        : m_empty(true)
    {
    }

    template <class S>
    JSValue(const v8::Local<S>& v)
        : m_empty(true)
    {
        operator=(v);
    }

    JSValue(v8::MaybeLocal<v8::Value> v)
    {
        operator=(v);
    }

    template <class S>
    v8::Local<v8::Value>& operator=(const v8::Local<S>& v)
    {
        if (v.IsEmpty()) {
            m_empty = true;
            Isolate* isolate = Isolate::current();
            return v8::Local<v8::Value>::operator=(v8::Undefined(isolate->m_isolate));
        }

        m_empty = false;
        return v8::Local<v8::Value>::operator=(v);
    }

    v8::Local<v8::Value>& operator=(v8::MaybeLocal<v8::Value> v)
    {
        if (v.IsEmpty()) {
            m_empty = true;
            Isolate* isolate = Isolate::current();
            return v8::Local<v8::Value>::operator=(v8::Undefined(isolate->m_isolate));
        }

        m_empty = false;
        return v8::Local<v8::Value>::operator=(v.FromMaybe(v8::Local<v8::Value>()));
    }

    bool IsEmpty() const
    {
        return m_empty;
    }

private:
    bool m_empty;
};

class JSArray : public v8::Local<v8::Array> {
public:
    JSArray()
    {
    }

    JSArray(const v8::Local<v8::Array>& v)
    {
        operator=(v);
    }

    JSArray(v8::MaybeLocal<v8::Array> v)
    {
        operator=(v);
    }

    v8::Local<v8::Array>& operator=(const v8::Local<v8::Array>& v)
    {
        if (v.IsEmpty()) {
            Isolate* isolate = Isolate::current();
            return v8::Local<v8::Array>::operator=(v8::Array::New(isolate->m_isolate));
        }
        return v8::Local<v8::Array>::operator=(v);
    }

    v8::Local<v8::Array>& operator=(v8::MaybeLocal<v8::Array> v)
    {
        if (v.IsEmpty()) {
            Isolate* isolate = Isolate::current();
            return v8::Local<v8::Array>::operator=(v8::Array::New(isolate->m_isolate));
        }
        return v8::Local<v8::Array>::operator=(v.FromMaybe(v8::Local<v8::Array>()));
    }
};

class JSFunction : public v8::Local<v8::Function> {
public:
    JSFunction()
    {
    }

    JSFunction(v8::Local<v8::Function> func)
        : v8::Local<v8::Function>(func)
    {
    }

public:
    v8::Local<v8::Value> Call(v8::Local<v8::Value> recv, int argc, v8::Local<v8::Value> argv[])
    {
        Isolate* isolate = Isolate::current(*this);
        v8::Local<v8::Context> context = isolate->context();

        return isolate->await((*this)->Call(context, recv, argc, argv).FromMaybe(v8::Local<v8::Value>()));
    }
};

}
