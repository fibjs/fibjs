/*
 * Event.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

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

v8::Local<v8::Object> object_base::GetHiddenList(const char *k, bool create,
        bool autoDelete)
{
    v8::Local<v8::Object> o = wrap();
    v8::Local<v8::String> s = v8::String::NewFromUtf8(isolate, k);
    v8::Local<v8::Value> es = o->GetHiddenValue(s);
    v8::Local<v8::Object> esa;

    if (es.IsEmpty())
    {
        if (create)
        {
            esa = v8::Object::New(isolate);
            o->SetHiddenValue(s, esa);
        }
    }
    else
        esa = v8::Local<v8::Object>::Cast(es);

    if (autoDelete)
        o->DeleteHiddenValue(s);

    return esa;
}

static uint64_t s_fid = 0;

inline int32_t putFunction(v8::Local<v8::Object> esa, v8::Local<v8::Function> func)
{
    v8::Local<v8::String> s = v8::String::NewFromUtf8(isolate, "_fid");
    v8::Local<v8::Value> fid = func->GetHiddenValue(s);
    char buf[64];
    const int32_t base = 26;

    if (fid.IsEmpty())
    {
        uint64_t num = s_fid ++;
        int32_t p = 0;

        buf[p++] = '_';
        while (num)
        {
            buf[p++] = (char)(num % base) + 'a';
            num /= base;
        }

        buf[p++] = 0;

        fid = v8::String::NewFromUtf8(isolate, buf);
        func->SetHiddenValue(s, fid);
    }

    if (!esa->Has(fid))
    {
        esa->Set(fid, func);
        return 1;
    }

    return 0;
}

inline int32_t removeFunction(v8::Local<v8::Object> esa,
                              v8::Local<v8::Function> func)
{
    if (esa.IsEmpty())
        return 0;

    v8::Local<v8::String> s = v8::String::NewFromUtf8(isolate, "_fid");
    v8::Local<v8::Value> fid = func->GetHiddenValue(s);

    if (!fid.IsEmpty() && esa->Has(fid))
    {
        esa->Delete(fid);
        return 1;
    }

    return 0;
}

inline result_t _map(object_base *o, v8::Local<v8::Object> m,
                     result_t (object_base::*fn)(const char *, v8::Local<v8::Function>, int32_t &),
                     int32_t &retVal)
{
    v8::Local<v8::Array> ks = m->GetPropertyNames();
    int len = ks->Length();
    int i;

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

result_t object_base::on(const char *ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    retVal = 0;

    std::string strKey = "_e_";
    strKey.append(ev);
    retVal += putFunction(GetHiddenList(strKey.c_str(), true), func);

    strKey = "_e1_";
    strKey.append(ev);
    retVal -= removeFunction(GetHiddenList(strKey.c_str()), func);

    return 0;
}

result_t object_base::on(v8::Local<v8::Object> map, int32_t &retVal)
{
    return _map(this, map, &object_base::on, retVal);
}

result_t object_base::once(const char *ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    retVal = 0;

    std::string strKey = "_e1_";
    strKey.append(ev);
    retVal += putFunction(GetHiddenList(strKey.c_str(), true), func);

    strKey = "_e_";
    strKey.append(ev);
    retVal -= removeFunction(GetHiddenList(strKey.c_str()), func);

    return 0;
}

result_t object_base::once(v8::Local<v8::Object> map, int32_t &retVal)
{
    return _map(this, map, &object_base::once, retVal);
}

result_t object_base::off(const char *ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    retVal = 0;

    std::string strKey = "_e_";
    strKey.append(ev);
    retVal += removeFunction(GetHiddenList(strKey.c_str()), func);

    strKey = "_e1_";
    strKey.append(ev);
    retVal += removeFunction(GetHiddenList(strKey.c_str()), func);

    return 0;
}

result_t object_base::off(const char *ev, int32_t &retVal)
{
    retVal = 0;

    std::string strKey = "_e_";
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
result_t fireTrigger(v8::Local<v8::Object> esa, T args, int argCount)
{
    if (esa.IsEmpty())
        return 0;

    v8::Local<v8::Array> ks = esa->GetPropertyNames();
    int len = ks->Length();
    int i;
    result_t hr;

    for (i = 0; i < len; i++)
    {
        v8::Local<v8::Value> func = esa->Get(ks->Get(i));
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

    result_t hr;
    std::string strKey = "_e_";
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

result_t object_base::_trigger(const char *ev, Variant *args, int argCount)
{
    class jsTrigger: public asyncRelease
    {
    public:
        jsTrigger(object_base *obj, const char *ev, Variant *args, int argCount) :
            m_obj(obj), m_ev(ev)
        {
            m_args.append((VariantEx *)args, argCount);
        }

    public:
        virtual void js_callback()
        {
            JSFiber::scope s;
            size_t i;

            std::vector<v8::Local<v8::Value> > argv;

            argv.resize(m_args.size());
            for (i = 0; i < m_args.size(); i++)
                argv[i] = v8::Local<v8::Value>::New(isolate, m_args[i]);

            m_obj->_trigger(m_ev.c_str(), argv.data(), (int) argv.size());

            delete this;
        }

    private:
        obj_ptr<object_base> m_obj;
        std::string m_ev;
        QuickArray<VariantEx> m_args;
    };

    (new jsTrigger(this, ev, args, argCount))->post(0);
    return 0;
}

result_t object_base::trigger(const char *ev, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    extMemory(0);

    result_t hr;
    std::string strKey = "_e_";
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
