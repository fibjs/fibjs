/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "ifs/zlib.h"
#include "Buffer.h"
#include "zlib.h"
#include <sstream>

#define CHUNK 32768

namespace fibjs
{

class zlibWorker
{
public:
	zlibWorker()
	{
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
	}

public:
	result_t process(Buffer_base* data, obj_ptr<Buffer_base>& retVal)
	{
		unsigned have;
		unsigned char out[CHUNK];
		std::string strBuf;
		std::stringstream outBuf;

		if (init() != Z_OK)
			return CALL_E_OVERFLOW;

		data->toString(strBuf);

		strm.avail_in = (int)strBuf.length();
		strm.next_in = (unsigned char*) strBuf.c_str();

		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;

			if (put() != Z_OK)
				return CALL_E_INVALID_DATA;

			have = CHUNK - strm.avail_out;
			outBuf.write((const char*) out, have);
		} while (strm.avail_out == 0);

		end();

		retVal = new Buffer(outBuf.str());

		return 0;
	}

public:
	virtual int init()
	{
		return Z_OK;
	}

	virtual int put()
	{
		return Z_OK;
	}

	virtual void end()
	{
	}

protected:
	z_stream strm;
};

class def: public zlibWorker
{
public:
	def(int level = -1) :
			m_level(level)
	{
	}

public:
	virtual int init()
	{
		return deflateInit(&strm, m_level);
	}

	virtual int put()
	{
		::deflate(&strm, Z_FINISH);
		return Z_OK;
	}

	virtual void end()
	{
		deflateEnd(&strm);
	}

private:
	int m_level;
};

class inf: public zlibWorker
{
public:
	virtual int init()
	{
		return inflateInit(&strm);
	}

	virtual int put()
	{
		int ret = ::inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_STREAM_END)
		{
			inflateReset(&strm);
			return Z_OK;
		}
		return ret;
	}

	virtual void end()
	{
		inflateEnd(&strm);
	}
};

class gunz: public inf
{
public:
	virtual int init()
	{
		return inflateInit2(&strm, 15 + 16);
	}
};

class unz: public def
{
public:
	virtual int init()
	{
		return deflateInit2(&strm, -1, 8, 15 + 16, 8, 0);
	}
};

result_t zlib_base::deflate(Buffer_base* data, int32_t level,
		obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	return def(level).process(data, retVal);
}

result_t zlib_base::inflate(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	return inf().process(data, retVal);
}

result_t zlib_base::gunzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	return gunz().process(data, retVal);
}

result_t zlib_base::gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	return unz().process(data, retVal);
}

}

