/*
 * RadosStream.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: asionius
 */

#include "object.h"
#include "RadosStream.h"
#include "RadosStat.h"
#include "Stat.h"
#include "Buffer.h"
#include <exlib/include/fiber.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {

#ifndef _WIN32
extern void* get_librados_handle(void);

static int (*_rados_aio_create_completion)( void *, rados_callback_t,
        rados_callback_t,
        rados_completion_t *);
static int (*_rados_aio_write)(rados_ioctx_t, const char *,
                               rados_completion_t, const char *,
                               size_t, uint64_t);
static int (*_rados_aio_append)(rados_ioctx_t, const char *,
                                rados_completion_t,
                                const char *, size_t);
static int (*_rados_aio_write_full)(rados_ioctx_t, const char *,
                                    rados_completion_t,
                                    const char *, size_t);

static int (*_rados_aio_read)(rados_ioctx_t , const char *,
                              rados_completion_t,
                              char *, size_t, uint64_t );
static int (*_rados_aio_flush)(rados_ioctx_t);
static int (*_rados_aio_flush_async)(rados_ioctx_t, rados_completion_t);
static int (*_rados_aio_stat)(rados_ioctx_t, const char *,
                              rados_completion_t,
                              uint64_t *, time_t *);
static int (*_rados_aio_get_return_value)(rados_completion_t);
static void (*_rados_aio_release)(rados_completion_t);
static int (*_rados_trunc)(rados_ioctx_t, const char *, uint64_t);
static int (*_rados_aio_remove)(rados_ioctx_t , const char *,
                                rados_completion_t);

static inline int32_t load_librados(void)
{
	static bool ldlib = false;
	void* handle = NULL;

	if (ldlib)
		return 0;

	handle = get_librados_handle();
	if (!handle)
		return CHECK_ERROR(CALL_E_INVALIDLIBRARYHANDLE);

	_rados_aio_create_completion = (int(*)( void *, rados_callback_t,
	                                        rados_callback_t,
	                                        rados_completion_t *))dlsym(handle, "rados_aio_create_completion");
	_rados_aio_write = (int(*)(rados_ioctx_t, const char *,
	                           rados_completion_t, const char *,
	                           size_t, uint64_t))dlsym(handle, "rados_aio_write");
	_rados_aio_append = (int(*)(rados_ioctx_t, const char *,
	                            rados_completion_t,
	                            const char *, size_t))dlsym(handle, "rados_aio_append");
	_rados_aio_write_full = (int(*)(rados_ioctx_t, const char *,
	                                rados_completion_t,
	                                const char *, size_t))dlsym(handle, "rados_aio_write_full");
	_rados_aio_read = (int(*)(rados_ioctx_t , const char *,
	                          rados_completion_t,
	                          char *, size_t, uint64_t ))dlsym(handle, "rados_aio_read");
	_rados_aio_flush = (int(*)(rados_ioctx_t))dlsym(handle, "rados_aio_flush");
	_rados_aio_flush_async = (int(*)(rados_ioctx_t, rados_completion_t))dlsym(handle, "rados_aio_flush_async");
	_rados_aio_stat = (int(*)(rados_ioctx_t, const char *,
	                          rados_completion_t,
	                          uint64_t *, time_t *))dlsym(handle, "rados_aio_stat");
	_rados_aio_get_return_value = (int(*)(rados_completion_t))dlsym(handle, "rados_aio_get_return_value");
	_rados_aio_release = (void(*)(rados_completion_t))dlsym(handle, "rados_aio_release");
	_rados_trunc = (int(*)(rados_ioctx_t, const char *, uint64_t))dlsym(handle, "rados_trunc");
	_rados_aio_remove = (int(*)(rados_ioctx_t , const char *,
	                            rados_completion_t))dlsym(handle, "rados_aio_remove");

	if (!_rados_aio_create_completion ||
	        !_rados_aio_write ||
	        !_rados_aio_append ||
	        !_rados_aio_write_full ||
	        !_rados_aio_read ||
	        !_rados_aio_flush ||
	        !_rados_aio_flush_async ||
	        !_rados_aio_stat ||
	        !_rados_aio_get_return_value ||
	        !_rados_aio_release ||
	        !_rados_aio_remove ||
	        !_rados_trunc)
		return CHECK_ERROR(CALL_E_SYMBOLNOTFOUND);

	return 0;
}

class asyncRadosCallback : public exlib::Task_base {
public:
	asyncRadosCallback(RadosStream *pThis, AsyncEvent* ac, exlib::Locker& locker)
		: m_comp(NULL)
		, m_pThis(pThis)
		, m_ac(ac)
		, m_locker(locker)
	{
	}
	virtual ~asyncRadosCallback()
	{
		if (m_comp)
		{
			_rados_aio_release(m_comp);
			m_comp = NULL;
		}
	}

public:
	virtual void suspend()
	{
	}

	virtual void suspend(exlib::spinlock& lock)
	{
		lock.unlock();
	}

	virtual void resume()
	{
	}

public:
	result_t call()
	{
		if (m_locker.lock(this)) {
			result_t hr = process();
			if (hr != CALL_E_PENDDING) {
				m_locker.unlock(this);
				delete this;

				return hr;
			}
		}

		return CALL_E_PENDDING;
	}

	virtual int32_t process()
	{
		return 0;
	}

	virtual void proc()
	{
		ready(process());
	}

	result_t before()
	{
		result_t hr;

		if (!m_comp)
		{
			hr = _rados_aio_create_completion((void*)this, complete_callback, NULL, &m_comp);
			if (hr < 0)
				return CHECK_ERROR(hr);
		}

		return 0;
	}

	void ready(int32_t v)
	{
		m_locker.unlock(this);
		m_ac->apost(v);
		delete this;
	}
	void onready()
	{
		proc();
	}

public:
	static void complete_callback(rados_completion_t comp, void *arg) {
		((asyncRadosCallback*)arg)->onready();
	}

public:
	rados_completion_t m_comp;
	obj_ptr<RadosStream> m_pThis;
	AsyncEvent* m_ac;
	exlib::Locker& m_locker;
};

result_t RadosStream::read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
	class  asyncRead: public asyncRadosCallback
	{
	public:
		asyncRead(int32_t bytes, obj_ptr<Buffer_base>& retVal, RadosStream *pThis,
		          AsyncEvent* ac, exlib::Locker& locker)
			: asyncRadosCallback(pThis, ac, locker)
			, m_bytes(bytes)
			, m_retVal(retVal)
		{
		}
		virtual result_t process()
		{
			result_t hr;

			hr = before();
			if (hr < 0)
				return hr;

			m_buf.resize(m_bytes);
			char* p = &m_buf[0];

			hr = _rados_aio_read(m_pThis->m_ioctx, m_pThis->m_key.c_str(), m_comp, p, m_bytes, m_pThis->m_off.value());
			if (hr < 0)
				return CHECK_ERROR(hr);

			return CALL_E_PENDDING;
		}

		virtual void proc()
		{
			int32_t nRead;

			nRead = _rados_aio_get_return_value(m_comp);
			if (nRead < 0)
			{
				CHECK_ERROR(nRead);
				ready(nRead);
			}
			else if (nRead == 0)
				ready(CALL_RETURN_NULL);
			else {
				m_pThis->m_off.add(nRead);
				m_buf.resize(nRead);
				m_retVal = new Buffer(m_buf);
				ready(0);
			}
		}
	public:
		int64_t m_bytes;
		exlib::string m_buf;
		obj_ptr<Buffer_base>& m_retVal;
	};

	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	int64_t lbytes = bytes;

	if (lbytes < 0)
	{
		result_t hr;

		obj_ptr<RadosStat_base> stat;
		hr = cc_radosStat(stat);
		if (hr < 0)
			return hr;

		stat->get_size(lbytes);
	}

	return (new asyncRead(lbytes, retVal, this, ac, m_lockRead))->call();
}

result_t RadosStream::write(Buffer_base* data, AsyncEvent* ac)
{
	class  asyncWrite: public asyncRadosCallback
	{
	public:
		asyncWrite(exlib::string data, RadosStream *pThis,
		           AsyncEvent* ac, exlib::Locker& locker)
			: asyncRadosCallback(pThis, ac, locker)
			, m_data(data)
		{
		}
		virtual result_t process()
		{
			result_t hr;

			hr = before();
			if (hr < 0)
				return hr;

			hr = _rados_aio_write(m_pThis->m_ioctx, m_pThis->m_key.c_str(), m_comp, m_data.c_str(), m_data.length(), m_pThis->m_off.value());
			if (hr < 0)
				return CHECK_ERROR(hr);

			return CALL_E_PENDDING;
		}

		virtual void proc()
		{
			result_t hr;

			hr = _rados_aio_get_return_value(m_comp);
			if (hr < 0)
			{
				CHECK_ERROR(hr);
				ready(hr);
			}
			else
			{
				m_pThis->m_off.add(m_data.length());
				ready(0);
			}
		}
	public:
		exlib::string m_data;
	};

	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	exlib::string strBuf;
	data->toString(strBuf);

	return (new asyncWrite(strBuf, this, ac, m_lockWrite))->call();
}

result_t RadosStream::close(AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	close();
	return 0;
}

result_t RadosStream::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
{
	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	obj_ptr<Buffer_base> buf;
	result_t hr;
	int64_t offset;

	if (bytes < 0)
		hr = cc_read(-1, buf);
	else
		hr = cc_read(bytes, buf);
	if (hr < 0)
		return hr;

	hr = stm->cc_write(buf);
	if (hr < 0)
		return hr;

	int32_t len;
	buf->get_length(len);
	retVal = len;

	return 0;

}

result_t RadosStream::seek(int64_t offset, int32_t whence)
{
	result_t hr;
	int64_t size;
	obj_ptr<RadosStat_base> stat;

	hr = cc_radosStat(stat);
	if (hr < 0)
		return hr;

	stat->get_size(size);

	if (whence == SEEK_SET)
		m_off = offset;
	else if (whence == SEEK_CUR)
		m_off.add(offset);
	else if (whence == SEEK_END)
		m_off = size + offset;
	else
		return CHECK_ERROR(-EINVAL);

	if (m_off.value() < 0)
		return CHECK_ERROR(-EINVAL);

	if (m_off.value() > size)
		m_off = size;

	return 0;
}

result_t RadosStream::tell(int64_t& retVal)
{
	retVal = m_off.value();
	return 0;
}

result_t RadosStream::rewind()
{
	m_off = 0;
	return 0;
}

result_t RadosStream::size(int64_t& retVal)
{
	result_t hr;
	obj_ptr<RadosStat_base> stat;
	int64_t size;

	hr = cc_radosStat(stat);
	if (hr < 0)
		return hr;

	stat->get_size(size);
	retVal = size;

	return 0;
}

result_t RadosStream::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	m_off = 0;

	hr = cc_read(-1, retVal);
	if (hr < 0)
		return hr;

	return 0;
}

result_t RadosStream::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;
	obj_ptr<Stat> st = new Stat();
	obj_ptr<RadosStat_base> stat;

	hr = cc_radosStat(stat);
	if (hr < 0)
		return hr;

	st->init();
	stat->get_size(st->size);
	stat->get_mtime(st->mtime);
	st->ctime = st->mtime;

	retVal = st;

	return 0;
}

result_t RadosStream::get_key(exlib::string& retVal)
{
	retVal = m_key;
	return 0;
}

result_t RadosStream::radosStat(obj_ptr<RadosStat_base>& retVal, AsyncEvent* ac)
{
	class  asyncGetStat: public asyncRadosCallback
	{
	public:
		asyncGetStat(obj_ptr<RadosStat_base> &retVal, RadosStream* pThis,
		             AsyncEvent* ac, exlib::Locker& locker)
			: asyncRadosCallback(pThis, ac, locker)
			, m_size(0)
			, m_mtime(0)
			, m_retVal(retVal)
		{
		}
		virtual result_t process()
		{
			result_t hr;

			hr = before();
			if (hr < 0)
				return hr;

			hr = _rados_aio_stat(m_pThis->m_ioctx, m_pThis->m_key.c_str(), m_comp, (uint64_t*)&m_size, &m_mtime);
			if (hr < 0)
				return CHECK_ERROR(hr);

			return CALL_E_PENDDING;
		}

		virtual void proc()
		{
			int32_t nRead;

			nRead = _rados_aio_get_return_value(m_comp);
			if (nRead < 0)
			{
				CHECK_ERROR(nRead);
				ready(nRead);
			}
			else {
				m_retVal = new RadosStat(m_size, (double)m_mtime * 1000ll);
				ready(0);
			}
		}
	public:
		int64_t m_size;
		time_t m_mtime;
		obj_ptr<RadosStat_base>& m_retVal;
	};

	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	return (new asyncGetStat(retVal, this, ac, m_lockRead))->call();
}

result_t RadosStream::writeFull(Buffer_base* data, AsyncEvent* ac)
{
	class asyncWriteFull : public asyncRadosCallback
	{
	public:
		asyncWriteFull(exlib::string data, RadosStream *pThis,
		               AsyncEvent* ac, exlib::Locker& locker)
			: asyncRadosCallback(pThis, ac, locker)
			, m_data(data)
		{
		}
		virtual result_t process()
		{
			result_t hr;

			hr = before();
			if (hr < 0)
				return hr;

			hr = _rados_aio_write_full(m_pThis->m_ioctx, m_pThis->m_key.c_str(), m_comp, m_data.c_str(), m_data.length());
			if (hr < 0)
				return CHECK_ERROR(hr);

			return CALL_E_PENDDING;
		}

		virtual void proc()
		{
			result_t hr;

			hr = _rados_aio_get_return_value(m_comp);
			if (hr < 0)
			{
				CHECK_ERROR(hr);
				ready(hr);
			}
			else
			{
				m_pThis->m_off = (m_data.length());
				ready(0);
			}
		}
	public:
		exlib::string m_data;
	};

	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	exlib::string strBuf;
	data->toString(strBuf);

	return (new asyncWriteFull(strBuf, this, ac, m_lockWrite))->call();
}

result_t RadosStream::append(Buffer_base* data, AsyncEvent* ac)
{
	class asyncAppend : public asyncRadosCallback
	{
	public:
		asyncAppend(exlib::string data, RadosStream *pThis,
		            AsyncEvent* ac, exlib::Locker& locker)
			: asyncRadosCallback(pThis, ac, locker)
			, m_data(data)
		{
		}
		virtual result_t process()
		{
			result_t hr;

			hr = before();
			if (hr < 0)
				return hr;

			hr = _rados_aio_append(m_pThis->m_ioctx, m_pThis->m_key.c_str(), m_comp, m_data.c_str(), m_data.length());
			if (hr < 0)
				return CHECK_ERROR(hr);

			return CALL_E_PENDDING;
		}

		virtual void proc()
		{
			result_t hr;

			hr = _rados_aio_get_return_value(m_comp);
			if (hr < 0)
			{
				CHECK_ERROR(hr);
				ready(hr);
			}
			else
			{
				m_pThis->m_off.add(m_data.length());
				ready(0);
			}
		}
	public:
		exlib::string m_data;
	};

	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	exlib::string strBuf;

	seek(0, SEEK_END);
	data->toString(strBuf);

	return (new asyncAppend(strBuf, this, ac, m_lockWrite))->call();
}

result_t RadosStream::truncate(int64_t bytes, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_trunc(m_ioctx, m_key.c_str(), bytes);
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosStream::flush(AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	result_t hr;

	hr = _rados_aio_flush(m_ioctx);
	if (hr < 0)
		return CHECK_ERROR(hr);

	return 0;
}

result_t RadosStream::open(rados_ioctx_t io, exlib::string key)
{
	result_t hr;

	hr = load_librados();
	if (hr < 0)
		return hr;

	m_off = 0;
	m_ioctx = io;
	m_key = key;

	return 0;
}

result_t RadosStream::remove(AsyncEvent* ac)
{
	class asyncRemove : public asyncRadosCallback
	{
	public:
		asyncRemove(exlib::string key, RadosStream *pThis,
		            AsyncEvent* ac, exlib::Locker& locker)
			: asyncRadosCallback(pThis, ac, locker)
			, m_key(key)
		{
		}
		virtual result_t process()
		{
			result_t hr;

			hr = before();
			if (hr < 0)
				return hr;

			hr = _rados_aio_remove(m_pThis->m_ioctx, m_key.c_str(), m_comp);
			if (hr < 0)
				return CHECK_ERROR(hr);

			return CALL_E_PENDDING;
		}

		virtual void proc()
		{
			result_t hr;

			hr = _rados_aio_get_return_value(m_comp);
			if (hr < 0)
			{
				CHECK_ERROR(hr);
				ready(hr);
			}
			else
				ready(0);
		}
	public:
		exlib::string m_key;
	};

	if (!m_ioctx)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	return (new asyncRemove(m_key, this, ac, m_lockWrite))->call();
}

#else
result_t RadosStream::read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::write(Buffer_base* data, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::close(AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::seek(int64_t offset, int32_t whence)
{
	return 0;
}
result_t RadosStream::tell(int64_t& retVal)
{
	return 0;
}
result_t RadosStream::rewind()
{
	return 0;
}
result_t RadosStream::size(int64_t& retVal)
{
	return 0;
}
result_t RadosStream::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::writeFull(Buffer_base* data, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::get_key(exlib::string& retVal)
{
	return 0;
}
result_t RadosStream::radosStat(obj_ptr<RadosStat_base>& retVal, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::truncate(int64_t bytes, AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::flush(AsyncEvent* ac)
{
	return 0;
}
result_t RadosStream::open(rados_ioctx_t io, exlib::string key)
{
	return 0;
}
result_t RadosStream::remove()
{
	return 0;
}
#endif
}
