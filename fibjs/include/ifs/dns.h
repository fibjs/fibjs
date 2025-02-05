/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class dns_base : public object_base {
    DECLARE_CLASS(dns_base);

public:
    // dns_base
    static result_t resolve(exlib::string name, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    static result_t lookup(exlib::string name, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<dns_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lookup(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(dns_base, resolve, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(dns_base, lookup, exlib::string, v8::Local<v8::Object>, Variant);
};
}

namespace fibjs {
inline ClassInfo& dns_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resolve", s_static_resolve, true, ClassData::ASYNC_ASYNC },
        { "lookup", s_static_lookup, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData s_cd = {
        "dns", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void dns_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_resolve(v0, cb, args);
    else
        hr = ac_resolve(v0, vr);

    METHOD_RETURN();
}

inline void dns_base::s_static_lookup(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_lookup(v0, v1, cb, args);
    else
        hr = ac_lookup(v0, v1, vr);

    METHOD_RETURN();
}
}
