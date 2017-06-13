/*
 * RadosIoCtx.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: asionius
 */

#include "object.h"
#include "RadosIoCtx.h"
#include "RadosStream.h"
#include "List.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {
#ifndef _WIN32

#define MAX_XATTRLEN 4096
extern void* get_librados_handle(void);
static void (*_rados_ioctx_destroy)(rados_ioctx_t );
static int (*_rados_ioctx_create)(rados_t , const char *, rados_ioctx_t *);
static int (*_rados_ioctx_snap_create)(rados_ioctx_t, const char *);
static int (*_rados_ioctx_snap_remove)(rados_ioctx_t, const char *);
static int (*_rados_ioctx_snap_rollback)(rados_ioctx_t, const char *, const char *);
static int (*_rados_objects_list_open)(rados_ioctx_t, rados_list_ctx_t *);
static int (*_rados_objects_list_next)(rados_list_ctx_t, const char **, const char **);
static void (*_rados_objects_list_close)(rados_list_ctx_t);
static int (*_rados_getxattr)(rados_ioctx_t, const char *, const char *, char *, size_t);
static int (*_rados_setxattr)(rados_ioctx_t, const char *, const char *, const char *, size_t);
static int (*_rados_rmxattr)(rados_ioctx_t, const char *, const char *);
static int (*_rados_getxattrs)(rados_ioctx_t, const char *, rados_xattrs_iter_t *);
static int (*_rados_getxattrs_next)(rados_xattrs_iter_t, const char **, const char **, size_t *);
static void (*_rados_getxattrs_end)(rados_xattrs_iter_t);

static inline int32_t load_librados(void) {
	static bool ldlib = false;
	void* handle = NULL;

	if (ldlib)
		return 0;

	handle = get_librados_handle();
	if (!handle)
		return CHECK_ERROR(CALL_E_INVALIDLIBRARYHANDLE);

	_rados_ioctx_destroy = (void(*)(rados_ioctx_t io))dlsym(handle, "rados_ioctx_destroy");
	_rados_ioctx_create = (int(*)(rados_t , const char *, rados_ioctx_t *))dlsym(handle, "rados_ioctx_create");
	_rados_ioctx_snap_create = (int(*)(rados_ioctx_t, const char *))dlsym(handle, "rados_ioctx_snap_create");
	_rados_ioctx_snap_remove = (int(*)(rados_ioctx_t, const char *))dlsym(handle, "rados_ioctx_snap_remove");
	_rados_ioctx_snap_rollback = (int(*)(rados_ioctx_t, const char *, const char *))dlsym(handle, "rados_ioctx_snap_rollback");
	_rados_objects_list_open = (int(*)(rados_ioctx_t, rados_list_ctx_t *))dlsym(handle, "rados_objects_list_open");
	_rados_objects_list_next = (int(*)(rados_list_ctx_t, const char **, const char **))dlsym(handle, "rados_objects_list_next");
	_rados_objects_list_close = (void(*)(rados_list_ctx_t))dlsym(handle, "rados_objects_list_close");
	_rados_getxattr = (int(*)(rados_ioctx_t, const char *, const char *, char *, size_t))dlsym(handle, "rados_getxattr");
	_rados_setxattr = (int(*)(rados_ioctx_t, const char *, const char *, const char *, size_t))dlsym(handle, "rados_setxattr");
	_rados_rmxattr = (int(*)(rados_ioctx_t, const char *, const char *))dlsym(handle, "rados_rmxattr");
	_rados_getxattrs = (int(*)(rados_ioctx_t, const char *, rados_xattrs_iter_t *))dlsym(handle, "rados_getxattrs");
	_rados_getxattrs_next = (int(*)(rados_xattrs_iter_t, const char **, const char **, size_t *))dlsym(handle, "rados_getxattrs_next");
	_rados_getxattrs_end = (void(*)(rados_xattrs_iter_t))dlsym(handle, "rados_getxattrs_end");

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
		return CHECK_ERROR(CALL_E_SYMBOLNOTFOUND);

	return 0;
}

result_t RadosIoCtx::open(exlib::string key, obj_ptr<RadosStream_base>& retVal)
{
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
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_ioctx_snap_create(m_ioctx, snapname.c_str());
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosIoCtx::removeSnap(exlib::string snapname, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_ioctx_snap_remove(m_ioctx, snapname.c_str());
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosIoCtx::rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_ioctx_snap_rollback(m_ioctx, oid.c_str(), snapname.c_str());
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosIoCtx::listOids(obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	rados_list_ctx_t ctx;
	obj_ptr<List> data = new List();
	exlib::string oid;
	const char* entry;

	hr = _rados_objects_list_open(m_ioctx, &ctx);
	if (hr < 0)
		return CHECK_ERROR(hr);

	while ((hr = _rados_objects_list_next(m_ioctx, &entry, NULL)) != -ENOENT)
	{
		if (hr < 0)
			return CHECK_ERROR(hr);
		oid = entry;
		data->append(oid);
	}

	_rados_objects_list_close(ctx);
	retVal = data;

	return 0;
}

result_t RadosIoCtx::listOids(Regex_base* reg, obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	rados_list_ctx_t ctx;
	obj_ptr<List> data = new List();
	exlib::string oid;
	bool match;
	const char* entry;

	hr = _rados_objects_list_open(m_ioctx, &ctx);
	if (hr < 0)
		return CHECK_ERROR(hr);

	while ((hr = _rados_objects_list_next(m_ioctx, &entry, NULL)) != -ENOENT)
	{
		if (hr < 0)
			return CHECK_ERROR(hr);
		oid = entry;
		reg->test(oid, match);
		if (match)
			data->append(oid);
	}

	_rados_objects_list_close(ctx);
	retVal = data;

	return 0;
}

result_t RadosIoCtx::getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac)
{
	if (!ac)
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
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_setxattr(m_ioctx, oid.c_str(), attr.c_str(), value.c_str(), value.length());
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosIoCtx::rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_rmxattr(m_ioctx, oid.c_str(), attr.c_str());
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosIoCtx::getXattrs(exlib::string oid, v8::Local<v8::Object>& retVal, AsyncEvent* ac)
{
	result_t hr;
	rados_xattrs_iter_t iter;
	Isolate* isolate = Isolate::current();
	retVal = v8::Object::New(isolate->m_isolate);

	hr = _rados_getxattrs(m_ioctx, oid.c_str(), &iter);
	if (hr < 0)
		return CHECK_ERROR(hr);

	while (1)
	{
		const char* name;
		const char* val;
		size_t len;

		hr = _rados_getxattrs_next(iter, &name, &val, &len);
		if (hr < 0)
			return CHECK_ERROR(hr);

		if (name == NULL)
			break;

		retVal->Set(isolate->NewFromUtf8(name), isolate->NewFromUtf8(val, len));
	}
	_rados_getxattrs_end(iter);

	return 0;
}

result_t RadosIoCtx::destroy(AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	if (m_ioctx)
	{
		_rados_ioctx_destroy(m_ioctx);
		m_ioctx = NULL;
	}
}

result_t RadosIoCtx::remove(exlib::string key, AsyncEvent* ac)
{
	if (!ac)
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
	if (m_ioctx)
	{
		_rados_ioctx_destroy(m_ioctx);
		m_ioctx = NULL;
	}
}

#else
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

result_t RadosIoCtx::listOids(Regex_base* reg, obj_ptr<List_base>& retVal, AsyncEvent* ac)
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
