/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RbdImage_base_H_
#define _RbdImage_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "SeekableStream.h"

namespace fibjs {

class SeekableStream_base;

class RbdImage_base : public SeekableStream_base {
    DECLARE_CLASS(RbdImage_base);

public:
    // RbdImage_base
    virtual result_t get_stripe_unit(int64_t& retVal) = 0;
    virtual result_t get_stripe_count(int64_t& retVal) = 0;
    virtual result_t get_features(int64_t& retVal) = 0;
    virtual result_t get_create_timestamp(date_t& retVal) = 0;
    virtual result_t get_block_name_prefix(exlib::string& retVal) = 0;
    virtual result_t resize(int64_t bytes, AsyncEvent* ac) = 0;
    virtual result_t createSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t removeSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t rollbackSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t listSnaps(v8::Local<v8::Array>& retVal, AsyncEvent* ac) = 0;
    virtual result_t protectSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t unprotectSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t setSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t isSnapProtected(exlib::string snapname, bool& retVal, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_stripe_unit(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_stripe_count(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_features(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_create_timestamp(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_block_name_prefix(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_resize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rollbackSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listSnaps(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_protectSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unprotectSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setSnap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSnapProtected(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(RbdImage_base, resize, int64_t);
    ASYNC_MEMBER1(RbdImage_base, createSnap, exlib::string);
    ASYNC_MEMBER1(RbdImage_base, removeSnap, exlib::string);
    ASYNC_MEMBER1(RbdImage_base, rollbackSnap, exlib::string);
    ASYNC_MEMBERVALUE1(RbdImage_base, listSnaps, v8::Local<v8::Array>);
    ASYNC_MEMBER1(RbdImage_base, protectSnap, exlib::string);
    ASYNC_MEMBER1(RbdImage_base, unprotectSnap, exlib::string);
    ASYNC_MEMBER1(RbdImage_base, setSnap, exlib::string);
    ASYNC_MEMBERVALUE2(RbdImage_base, isSnapProtected, exlib::string, bool);
};
}

namespace fibjs {
inline ClassInfo& RbdImage_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resize", s_resize, false },
        { "resizeSync", s_resize, false },
        { "createSnap", s_createSnap, false },
        { "createSnapSync", s_createSnap, false },
        { "removeSnap", s_removeSnap, false },
        { "removeSnapSync", s_removeSnap, false },
        { "rollbackSnap", s_rollbackSnap, false },
        { "rollbackSnapSync", s_rollbackSnap, false },
        { "listSnaps", s_listSnaps, false },
        { "listSnapsSync", s_listSnaps, false },
        { "protectSnap", s_protectSnap, false },
        { "protectSnapSync", s_protectSnap, false },
        { "unprotectSnap", s_unprotectSnap, false },
        { "unprotectSnapSync", s_unprotectSnap, false },
        { "setSnap", s_setSnap, false },
        { "setSnapSync", s_setSnap, false },
        { "isSnapProtected", s_isSnapProtected, false },
        { "isSnapProtectedSync", s_isSnapProtected, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "stripe_unit", s_get_stripe_unit, block_set, false },
        { "stripe_count", s_get_stripe_count, block_set, false },
        { "features", s_get_features, block_set, false },
        { "create_timestamp", s_get_create_timestamp, block_set, false },
        { "block_name_prefix", s_get_block_name_prefix, block_set, false }
    };

    static ClassData s_cd = {
        "RbdImage", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &SeekableStream_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RbdImage_base::s_get_stripe_unit(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("RbdImage.stripe_unit");
    METHOD_INSTANCE(RbdImage_base);
    PROPERTY_ENTER();

    hr = pInst->get_stripe_unit(vr);

    METHOD_RETURN();
}

inline void RbdImage_base::s_get_stripe_count(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("RbdImage.stripe_count");
    METHOD_INSTANCE(RbdImage_base);
    PROPERTY_ENTER();

    hr = pInst->get_stripe_count(vr);

    METHOD_RETURN();
}

inline void RbdImage_base::s_get_features(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("RbdImage.features");
    METHOD_INSTANCE(RbdImage_base);
    PROPERTY_ENTER();

    hr = pInst->get_features(vr);

    METHOD_RETURN();
}

inline void RbdImage_base::s_get_create_timestamp(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("RbdImage.create_timestamp");
    METHOD_INSTANCE(RbdImage_base);
    PROPERTY_ENTER();

    hr = pInst->get_create_timestamp(vr);

    METHOD_RETURN();
}

inline void RbdImage_base::s_get_block_name_prefix(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("RbdImage.block_name_prefix");
    METHOD_INSTANCE(RbdImage_base);
    PROPERTY_ENTER();

    hr = pInst->get_block_name_prefix(vr);

    METHOD_RETURN();
}

inline void RbdImage_base::s_resize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.resize");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int64_t, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_resize(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_resize(v0);

    METHOD_VOID();
}

inline void RbdImage_base::s_createSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.createSnap");
    METHOD_INSTANCE(RbdImage_base);
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

inline void RbdImage_base::s_removeSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.removeSnap");
    METHOD_INSTANCE(RbdImage_base);
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

inline void RbdImage_base::s_rollbackSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.rollbackSnap");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_rollbackSnap(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_rollbackSnap(v0);

    METHOD_VOID();
}

inline void RbdImage_base::s_listSnaps(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("RbdImage.listSnaps");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_listSnaps(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_listSnaps(vr);

    METHOD_RETURN();
}

inline void RbdImage_base::s_protectSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.protectSnap");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_protectSnap(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_protectSnap(v0);

    METHOD_VOID();
}

inline void RbdImage_base::s_unprotectSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.unprotectSnap");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_unprotectSnap(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_unprotectSnap(v0);

    METHOD_VOID();
}

inline void RbdImage_base::s_setSnap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RbdImage.setSnap");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty()) {
        pInst->acb_setSnap(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_setSnap(v0);

    METHOD_VOID();
}

inline void RbdImage_base::s_isSnapProtected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("RbdImage.isSnapProtected");
    METHOD_INSTANCE(RbdImage_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_isSnapProtected(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_isSnapProtected(v0, vr);

    METHOD_RETURN();
}
}

#endif
