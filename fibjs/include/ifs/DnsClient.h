/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DnsClient_base_H_
#define _DnsClient_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class List_base;
class Map_base;

class DnsClient_base : public object_base {
    DECLARE_CLASS(DnsClient_base);

public:
    // DnsClient_base
    static result_t _new(obj_ptr<DnsClient_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t resolve(exlib::string host, exlib::string type, obj_ptr<object_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolve4(exlib::string host, Map_base* options, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolve4(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolve6(exlib::string host, Map_base* options, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolve6(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveMx(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveTxt(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveSrv(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveSoa(exlib::string host, obj_ptr<object_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveNs(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveCname(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolveNaptr(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t resolvePtr(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve4(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveMx(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveTxt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveSrv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveSoa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveNs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveCname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveNaptr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolvePtr(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(DnsClient_base, resolve, exlib::string, exlib::string, obj_ptr<object_base>);
    ASYNC_MEMBERVALUE3(DnsClient_base, resolve4, exlib::string, Map_base*, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolve4, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE3(DnsClient_base, resolve6, exlib::string, Map_base*, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolve6, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveMx, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveTxt, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveSrv, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveSoa, exlib::string, obj_ptr<object_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveNs, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveCname, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolveNaptr, exlib::string, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(DnsClient_base, resolvePtr, exlib::string, obj_ptr<List_base>);
};
}

#include "List.h"
#include "Map.h"

namespace fibjs {
inline ClassInfo& DnsClient_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resolve", s_resolve, false },
        { "resolveSync", s_resolve, false },
        { "resolve4", s_resolve4, false },
        { "resolve4Sync", s_resolve4, false },
        { "resolve6", s_resolve6, false },
        { "resolve6Sync", s_resolve6, false },
        { "resolveMx", s_resolveMx, false },
        { "resolveMxSync", s_resolveMx, false },
        { "resolveTxt", s_resolveTxt, false },
        { "resolveTxtSync", s_resolveTxt, false },
        { "resolveSrv", s_resolveSrv, false },
        { "resolveSrvSync", s_resolveSrv, false },
        { "resolveSoa", s_resolveSoa, false },
        { "resolveSoaSync", s_resolveSoa, false },
        { "resolveNs", s_resolveNs, false },
        { "resolveNsSync", s_resolveNs, false },
        { "resolveCname", s_resolveCname, false },
        { "resolveCnameSync", s_resolveCname, false },
        { "resolveNaptr", s_resolveNaptr, false },
        { "resolveNaptrSync", s_resolveNaptr, false },
        { "resolvePtr", s_resolvePtr, false },
        { "resolvePtrSync", s_resolvePtr, false }
    };

    static ClassData s_cd = {
        "DnsClient", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DnsClient_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void DnsClient_base::__new(const T& args)
{
    obj_ptr<DnsClient_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void DnsClient_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<object_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "A");

    if (!cb.IsEmpty()) {
        pInst->acb_resolve(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolve(v0, v1, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolve4(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Map_base>, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_resolve4(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolve4(v0, v1, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolve4(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolve4(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolve6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Map_base>, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_resolve6(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolve6(v0, v1, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolve6(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolve6(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveMx(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveMx(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveMx(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveTxt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveTxt(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveTxt(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveSrv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveSrv(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveSrv(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveSoa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<object_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveSoa(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveSoa(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveNs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveNs(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveNs(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveCname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveCname(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveCname(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolveNaptr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolveNaptr(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolveNaptr(v0, vr);

    METHOD_RETURN();
}

inline void DnsClient_base::s_resolvePtr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(DnsClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resolvePtr(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resolvePtr(v0, vr);

    METHOD_RETURN();
}
}

#endif
