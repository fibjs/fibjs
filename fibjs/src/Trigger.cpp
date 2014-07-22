/*
 * Event.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "Fiber.h"
#include "Trigger.h"
#include "ifs/coroutine.h"

namespace fibjs
{

result_t Trigger_base::_new(obj_ptr<Trigger_base> &retVal)
{
    retVal = new Trigger();
    return 0;
}

v8::Local<v8::Array> object_base::GetHiddenArray(const char *k, bool create,
        bool autoDelete)
{
    v8::Local<v8::String> s = v8::String::NewFromUtf8(isolate, k);
    v8::Local<v8::Object> o = wrap();
    v8::Local<v8::Value> es = o->GetHiddenValue(s);
    v8::Local<v8::Array> esa;

    if (es.IsEmpty())
    {
        if (create)
        {
            esa = v8::Array::New(isolate);
            o->SetHiddenValue(s, esa);
        }
    }
    else
        esa = v8::Local<v8::Array> ::Cast(es);

    if (autoDelete)
        o->DeleteHiddenValue(s);

    return esa;
}

inline int putFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func)
{
    int len = esa->Length();
    int i, p = -1;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> v = esa->Get(i);
        if (v->IsFunction())
        {
            if (v->StrictEquals(func))
                return 0;
        }
        else if (p == -1)
            p = i;
    }

    if (p == -1)
        p = len;

    esa->Set(p, func);

    return 1;
}

inline int removeFunction(v8::Local<v8::Array> esa,
                          v8::Local<v8::Function> func)
{
    if (esa.IsEmpty())
        return 0;

    int len = esa->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> v = esa->Get(i);
        if (v->IsFunction())
        {
            if (v->StrictEquals(func))
            {
                esa->Delete(i);
                return -1;
            }
        }
    }

    return 0;
}

inline result_t _map(object_base *o, v8::Local<v8::Object> m,
                     result_t (object_base::*fn)(const char *, v8::Local<v8::Function>))
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int len = ks->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);

        if (!k->IsNumber())
        {
            v8::Local<v8::Value> v = m->Get(k);

            if (v->IsFunction())
                (o->*fn)(*v8::String::Utf8Value(k),
                         v8::Local<v8::Function>::Cast(v));
            else
                return CHECK_ERROR(CALL_E_BADVARTYPE);
        }
    }

    return 0;
}

result_t object_base::on(const char *ev, v8::Local<v8::Function> func)
{
    std::string strKey = "_e_";
    strKey.append(ev);
    m_nTriggers += putFunction(GetHiddenArray(strKey.c_str(), true), func);

    strKey = "_e1_";
    strKey.append(ev);
    m_nTriggers += removeFunction(GetHiddenArray(strKey.c_str()), func);

    return 0;
}

result_t object_base::on(v8::Local<v8::Object> map)
{
    return _map(this, map, &object_base::on);
}

result_t object_base::once(const char *ev, v8::Local<v8::Function> func)
{
    std::string strKey = "_e1_";
    strKey.append(ev);
    m_nTriggers += putFunction(GetHiddenArray(strKey.c_str(), true), func);

    strKey = "_e_";
    strKey.append(ev);
    m_nTriggers += removeFunction(GetHiddenArray(strKey.c_str()), func);

    return 0;
}

result_t object_base::once(v8::Local<v8::Object> map)
{
    return _map(this, map, &object_base::once);
}

result_t object_base::off(const char *ev, v8::Local<v8::Function> func)
{
    if (!m_nTriggers)
        return 0;

    std::string strKey = "_e_";
    strKey.append(ev);
    m_nTriggers += removeFunction(GetHiddenArray(strKey.c_str()), func);

    strKey = "_e1_";
    strKey.append(ev);
    m_nTriggers += removeFunction(GetHiddenArray(strKey.c_str()), func);

    return 0;
}

result_t object_base::off(v8::Local<v8::Object> map)
{
    if (!m_nTriggers)
        return 0;

    return _map(this, map, &object_base::off);
}

inline result_t _fire(v8::Local<v8::Function> func, const v8::FunctionCallbackInfo<v8::Value> &args,
                      int argCount)
{
    obj_ptr<Fiber_base> retVal;
    return JSFiber::New(func, args, 1, retVal);
}

inline result_t _fire(v8::Local<v8::Function> func,
                      v8::Local<v8::Value> *args, int argCount)
{
    obj_ptr<Fiber_base> retVal;
    return JSFiber::New(func, args, argCount, retVal);
}

template<typename T>
result_t fireTrigger(v8::Local<v8::Array> esa, T args, int argCount)
{
    if (esa.IsEmpty())
        return 0;

    result_t hr;
    int len = esa->Length();
    int i;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> func = esa->Get(i);
        if (func->IsFunction())
        {
            hr = _fire(v8::Local<v8::Function>::Cast(func), args, argCount);
            if (hr < 0)
                return hr;
        }
    }

    return 0;
}

result_t object_base::_trigger(const char *ev, v8::Local<v8::Value> *args,
                               int argCount)
{
    extMemory(0);

    if (!m_nTriggers)
        return 0;

    result_t hr;
    std::string strKey = "_e_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenArray(strKey.c_str()), args, argCount);
    if (hr < 0)
        return hr;

    strKey = "_e1_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenArray(strKey.c_str(), false, true), args,
                     argCount);
    if (hr < 0)
        return hr;

    return 0;
}

result_t object_base::trigger(const char *ev, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    extMemory(0);

    if (!m_nTriggers)
        return 0;

    result_t hr;
    std::string strKey = "_e_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenArray(strKey.c_str()), args, 0);
    if (hr < 0)
        return hr;

    strKey = "_e1_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenArray(strKey.c_str(), false, true), args, 0);
    if (hr < 0)
        return hr;

    return 0;
}

}
