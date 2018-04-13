/*
 * RadosIoCtx.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: asionius
 */

#include "object.h"
#include "RadosIoCtx.h"
#include "RadosStream.h"
#include "RbdImage.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {
#ifndef _WIN32

#define MAX_XATTRLEN 4096

extern void* get_librados_handle(void);
static void* s_handle = NULL;
static bool s_ldlib = false;
void* get_librbd_handle(void)
{
    return s_handle;
}

static void (*_rados_ioctx_destroy)(rados_ioctx_t);
static int (*_rados_ioctx_create)(rados_t, const char*, rados_ioctx_t*);
static int (*_rados_ioctx_snap_create)(rados_ioctx_t, const char*);
static int (*_rados_ioctx_snap_remove)(rados_ioctx_t, const char*);
static int (*_rados_ioctx_snap_rollback)(rados_ioctx_t, const char*, const char*);
static int (*_rados_objects_list_open)(rados_ioctx_t, rados_list_ctx_t*);
static int (*_rados_objects_list_next)(rados_list_ctx_t, const char**, const char**);
static void (*_rados_objects_list_close)(rados_list_ctx_t);
static int (*_rados_getxattr)(rados_ioctx_t, const char*, const char*, char*, size_t);
static int (*_rados_setxattr)(rados_ioctx_t, const char*, const char*, const char*, size_t);
static int (*_rados_rmxattr)(rados_ioctx_t, const char*, const char*);
static int (*_rados_getxattrs)(rados_ioctx_t, const char*, rados_xattrs_iter_t*);
static int (*_rados_getxattrs_next)(rados_xattrs_iter_t, const char**, const char**, size_t*);
static void (*_rados_getxattrs_end)(rados_xattrs_iter_t);

static int (*_rbd_create3)(rados_ioctx_t, const char*, uint64_t,
    uint64_t, int*,
    uint64_t, uint64_t);
static int (*_rbd_create2)(rados_ioctx_t, const char*, uint64_t,
    uint64_t, int*);
static int (*_rbd_stat)(rbd_image_t, rbd_image_info_t*, size_t);
static int (*_rbd_close)(rbd_image_t);
static int (*_rbd_resize)(rbd_image_t, uint64_t);
static int (*_rbd_rename)(rados_ioctx_t, const char*, const char*);
static int (*_rbd_remove)(rados_ioctx_t io, const char* name);
static int (*_rbd_list)(rados_ioctx_t, char*, size_t*);
static int (*_rbd_clone2)(rados_ioctx_t, const char*,
    const char*, rados_ioctx_t,
    const char*, uint64_t, int*,
    uint64_t, int);
static int (*_rbd_clone)(rados_ioctx_t, const char*,
    const char*, rados_ioctx_t,
    const char*, uint64_t, int*);
static void (*_rbd_version)(int*, int*, int*);

static inline int32_t load_librados(void)
{
    static bool ldlib = false;
    void* handle = NULL;

    if (ldlib)
        return 0;

    handle = get_librados_handle();
    if (!handle)
        return CHECK_ERROR(Runtime::setError("Invalid library handle"));

    _rados_ioctx_destroy = (void (*)(rados_ioctx_t io))dlsym(handle, "rados_ioctx_destroy");
    _rados_ioctx_create = (int (*)(rados_t, const char*, rados_ioctx_t*))dlsym(handle, "rados_ioctx_create");
    _rados_ioctx_snap_create = (int (*)(rados_ioctx_t, const char*))dlsym(handle, "rados_ioctx_snap_create");
    _rados_ioctx_snap_remove = (int (*)(rados_ioctx_t, const char*))dlsym(handle, "rados_ioctx_snap_remove");
    _rados_ioctx_snap_rollback = (int (*)(rados_ioctx_t, const char*, const char*))dlsym(handle, "rados_ioctx_snap_rollback");
    _rados_objects_list_open = (int (*)(rados_ioctx_t, rados_list_ctx_t*))dlsym(handle, "rados_objects_list_open");
    _rados_objects_list_next = (int (*)(rados_list_ctx_t, const char**, const char**))dlsym(handle, "rados_objects_list_next");
    _rados_objects_list_close = (void (*)(rados_list_ctx_t))dlsym(handle, "rados_objects_list_close");
    _rados_getxattr = (int (*)(rados_ioctx_t, const char*, const char*, char*, size_t))dlsym(handle, "rados_getxattr");
    _rados_setxattr = (int (*)(rados_ioctx_t, const char*, const char*, const char*, size_t))dlsym(handle, "rados_setxattr");
    _rados_rmxattr = (int (*)(rados_ioctx_t, const char*, const char*))dlsym(handle, "rados_rmxattr");
    _rados_getxattrs = (int (*)(rados_ioctx_t, const char*, rados_xattrs_iter_t*))dlsym(handle, "rados_getxattrs");
    _rados_getxattrs_next = (int (*)(rados_xattrs_iter_t, const char**, const char**, size_t*))dlsym(handle, "rados_getxattrs_next");
    _rados_getxattrs_end = (void (*)(rados_xattrs_iter_t))dlsym(handle, "rados_getxattrs_end");

    if (!_rados_ioctx_destroy
        || !_rados_ioctx_create
        || !_rados_ioctx_snap_create
        || !_rados_ioctx_snap_remove
        || !_rados_ioctx_snap_rollback
        || !_rados_objects_list_open
        || !_rados_objects_list_next
        || !_rados_objects_list_close
        || !_rados_getxattr
        || !_rados_setxattr
        || !_rados_rmxattr
        || !_rados_getxattrs
        || !_rados_getxattrs_next
        || !_rados_getxattrs_end)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    return 0;
}

static inline int32_t load_librbd(void)
{
    if (s_ldlib)
        return 0;

    s_ldlib = true;
    s_handle = dlopen("librbd.so", RTLD_LAZY);
    if (!s_handle)
        return CHECK_ERROR(Runtime::setError("Cannot load library: 'librbd.so'"));

    _rbd_create3 = (int (*)(rados_ioctx_t, const char*,
        uint64_t, uint64_t,
        int*, uint64_t, uint64_t))dlsym(s_handle, "rbd_create3");
    _rbd_create2 = (int (*)(rados_ioctx_t, const char*,
        uint64_t, uint64_t,
        int*))dlsym(s_handle, "rbd_create2");
    _rbd_stat = (int (*)(rbd_image_t, rbd_image_info_t*,
        size_t))dlsym(s_handle, "rbd_stat");
    _rbd_close = (int (*)(rbd_image_t))dlsym(s_handle, "rbd_close");
    _rbd_resize = (int (*)(rbd_image_t, uint64_t))dlsym(s_handle, "rbd_resize");
    _rbd_rename = (int (*)(rados_ioctx_t, const char*,
        const char*))dlsym(s_handle, "rbd_rename");
    _rbd_remove = (int (*)(rados_ioctx_t io, const char* name))dlsym(s_handle, "rbd_remove");
    _rbd_list = (int (*)(rados_ioctx_t, char*,
        size_t*))dlsym(s_handle, "rbd_list");
    _rbd_clone2 = (int (*)(rados_ioctx_t, const char*,
        const char*, rados_ioctx_t,
        const char*, uint64_t,
        int*, uint64_t,
        int))dlsym(s_handle, "rbd_clone2");
    _rbd_clone = (int (*)(rados_ioctx_t, const char*,
        const char*, rados_ioctx_t,
        const char*, uint64_t,
        int*))dlsym(s_handle, "rbd_clone");
    _rbd_version = (void (*)(int*, int*,
        int*))dlsym(s_handle, "rbd_version");

    if (!_rbd_create3
        || !_rbd_create2
        || !_rbd_stat
        || !_rbd_close
        || !_rbd_resize
        || !_rbd_rename
        || !_rbd_remove
        || !_rbd_list
        || !_rbd_clone2
        || !_rbd_version)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    return 0;
}

result_t RadosIoCtx::createImage(exlib::string name, int64_t size, int64_t stripe_unit, int64_t stripe_count, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    int32_t order = 0;

    hr = load_librbd();
    if (hr < 0)
        return hr;

    if (stripe_unit < 0)
        hr = _rbd_create2(m_ioctx, name.c_str(),
            size, RBD_FEATURE_LAYERING, &order);
    else
        hr = _rbd_create3(m_ioctx, name.c_str(),
            size, RBD_FEATURE_LAYERING | RBD_FEATURE_STRIPINGV2,
            &order, stripe_unit, stripe_count);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::cloneImage(exlib::string pName, exlib::string pSnapshot, RadosIoCtx_base* dstio, exlib::string cName, int64_t stripe_unit, int32_t stripe_count, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    int32_t order = 0;

    hr = load_librbd();
    if (hr < 0)
        return hr;

    RadosIoCtx* dio = (RadosIoCtx*)dstio;
    const char* p_snap = pSnapshot.c_str();
    if (stripe_unit < 0)
        hr = _rbd_clone(m_ioctx, pName.c_str(),
            p_snap[0] == '\0' ? NULL : p_snap, dio->m_ioctx,
            cName.c_str(), RBD_FEATURE_LAYERING, &order);
    else
        hr = _rbd_clone2(m_ioctx, pName.c_str(),
            p_snap[0] == '\0' ? NULL : p_snap, dio->m_ioctx,
            cName.c_str(), RBD_FEATURE_LAYERING | RBD_FEATURE_STRIPINGV2,
            &order, stripe_unit, stripe_count);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::removeImage(exlib::string name, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = load_librbd();
    if (hr < 0)
        return hr;

    hr = _rbd_remove(m_ioctx, name.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::renameImage(exlib::string src, exlib::string dst, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = load_librbd();
    if (hr < 0)
        return hr;

    hr = _rbd_rename(m_ioctx, src.c_str(), dst.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::listImages(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    hr = load_librbd();
    if (hr < 0)
        return hr;

    size_t max_size = 1024;
    exlib::string strBuf;

    strBuf.resize(max_size);
    char* names = &strBuf[0];
    char* cur_name;
    obj_ptr<NArray> oa;

    oa = new NArray();
    hr = _rbd_list(m_ioctx, names, &max_size);
    if (hr < 0) {
        if (hr != -ERANGE)
            return CHECK_ERROR(hr);

        strBuf.resize(max_size);
        names = &strBuf[0];
        hr = _rbd_list(m_ioctx, names, &max_size);
        if (hr < 0)
            return CHECK_ERROR(hr);
    }

    for (cur_name = names; cur_name < names + hr;) {
        oa->append(cur_name);
        cur_name += strlen(cur_name) + 1;
    }
    retVal = oa;

    return 0;
}

result_t RadosIoCtx::openImage(exlib::string name, exlib::string snapshot, obj_ptr<RbdImage_base>& retVal, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = load_librbd();
    if (hr < 0)
        return hr;

    obj_ptr<RbdImage> img = new RbdImage();

    hr = img->open(m_ioctx, name, snapshot);
    if (hr < 0)
        return hr;

    retVal = img;
    return 0;
}

result_t RadosIoCtx::version(exlib::string& retVal)
{
    result_t hr;

    hr = load_librbd();
    if (hr < 0)
        return hr;

    char version[12] = { 0 };
    int32_t maj, min, extra;

    _rbd_version(&maj, &min, &extra);
    sprintf(version, "%d.%d.%d", maj, min, extra);

    retVal = version;

    return 0;
}

result_t RadosIoCtx::open(exlib::string key, obj_ptr<RadosStream_base>& retVal)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;

    obj_ptr<RadosStream> s = new RadosStream();

    hr = s->open(m_ioctx, key);
    if (hr < 0)
        return hr;

    retVal = s;
    return 0;
}

result_t RadosIoCtx::createSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_ioctx_snap_create(m_ioctx, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::removeSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_ioctx_snap_remove(m_ioctx, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_ioctx_snap_rollback(m_ioctx, oid.c_str(), snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::listOids(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    rados_list_ctx_t ctx;
    const char* entry;
    obj_ptr<NArray> oa;

    oa = new NArray();

    hr = _rados_objects_list_open(m_ioctx, &ctx);
    if (hr < 0)
        return CHECK_ERROR(hr);

    while ((hr = _rados_objects_list_next(ctx, &entry, NULL)) != -ENOENT) {
        if (hr < 0)
            return CHECK_ERROR(hr);
        oa->append(entry);
    }
    retVal = oa;
    _rados_objects_list_close(ctx);

    return 0;
}

#ifndef RE_SIZE
#define RE_SIZE 64
#endif
result_t RadosIoCtx::listOids(exlib::string pattern, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    rados_list_ctx_t ctx;
    int32_t rc = 0;
    int32_t opt = PCRE_JAVASCRIPT_COMPAT | PCRE_NEWLINE_ANYCRLF | PCRE_UCP;
    const char* error;
    int32_t erroffset;
    pcre* re;
    int32_t ovector[RE_SIZE];
    const char* entry;
    obj_ptr<NArray> oa;

    oa = new NArray();

    hr = _rados_objects_list_open(m_ioctx, &ctx);
    if (hr < 0)
        return CHECK_ERROR(hr);

    re = pcre_compile(pattern.c_str(), opt, &error, &erroffset, NULL);
    if (re == NULL) {
        char buf[1024];

        sprintf(buf, "listOids: Compilation failed at offset %d: %s.", erroffset, error);
        return CHECK_ERROR(Runtime::setError(buf));
    }

    while ((hr = _rados_objects_list_next(ctx, &entry, NULL)) != -ENOENT) {
        if (hr < 0)
            return CHECK_ERROR(hr);
        rc = pcre_exec(re, NULL, entry, (int32_t)qstrlen(entry),
            0, 0, ovector, RE_SIZE);
        if (rc > 0)
            oa->append(entry);
    }
    retVal = oa;

    pcre_free(re);
    _rados_objects_list_close(ctx);

    return 0;
}

result_t RadosIoCtx::getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    char buf[MAX_XATTRLEN];

    hr = _rados_getxattr(m_ioctx, oid.c_str(), attr.c_str(), buf, MAX_XATTRLEN);
    if (hr < 0)
        return CHECK_ERROR(hr);

    buf[hr] = '\0';
    retVal = buf;

    return 0;
}

result_t RadosIoCtx::setXattr(exlib::string oid, exlib::string attr, exlib::string value, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_setxattr(m_ioctx, oid.c_str(), attr.c_str(), value.c_str(), value.length());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_rmxattr(m_ioctx, oid.c_str(), attr.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosIoCtx::getXattrs(exlib::string oid, obj_ptr<NObject>& retVal, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;
    rados_xattrs_iter_t iter;
    obj_ptr<Attrs> attrs;

    attrs = new Attrs();
    hr = _rados_getxattrs(m_ioctx, oid.c_str(), &iter);
    if (hr < 0)
        return CHECK_ERROR(hr);

    while (1) {
        const char* name;
        const char* val;
        size_t len;

        hr = _rados_getxattrs_next(iter, &name, &val, &len);
        if (hr < 0)
            return CHECK_ERROR(hr);

        if (name == NULL)
            break;
        exlib::string str(val, len);
        attrs->add(name, str);
    }
    _rados_getxattrs_end(iter);
    retVal = attrs;

    return 0;
}

result_t RadosIoCtx::destroy(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_ioctx) {
        _rados_ioctx_destroy(m_ioctx);
        m_ioctx = NULL;
    }
    return 0;
}

result_t RadosIoCtx::remove(exlib::string key, AsyncEvent* ac)
{
    if (!m_ioctx)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    obj_ptr<RadosStream> stm = new RadosStream();
    stm->open(m_ioctx, key);

    hr = stm->remove(ac);
    if (hr < 0)
        return hr;

    return 0;
}

result_t RadosIoCtx::create(rados_t cluster, exlib::string poolName)
{
    result_t hr;

    hr = load_librados();
    if (hr < 0)
        return hr;

    hr = _rados_ioctx_create(cluster, poolName.c_str(), &m_ioctx);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

RadosIoCtx::~RadosIoCtx()
{
    if (m_ioctx) {
        _rados_ioctx_destroy(m_ioctx);
        m_ioctx = NULL;
    }
}

#else
result_t RadosIoCtx::createImage(exlib::string name, int64_t size, int32_t stripe_unit, int32_t stripe_count)
{
    return 0;
}

result_t RadosIoCtx::cloneImage(exlib::string pName, exlib::string pSnapshot, RadosIoCtx_base* dstio, exlib::string cName, int64_t stripe_unit, int32_t stripe_count)
{
    return 0;
}
result_t RadosIoCtx::removeImage(exlib::string name)
{
    return 0;
}
result_t RadosIoCtx::renameImage(exlib::string src, exlib::string dst)
{
    return 0;
}
result_t RadosIoCtx::listImages(obj_ptr<List_base>& retVal)
{
    return 0;
}
result_t RadosIoCtx::openImage(exlib::string name, exlib::string snapshot, obj_ptr<RbdImage_base>& retVal)
{
    return 0;
}
result_t RadosIoCtx::version(exlib::string& retVal)
{
    return 0;
}
result_t RadosIoCtx::open(exlib::string key, obj_ptr<RadosStream_base>& retVal)
{
    return 0;
}
result_t RadosIoCtx::createSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::removeSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::listOids(obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::listOids(exlib::string pattern, obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::setXattr(exlib::string oid, exlib::string attr, exlib::string value, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac)
{
    return 0;
}

result_t RadosIoCtx::getXattrs(exlib::string oid, v8::Local<v8::Object>& retVal, AsyncEvent* ac)
{
    return 0;
}
result_t RadosIoCtx::remove(exlib::string key, AsyncEvent* ac)
{
    return 0;
}
result_t RadosIoCtx::destroy(AsyncEvent* ac)
{
    return 0;
}
result_t RadosIoCtx::create(rados_t cluster, exlib::string poolName)
{
    return 0;
}
RadosIoCtx::~RadosIoCtx()
{
}
#endif
}
