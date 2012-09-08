/*
 * File.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "object.h"

#ifndef _WIN32
#include <sys/ioctl.h>
#endif

#include "File.h"
#include "Buffer.h"
#include "Stat.h"
#include "utf8.h"
#include "Stream.h"

namespace fibjs
{

result_t File::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	std::string strBuf;

	if (bytes < 0)
	{
		if (m_pipe)
		{
#ifndef _WIN32
			int sz;

			if (!ioctl(fileno(m_file), FIONREAD, &sz))
			{
				if (sz == 0)
					bytes = 1;
				else if (sz > STREAM_BUFF_SIZE)
					bytes = STREAM_BUFF_SIZE;
				else
					bytes = sz;
			}
			else
				bytes = 1;
#else
			bytes = 1;
#endif
		}
		else
		{
			int64_t sz;

			int64_t p = ftello64(m_file);
			if (0 == fseeko64(m_file, 0, SEEK_END))
			{
				sz = ftello64(m_file) - p;
				fseeko64(m_file, p, SEEK_SET);
			}
			else
				return LastError();

			if (sz > STREAM_BUFF_SIZE)
				sz = STREAM_BUFF_SIZE;

			bytes = (int32_t) sz;
		}
	}

	if (bytes > 0)
	{
		strBuf.resize(bytes);
		int sz = bytes;
		char* p = &strBuf[0];

		while (sz && !feof(m_file))
		{
			int n = (int) fread(p, 1, sz > 65536 ? 65536 : sz, m_file);
			if (n == 0 && ferror(m_file))
				return LastError();

			sz -= n;
			p += n;
		}

		strBuf.resize(bytes - sz);
	}

	if (strBuf.length() == 0)
		return CALL_RETURN_NULL;

	retVal = new Buffer(strBuf);

	return 0;
}

result_t File::asyncRead(int32_t bytes)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_read(bytes);
	return 0;
}

result_t File::onread(v8::Handle<v8::Function> func)
{
	return on("read", func);
}

result_t File::Write(const char* p, int sz)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	while (sz)
	{
		int n = (int) fwrite(p, 1, sz > 65536 ? 65536 : sz, m_file);
		if (n == 0 && ferror(m_file))
			return LastError();

		sz -= n;
		p += n;
	}

	if(m_pipe)
		fflush(m_file);

	return 0;
}

result_t File::write(Buffer_base* data, exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	std::string strBuf;
	data->toString(strBuf);

	return Write(strBuf.c_str(), (int) strBuf.length());
}

result_t File::asyncWrite(Buffer_base* data)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_write(data);
	return 0;
}

result_t File::onwrite(v8::Handle<v8::Function> func)
{
	return on("write", func);
}

result_t File::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal,
		exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	return copyStream(this, stm, bytes, retVal, ac);
}

result_t File::asyncCopyTo(Stream_base* stm, int64_t bytes)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_copyTo(stm, bytes);
	return 0;
}

result_t File::oncopyto(v8::Handle<v8::Function> func)
{
	return on("copyto", func);
}

result_t File::open(const char* fname, const char* mode, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

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

#ifndef _WIN32
#define _fileno fileno
#endif

result_t File::stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	struct stat64 st;
	fstat64(_fileno(m_file), &st);

	obj_ptr<Stat> pStat = new Stat();
	pStat->fill(name.c_str(), st);
	retVal = pStat;

	return 0;
}

result_t File::asyncStat()
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_stat();
	return 0;
}

result_t File::onstat(v8::Handle<v8::Function> func)
{
	return on("stat", func);
}

result_t File::size(int64_t& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	int64_t p = ftello64(m_file);
	if (0 == fseeko64(m_file, 0, SEEK_END))
	{
		retVal = ftello64(m_file);
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

result_t File::seek(int64_t offset, int32_t whence)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (fseeko64(m_file, offset, whence) < 0)
		return LastError();

	return 0;
}

result_t File::tell(int64_t& retVal)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	retVal = ftello64(m_file);

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

	if (!ac)
		return CALL_E_NOSYNC;

	fflush(m_file);

	if (ferror(m_file))
		return LastError();

	return 0;
}

result_t File::asyncFlush()
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_flush();
	return 0;
}

result_t File::onflush(v8::Handle<v8::Function> func)
{
	return on("flush", func);
}

result_t File::close(exlib::AsyncEvent* ac)
{
	if (m_file)
	{
		if (!ac)
			return CALL_E_NOSYNC;

		fclose(m_file);
		m_file = NULL;
	}

	return 0;
}

result_t File::asyncClose()
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_close();
	return 0;
}

result_t File::onclose(v8::Handle<v8::Function> func)
{
	return on("close", func);
}

result_t File::truncate(int64_t bytes, exlib::AsyncEvent* ac)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	if (ftruncate64(_fileno(m_file), bytes) < 0)
		return LastError();

	return 0;
}

result_t File::asyncTruncate(int64_t bytes)
{
	if (!m_file)
		return CALL_E_INVALID_CALL;

	acb_truncate(bytes);
	return 0;
}

result_t File::ontruncate(v8::Handle<v8::Function> func)
{
	return on("truncate", func);
}

result_t File::onerror(v8::Handle<v8::Function> func)
{
	return on("error", func);
}

}
