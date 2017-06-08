/*
 * RadosIoCtx.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: asionius
 */

#include "object.h"
#include "RadosIoCtx.h"
#include "RadosStream.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {
#ifndef _WIN32
extern void* get_librados_handle(void);
static void (*_rados_ioctx_destroy)(rados_ioctx_t );
static int (*_rados_ioctx_create)(rados_t , const char *, rados_ioctx_t *);

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

	if (!_rados_ioctx_destroy ||
	        !_rados_ioctx_create)
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
