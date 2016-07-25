/*
 * Event.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "object.h"
#include "Fiber.h"
#include "Trigger.h"
#include "ifs/coroutine.h"
#include "QuickArray.h"

namespace fibjs
{

result_t Trigger_base::_new(obj_ptr<Trigger_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new Trigger();
    return 0;
}

v8::Local<v8::Array> object_base::GetHiddenList(const char *k, bool create,
        bool autoDelete)
{
    Isolate* isolate = holder();

    v8::Local<v8::Value> es = GetPrivate(k);
    v8::Local<v8::Array> esa;

    if (es->IsUndefined())
    {
        if (create)
        {
            esa = v8::Array::New(isolate->m_isolate);
            SetPrivate(k, esa);
        }
    }
    else
        esa = v8::Local<v8::Array>::Cast(es);

    if (autoDelete)
        DeletePrivate(k);

    return esa;
}

inline int32_t putFunction(Isolate* isolate, v8::Local<v8::Array> esa, v8::Local<v8::Function> func)
{
    int32_t len = esa->Length();
    int32_t i;
    int32_t append = len;

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> v = esa->Get(i);
        if (append == len && v->IsUndefined())
            append = i;
        else if (v->Equals(func))
            return 0;
    }

    esa->Set(append, func);

    return 1;
}

inline int32_t removeFunction(Isolate* isolate, v8::Local<v8::Array> esa,
                              v8::Local<v8::Function> func)
{
    if (esa.IsEmpty())
        return 0;

    int32_t len = esa->Length();
    int32_t i;

    for (i = 0; i < len; i ++)
    {
        v8::Local<v8::Value> v = esa->Get(i);
        if (v->Equals(func))
        {
            esa->Delete(i);
            return 1;
        }
    }

    return 0;
}

inline result_t _map(object_base *o, v8::Local<v8::Object> m,
                     result_t (object_base::*fn)(exlib::string , v8::Local<v8::Function>, int32_t &),
                     int32_t &retVal)
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int32_t len = ks->Length();
    int32_t i;

    retVal = 0;
    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> k = ks->Get(i);

        if (!k->IsNumber())
        {
            v8::Local<v8::Value> v = m->Get(k);

            if (v->IsFunction())
            {
                int32_t n = 0;
                (o->*fn)(*v8::String::Utf8Value(k),
                         v8::Local<v8::Function>::Cast(v), n);
                retVal += n;
            }
            else
                return CHECK_ERROR(CALL_E_BADVARTYPE);
        }
    }

    return 0;
}

result_t object_base::on(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    retVal = 0;
    Isolate* isolate = holder();

    exlib::string strKey = "_e_";
    strKey.append(ev);
    retVal += putFunction(isolate, GetHiddenList(strKey.c_str(), true), func);

    strKey = "_e1_";
    strKey.append(ev);
    retVal -= removeFunction(isolate, GetHiddenList(strKey.c_str()), func);

    return 0;
}

result_t object_base::on(v8::Local<v8::Object> map, int32_t &retVal)
{
    return _map(this, map, &object_base::on, retVal);
}

result_t object_base::once(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    retVal = 0;
    Isolate* isolate = holder();

    exlib::string strKey = "_e1_";
    strKey.append(ev);
    retVal += putFunction(isolate, GetHiddenList(strKey.c_str(), true), func);

    strKey = "_e_";
    strKey.append(ev);
    retVal -= removeFunction(isolate, GetHiddenList(strKey.c_str()), func);

    return 0;
}

result_t object_base::once(v8::Local<v8::Object> map, int32_t &retVal)
{
    return _map(this, map, &object_base::once, retVal);
}

result_t object_base::off(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    retVal = 0;
    Isolate* isolate = holder();

    exlib::string strKey = "_e_";
    strKey.append(ev);
    retVal += removeFunction(isolate, GetHiddenList(strKey.c_str()), func);

    strKey = "_e1_";
    strKey.append(ev);
    retVal += removeFunction(isolate, GetHiddenList(strKey.c_str()), func);

    return 0;
}

result_t object_base::off(exlib::string ev, int32_t &retVal)
{
    retVal = 0;

    exlib::string strKey = "_e_";
    strKey.append(ev);

    GetHiddenList(strKey.c_str(), false, true);

    strKey = "_e1_";
    strKey.append(ev);

    GetHiddenList(strKey.c_str(), false, true);

    return 0;
}

result_t object_base::off(v8::Local<v8::Object> map, int32_t &retVal)
{
    return _map(this, map, &object_base::off, retVal);
}

inline result_t _fire(v8::Local<v8::Function> func, const v8::FunctionCallbackInfo<v8::Value> &args,
                      int32_t argCount)
{
    obj_ptr<Fiber_base> retVal;
    return JSFiber::New(func, args, 1, retVal);
}

inline result_t _fire(v8::Local<v8::Function> func,
                      v8::Local<v8::Value> *args, int32_t argCount)
{
    obj_ptr<Fiber_base> retVal;
    return JSFiber::New(func, args, argCount, retVal);
}

template<typename T>
result_t fireTrigger(v8::Local<v8::Array> esa, T args, int32_t argCount)
{
    if (esa.IsEmpty())
        return 0;

    int32_t len = esa->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i ++)
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

result_t object_base::_trigger(exlib::string ev, v8::Local<v8::Value> *args,
                               int32_t argCount)
{
    extMemory(0);

    result_t hr;
    exlib::string strKey = "_e_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenList(strKey.c_str()), args, argCount);
    if (hr < 0)
        return hr;

    strKey = "_e1_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenList(strKey.c_str(), false, true), args,
                     argCount);
    if (hr < 0)
        return hr;

    return 0;
}

result_t object_base::_trigger(exlib::string ev, Variant *args, int32_t argCount)
{
    class jsTrigger: public AsyncEvent
    {
    public:
        jsTrigger(object_base *obj, exlib::string ev, Variant *args, int32_t argCount) :
            m_obj(obj), m_ev(ev)
        {
            m_args.append((VariantEx *)args, argCount);
        }

    public:
        virtual void js_invoke()
        {
            JSFiber::scope s;
            size_t i;

            std::vector<v8::Local<v8::Value> > argv;

            argv.resize(m_args.size());
            for (i = 0; i < m_args.size(); i++)
                argv[i] = v8::Local<v8::Value>::New(m_obj->holder()->m_isolate, m_args[i]);

            m_obj->_trigger(m_ev.c_str(), argv.data(), (int32_t) argv.size());

            delete this;
        }

    private:
        obj_ptr<object_base> m_obj;
        exlib::string m_ev;
        QuickArray<VariantEx> m_args;
    };

    (new jsTrigger(this, ev, args, argCount))->sync(holder());
    return 0;
}

result_t object_base::trigger(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    extMemory(0);

    result_t hr;
    exlib::string strKey = "_e_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenList(strKey.c_str()), args, 0);
    if (hr < 0)
        return hr;

    strKey = "_e1_";
    strKey.append(ev);

    hr = fireTrigger(GetHiddenList(strKey.c_str(), false, true), args, 0);
    if (hr < 0)
        return hr;

    return 0;
}

}
