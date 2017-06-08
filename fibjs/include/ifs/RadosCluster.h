/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RadosCluster_base_H_
#define _RadosCluster_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class RadosIoCtx_base;

class RadosCluster_base : public object_base {
    DECLARE_CLASS(RadosCluster_base);

public:
    // RadosCluster_base
    static result_t _new(exlib::string clusterName, exlib::string userName, exlib::string confPath, obj_ptr<RadosCluster_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t connect(AsyncEvent* ac) = 0;
    virtual result_t createIoCtx(exlib::string poolName, obj_ptr<RadosIoCtx_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t shutdown(AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createIoCtx(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_shutdown(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(RadosCluster_base, connect);
    ASYNC_MEMBERVALUE2(RadosCluster_base, createIoCtx, exlib::string, obj_ptr<RadosIoCtx_base>);
    ASYNC_MEMBER0(RadosCluster_base, shutdown);
};
}

#include "RadosIoCtx.h"

namespace fibjs {
inline ClassInfo& RadosCluster_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_connect, false },
        { "createIoCtx", s_createIoCtx, false },
        { "shutdown", s_shutdown, false }
    };

    static ClassData s_cd = {
        "RadosCluster", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RadosCluster_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void RadosCluster_base::__new(const T& args)
{
    obj_ptr<RadosCluster_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void RadosCluster_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosCluster_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_connect(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_connect();

    METHOD_VOID();
}

inline void RadosCluster_base::s_createIoCtx(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RadosIoCtx_base> vr;

    METHOD_INSTANCE(RadosCluster_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_createIoCtx(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_createIoCtx(v0, vr);

    METHOD_RETURN();
}

inline void RadosCluster_base::s_shutdown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosCluster_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_shutdown(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_shutdown();

    METHOD_VOID();
}
}

#endif
