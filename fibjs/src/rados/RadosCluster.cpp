/*
 * RadosCluster.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: asionius
 */

#include "object.h"
#include "RadosCluster.h"
#include "RadosIoCtx.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {

#ifndef _WIN32

static void* s_handle = NULL;
static bool ldlib = false;
static int (*_rados_create2)(rados_t*, const char* const,
    const char* const, uint64_t);
static int (*_rados_conf_read_file)(rados_t, const char*);
static int (*_rados_connect)(rados_t);
static int (*_rados_pool_create)(rados_t, const char*);
static int (*_rados_pool_create_with_auid)(rados_t, const char*, uint64_t);
static int (*_rados_pool_create_with_crush_rule)(rados_t, const char*, uint8_t);
static int (*_rados_pool_create_with_all)(rados_t, const char*, uint64_t, uint8_t);
static int (*_rados_pool_list)(rados_t, char*, size_t);
static int (*_rados_pool_delete)(rados_t, const char*);
static int (*_rados_ioctx_create)(rados_t, const char*, rados_ioctx_t*);
static void (*_rados_shutdown)(rados_t);

void* get_librados_handle(void)
{
    return s_handle;
}

static inline int32_t load_librados()
{
    if (ldlib)
        return 0;

    ldlib = true;
    s_handle = dlopen("librados.so", RTLD_LAZY);
    if (!s_handle)
        return CHECK_ERROR(Runtime::setError("Cannot load library: 'librados.so'"));

    _rados_create2 = (int (*)(rados_t*, const char* const,
        const char* const, uint64_t))dlsym(s_handle, "rados_create2");
    _rados_conf_read_file = (int (*)(rados_t, const char*))dlsym(s_handle, "rados_conf_read_file");
    _rados_connect = (int (*)(rados_t))dlsym(s_handle, "rados_connect");
    _rados_pool_create = (int (*)(rados_t, const char*))dlsym(s_handle, "rados_pool_create");
    _rados_pool_create_with_auid = (int (*)(rados_t, const char*, uint64_t))dlsym(s_handle, "rados_pool_create_with_auid");
    _rados_pool_create_with_crush_rule = (int (*)(rados_t, const char*, uint8_t))dlsym(s_handle, "rados_pool_create_with_crush_rule");
    _rados_pool_create_with_all = (int (*)(rados_t, const char*, uint64_t, uint8_t))dlsym(s_handle, "rados_pool_create_with_all");
    _rados_pool_list = (int (*)(rados_t, char*, size_t))dlsym(s_handle, "rados_pool_list");
    _rados_pool_delete = (int (*)(rados_t, const char*))dlsym(s_handle, "rados_pool_delete");
    _rados_ioctx_create = (int (*)(rados_t, const char*, rados_ioctx_t*))dlsym(s_handle, "rados_ioctx_create");
    _rados_shutdown = (void (*)(rados_t))dlsym(s_handle, "rados_shutdown");

    if (!_rados_create2
        || !_rados_conf_read_file
        || !_rados_connect
        || !_rados_pool_create
        || !_rados_pool_create_with_auid
        || !_rados_pool_create_with_crush_rule
        || !_rados_pool_create_with_all
        || !_rados_pool_list
        || !_rados_pool_delete
        || !_rados_ioctx_create)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));
    return 0;
}

inline void close_librados()
{
    if (s_handle) {
        dlclose(s_handle);
        ldlib = false;
        s_handle = NULL;
    }
}

result_t RadosCluster_base::_new(exlib::string clusterName, exlib::string userName, exlib::string confPath, obj_ptr<RadosCluster_base>& retVal, v8::Local<v8::Object> This)
{
    result_t hr;

    hr = load_librados();
    if (hr < 0)
        return hr;
    obj_ptr<RadosCluster> cluster = new RadosCluster();

    hr = cluster->create(clusterName, userName, confPath);
    if (hr < 0)
        return hr;

    retVal = cluster;
    return 0;
}

result_t RadosCluster::create(exlib::string cn, exlib::string un, exlib::string path)
{
    result_t hr;

    hr = _rados_create2(&m_cluster, cn.c_str(), un.c_str(), 0);
    if (hr < 0)
        return CHECK_ERROR(hr);

    hr = _rados_conf_read_file(m_cluster, path.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosCluster::connect(AsyncEvent* ac)
{
    result_t hr;

    hr = _rados_connect(m_cluster);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosCluster::createPool(exlib::string poolName, int64_t auid, int32_t crushRule, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_pool_create_with_all(m_cluster, poolName.c_str(), auid, crushRule);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosCluster::listPool(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t size;
    exlib::string str;
    char *c, *t;
    obj_ptr<NArray> lp;

    lp = new NArray();
    size = _rados_pool_list(m_cluster, NULL, 0);
    str.resize(size);
    c = &str[0];
    _rados_pool_list(m_cluster, c, str.length());

    while (c[0] != '\0') {
        t = c;
        while (*(++t) != '\0')
            ;
        exlib::string pool(c, t - c);
        lp->append(pool);
        c = t + 1;
    }
    retVal = lp;
    return 0;
}

result_t RadosCluster::deletePool(exlib::string poolName, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = _rados_pool_delete(m_cluster, poolName.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RadosCluster::createIoCtx(exlib::string poolName, obj_ptr<RadosIoCtx_base>& retVal, AsyncEvent* ac)
{
    result_t hr;

    obj_ptr<RadosIoCtx> ioCtx = new RadosIoCtx();

    hr = ioCtx->create(m_cluster, poolName);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = ioCtx;
    return 0;
}

result_t RadosCluster::shutdown(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_cluster)
        _rados_shutdown(m_cluster);
    return 0;
}

#else
result_t RadosCluster::connect(AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}
result_t RadosCluster::createIoCtx(exlib::string poolName, obj_ptr<RadosIoCtx_base>& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}
result_t RadosCluster::shutdown(AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}
result_t RadosCluster_base::_new(exlib::string clusterName, exlib::string userName, exlib::string confPath, obj_ptr<RadosCluster_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>())
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t RadosCluster::create(exlib::string cn, exlib::string un, exlib::string path)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}
#endif

RadosCluster::~RadosCluster()
{
    close_librados();
    if (m_cluster)
        _rados_shutdown(m_cluster);
}
}
