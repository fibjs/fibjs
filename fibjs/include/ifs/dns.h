/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _dns_base_H_
#define _dns_base_H_

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

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(dns_base, resolve, exlib::string, obj_ptr<NArray>);
};
}

namespace fibjs {
inline ClassInfo& dns_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resolve", s_resolve, true },
        { "resolveSync", s_resolve, true }
    };

    static ClassData s_cd = {
        "dns", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void dns_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("dns.resolve");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_resolve(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_resolve(v0, vr);

    METHOD_RETURN();
}
}

#endif
