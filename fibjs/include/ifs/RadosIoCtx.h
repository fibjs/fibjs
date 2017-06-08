/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RadosIoCtx_base_H_
#define _RadosIoCtx_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class RadosStream_base;

class RadosIoCtx_base : public object_base {
    DECLARE_CLASS(RadosIoCtx_base);

public:
    // RadosIoCtx_base
    virtual result_t open(exlib::string key, obj_ptr<RadosStream_base>& retVal) = 0;
    virtual result_t remove(exlib::string key, AsyncEvent* ac) = 0;
    virtual result_t destroy(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(RadosIoCtx_base, remove, exlib::string);
    ASYNC_MEMBER0(RadosIoCtx_base, destroy);
};
}

#include "RadosStream.h"

namespace fibjs {
inline ClassInfo& RadosIoCtx_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "open", s_open, false },
        { "remove", s_remove, false },
        { "destroy", s_destroy, false }
    };

    static ClassData s_cd = {
        "RadosIoCtx", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RadosIoCtx_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RadosStream_base> vr;

    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->open(v0, vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_remove(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_remove(v0);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_destroy(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_destroy();

    METHOD_VOID();
}
}

#endif
