/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Fiber_base_H_
#define _Fiber_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Fiber_base : public object_base {
    DECLARE_CLASS(Fiber_base);

public:
    // Fiber_base
    virtual result_t join() = 0;
    virtual result_t get_caller(obj_ptr<Fiber_base>& retVal) = 0;
    virtual result_t get_traceInfo(exlib::string& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_caller(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_traceInfo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Fiber_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "join", s_join, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "caller", s_get_caller, block_set, false },
        { "traceInfo", s_get_traceInfo, block_set, false }
    };

    static ClassData s_cd = {
        "Fiber", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Fiber_base::s_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Fiber_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->join();

    METHOD_VOID();
}

inline void Fiber_base::s_get_caller(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Fiber_base> vr;

    METHOD_INSTANCE(Fiber_base);
    PROPERTY_ENTER();

    hr = pInst->get_caller(vr);

    METHOD_RETURN();
}

inline void Fiber_base::s_get_traceInfo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Fiber_base);
    PROPERTY_ENTER();

    hr = pInst->get_traceInfo(vr);

    METHOD_RETURN();
}
}

#endif
