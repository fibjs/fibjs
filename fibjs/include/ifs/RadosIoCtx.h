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

class RbdImage_base;
class RadosStream_base;

class RadosIoCtx_base : public object_base {
    DECLARE_CLASS(RadosIoCtx_base);

public:
    // RadosIoCtx_base
    virtual result_t createImage(exlib::string name, int64_t size, int64_t stripe_unit, int64_t stripe_count, AsyncEvent* ac) = 0;
    virtual result_t cloneImage(exlib::string pName, exlib::string pSnapshot, RadosIoCtx_base* dstio, exlib::string cName, int64_t stripe_unit, int32_t stripe_count, AsyncEvent* ac) = 0;
    virtual result_t removeImage(exlib::string name, AsyncEvent* ac) = 0;
    virtual result_t renameImage(exlib::string src, exlib::string dst, AsyncEvent* ac) = 0;
    virtual result_t listImages(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t openImage(exlib::string name, exlib::string snapshot, obj_ptr<RbdImage_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t version(exlib::string& retVal) = 0;
    virtual result_t open(exlib::string key, obj_ptr<RadosStream_base>& retVal) = 0;
    virtual result_t remove(exlib::string key, AsyncEvent* ac) = 0;
    virtual result_t createSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t removeSnap(exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac) = 0;
    virtual result_t listOids(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t listOids(exlib::string pattern, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setXattr(exlib::string oid, exlib::string attr, exlib::string value, AsyncEvent* ac) = 0;
    virtual result_t rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac) = 0;
    virtual result_t getXattrs(exlib::string oid, obj_ptr<NObject>& retVal, AsyncEvent* ac) = 0;
    virtual result_t destroy(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_createImage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_cloneImage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeImage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_renameImage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listImages(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openImage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_version(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    ASYNC_MEMBER4(RadosIoCtx_base, createImage, exlib::string, int64_t, int64_t, int64_t);
    ASYNC_MEMBER6(RadosIoCtx_base, cloneImage, exlib::string, exlib::string, RadosIoCtx_base*, exlib::string, int64_t, int32_t);
    ASYNC_MEMBER1(RadosIoCtx_base, removeImage, exlib::string);
    ASYNC_MEMBER2(RadosIoCtx_base, renameImage, exlib::string, exlib::string);
    ASYNC_MEMBERVALUE1(RadosIoCtx_base, listImages, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE3(RadosIoCtx_base, openImage, exlib::string, exlib::string, obj_ptr<RbdImage_base>);
    ASYNC_MEMBER1(RadosIoCtx_base, remove, exlib::string);
    ASYNC_MEMBER1(RadosIoCtx_base, createSnap, exlib::string);
    ASYNC_MEMBER1(RadosIoCtx_base, removeSnap, exlib::string);
    ASYNC_MEMBER2(RadosIoCtx_base, rollbackSnap, exlib::string, exlib::string);
    ASYNC_MEMBERVALUE1(RadosIoCtx_base, listOids, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(RadosIoCtx_base, listOids, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE3(RadosIoCtx_base, getXattr, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER3(RadosIoCtx_base, setXattr, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER2(RadosIoCtx_base, rmXattr, exlib::string, exlib::string);
    ASYNC_MEMBERVALUE2(RadosIoCtx_base, getXattrs, exlib::string, obj_ptr<NObject>);
    ASYNC_MEMBER0(RadosIoCtx_base, destroy);
};
}

#include "RbdImage.h"
#include "RadosStream.h"

namespace fibjs {
inline ClassInfo& RadosIoCtx_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createImage", s_createImage, false },
        { "createImageSync", s_createImage, false },
        { "cloneImage", s_cloneImage, false },
        { "cloneImageSync", s_cloneImage, false },
        { "removeImage", s_removeImage, false },
        { "removeImageSync", s_removeImage, false },
        { "renameImage", s_renameImage, false },
        { "renameImageSync", s_renameImage, false },
        { "listImages", s_listImages, false },
        { "listImagesSync", s_listImages, false },
        { "openImage", s_openImage, false },
        { "openImageSync", s_openImage, false },
        { "version", s_version, false },
        { "open", s_open, false },
        { "remove", s_remove, false },
        { "removeSync", s_remove, false },
        { "createSnap", s_createSnap, false },
        { "createSnapSync", s_createSnap, false },
        { "removeSnap", s_removeSnap, false },
        { "removeSnapSync", s_removeSnap, false },
        { "rollbackSnap", s_rollbackSnap, false },
        { "rollbackSnapSync", s_rollbackSnap, false },
        { "listOids", s_listOids, false },
        { "listOidsSync", s_listOids, false },
        { "getXattr", s_getXattr, false },
        { "getXattrSync", s_getXattr, false },
        { "setXattr", s_setXattr, false },
        { "setXattrSync", s_setXattr, false },
        { "rmXattr", s_rmXattr, false },
        { "rmXattrSync", s_rmXattr, false },
        { "getXattrs", s_getXattrs, false },
        { "getXattrsSync", s_getXattrs, false },
        { "destroy", s_destroy, false },
        { "destroySync", s_destroy, false }
    };

    static ClassData s_cd = {
        "RadosIoCtx", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RadosIoCtx_base::s_createImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RadosIoCtx.createImage");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(4, 2);

    ARG(exlib::string, 0);
    ARG(int64_t, 1);
    OPT_ARG(int64_t, 2, -1);
    OPT_ARG(int64_t, 3, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_createImage(v0, v1, v2, v3, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_createImage(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_cloneImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RadosIoCtx.cloneImage");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(6, 4);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(obj_ptr<RadosIoCtx_base>, 2);
    ARG(exlib::string, 3);
    OPT_ARG(int64_t, 4, -1);
    OPT_ARG(int32_t, 5, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_cloneImage(v0, v1, v2, v3, v4, v5, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_cloneImage(v0, v1, v2, v3, v4, v5);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_removeImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RadosIoCtx.removeImage");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_removeImage(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_removeImage(v0);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_renameImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RadosIoCtx.renameImage");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_renameImage(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_renameImage(v0, v1);

    METHOD_VOID();
}

inline void RadosIoCtx_base::s_listImages(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("RadosIoCtx.listImages");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_listImages(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_listImages(vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_openImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RbdImage_base> vr;

    METHOD_NAME("RadosIoCtx.openImage");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty()) {
        pInst->acb_openImage(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_openImage(v0, v1, vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_version(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("RadosIoCtx.version");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->version(vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RadosStream_base> vr;

    METHOD_NAME("RadosIoCtx.open");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->open(v0, vr);

    METHOD_RETURN();
}

inline void RadosIoCtx_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RadosIoCtx.remove");
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
    METHOD_NAME("RadosIoCtx.createSnap");
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
    METHOD_NAME("RadosIoCtx.removeSnap");
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
    METHOD_NAME("RadosIoCtx.rollbackSnap");
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
    obj_ptr<NArray> vr;

    METHOD_NAME("RadosIoCtx.listOids");
    METHOD_INSTANCE(RadosIoCtx_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_listOids(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_listOids(vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

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

    METHOD_NAME("RadosIoCtx.getXattr");
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
    METHOD_NAME("RadosIoCtx.setXattr");
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
    METHOD_NAME("RadosIoCtx.rmXattr");
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
    obj_ptr<NObject> vr;

    METHOD_NAME("RadosIoCtx.getXattrs");
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
    METHOD_NAME("RadosIoCtx.destroy");
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
