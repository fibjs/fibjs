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
class List_base;
class Regex_base;

class RadosIoCtx_base : public object_base {
    DECLARE_CLASS(RadosIoCtx_base);

public:
    // RadosIoCtx_base
    virtual result_t open(exlib::string key, obj_ptr<RadosStream_base>& retVal) = 0;
    virtual result_t remove(exlib::string key, AsyncEvent* ac) = 0;
    virtual result_t createSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t removeSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t listOids(obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t listOids(Regex_base* reg, obj_ptr<List_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setXattr(exlib::string oid, exlib::string attr, exlib::string value, AsyncEvent* ac) = 0;
    virtual result_t rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac) = 0;
    virtual result_t getXattrs(exlib::string oid, v8::Local<v8::Object>& retVal, AsyncEvent* ac) = 0;
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
    static void s_createSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rollbackSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listOids(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getXattr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setXattr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rmXattr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getXattrs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(RadosIoCtx_base, remove, exlib::string);
    ASYNC_MEMBER1(RadosIoCtx_base, createSnap, exlib::string);
    ASYNC_MEMBER1(RadosIoCtx_base, removeSnap, exlib::string);
    ASYNC_MEMBER2(RadosIoCtx_base, rollbackSnap, exlib::string, exlib::string);
    ASYNC_MEMBERVALUE1(RadosIoCtx_base, listOids, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE2(RadosIoCtx_base, listOids, Regex_base*, obj_ptr<List_base>);
    ASYNC_MEMBERVALUE3(RadosIoCtx_base, getXattr, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER3(RadosIoCtx_base, setXattr, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER2(RadosIoCtx_base, rmXattr, exlib::string, exlib::string);
    ASYNC_MEMBERVALUE2(RadosIoCtx_base, getXattrs, exlib::string, v8::Local<v8::Object>);
    ASYNC_MEMBER0(RadosIoCtx_base, destroy);
};
}

#include "RadosStream.h"
#include "List.h"
#include "Regex.h"

namespace fibjs {
inline ClassInfo& RadosIoCtx_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "open", s_open, false },
        { "remove", s_remove, false },
        { "createSnap", s_createSnap, false },
        { "removeSnap", s_removeSnap, false },
        { "rollbackSnap", s_rollbackSnap, false },
        { "listOids", s_listOids, false },
        { "getXattr", s_getXattr, false },
        { "setXattr", s_setXattr, false },
        { "rmXattr", s_rmXattr, false },
        { "getXattrs", s_getXattrs, false },
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

inline void RadosIoCtx_base::s_createSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_createSnap(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_createSnap(v0);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_removeSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_removeSnap(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_removeSnap(v0);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_rollbackSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_rollbackSnap(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_rollbackSnap(v0, v1);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_listOids(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_listOids(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_listOids(vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Regex_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_listOids(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_listOids(v0, vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_getXattr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_getXattr(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_getXattr(v0, v1, vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_setXattr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);

    if (!cb.IsEmpty()) {
        pInst->acb_setXattr(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_setXattr(v0, v1, v2);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_rmXattr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_rmXattr(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_rmXattr(v0, v1);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_getXattrs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_getXattrs(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_getXattrs(v0, vr);

    METHOD_RETURN();
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
