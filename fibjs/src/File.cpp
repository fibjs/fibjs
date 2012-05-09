/*
 * File.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#include "File.h"
#include "Buffer.h"
#include "Stat.h"
#include "utf8.h"

namespace fibjs
{

result_t File::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	std::string strBuf;
	result_t hr;

	if (bytes < 0)
	{
		double sz;

		hr = this->size(sz);
		if (hr < 0)
			return hr;

		if (sz > 2147483647ll)
			return CALL_E_OVERFLOW;

		bytes = (int32_t) sz;
	}

	if (bytes > 0)
	{
		strBuf.resize(bytes);
		int sz = bytes;
		char* p = &strBuf[0];

		while (sz && !feof(m_file))
		{
			int n = (int) fread(p, 1, sz, m_file);
			if (n == 0 && ferror(m_file))
				return LastError();

			sz -= n;
			p += n;
		}

		strBuf.resize(bytes - sz);
	}

	retVal = new Buffer(strBuf);

	return 0;
}

result_t File::asyncRead(int32_t bytes)
{
	class _t: public AsyncCallBack
	{
	public:
		_t(AsyncQueue& q, File* pThis, int32_t v0) :
				AsyncCallBack(q, pThis, NULL, _stub), m_v0(v0)
		{
			pThis->Ref();
		}

		static void _stub(AsyncCall* ac)
		{
			_t* t = (_t*) ac;

			result_t hr = ((File*)t->m_pThis)->read(t->m_v0, t->retVal, t);
			if (hr != CALL_E_PENDDING)
				t->post(hr);
		}

		virtual void post(int v)
		{
			if(m_pThis->hasTrigger())
				AsyncCallBack::post(v);
			else
			{
				m_pThis->Unref();
				delete this;
			}
		}

		virtual void callback()
		{
			v8::Handle<v8::Value> v = ReturnValue(retVal);
			m_pThis->_trigger("read", &v, 1);

			m_pThis->Unref();
			delete this;
		}

	private:
		obj_ptr<Buffer_base> retVal;
		int32_t m_v0;
	};

	new _t(s_acPool, this, bytes);

	return 0;
}

result_t File::Write(const char* p, int sz)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	while (sz)
	{
		int n = (int) fwrite(p, 1, sz, m_file);
		if (n == 0 && ferror(m_file))
			return LastError();

		sz -= n;
		p += n;
	}

	return 0;
}

result_t File::write(obj_ptr<Buffer_base> data, exlib::AsyncEvent* ac)
{
	std::string strBuf;
	data->toString(strBuf);

	return Write(strBuf.c_str(), (int) strBuf.length());
}

result_t File::Open(const char* fname, const char* mode)
{
#ifdef _WIN32
	wchar_t m[] = L"rb\0";
#else
	char m[] = "rb\0";
#endif

	if ((*mode != 'a' && *mode != 'r' && *mode != 'w')
			|| (mode[1] != '+' && mode[1]))
		return CALL_E_INVALIDARG;

	m[0] = mode[0];
	m[2] = mode[1];

	close(NULL);

#ifdef _WIN32
	m_file = _wfopen(UTF8_W(fname), m);
#else
	m_file = fopen(fname, m);
#endif
	if (m_file == NULL)
		return LastError();

	name = fname;

	return 0;
}

result_t File::get_name(std::string& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = name;
	return 0;
}

result_t File::stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	struct stat64 st;
	fstat64(fileno(m_file), &st);

	obj_ptr<Stat> pStat = new Stat();
	pStat->fillStat(name.c_str(), st);
	retVal = pStat;

	return 0;
}

result_t File::size(double& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	int64_t p = ftello64(m_file);
	if (0 == fseeko64(m_file, 0, SEEK_END))
	{
		retVal = (double) ftello64(m_file);
		fseeko64(m_file, p, SEEK_SET);
	}
	else
		return LastError();

	return 0;
}

result_t File::eof(bool& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = feof(m_file) != 0;

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::seek(double offset, int32_t whence)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (fseeko64(m_file, (int64_t) offset, whence) < 0)
		return LastError();

	return 0;
}

result_t File::tell(double& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = (double) ftello64(m_file);

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::rewind()
{
	return seek(0, SEEK_SET);
}

result_t File::flush(exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	fflush(m_file);

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::close(exlib::AsyncEvent* ac)
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}

	return 0;
}

result_t File::truncate(double bytes, exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (ftruncate64(fileno(m_file), (int64_t) bytes) < 0)
		return LastError();

	return 0;
}

}
