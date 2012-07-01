/*
 * MemoryStream.cpp
 *
 *  Created on: Jun 29, 2012
 *      Author: lion
 */

#include "MemoryStream.h"
#include "Stream.h"
#include "Stat.h"
#include "Buffer.h"

namespace fibjs
{

result_t MemoryStream_base::_new(obj_ptr<MemoryStream_base>& retVal)
{
	retVal = new MemoryStream();
	return 0;
}

result_t MemoryStream::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	std::string strBuf;

	int64_t sz;
	int64_t p = m_buffer.tellg();

	m_buffer.seekg(0, std::ios::end);
	sz = m_buffer.tellg() - p;
	m_buffer.seekg(p, std::ios::beg);

	if (bytes < 0 || bytes > sz)
		bytes = (int32_t) sz;

	if (bytes > 0)
	{
		if (p == 0)
		{
			strBuf = m_buffer.str();
			m_buffer.seekg(sz);
		}
		else
		{
			strBuf.resize(bytes);
			m_buffer.read(&strBuf[0], bytes);
		}

		m_buffer.seekp(m_buffer.tellg());
	}

	if (strBuf.length())
		retVal = new Buffer(strBuf);

	return 0;
}

result_t MemoryStream::asyncRead(int32_t bytes)
{
	acb_read(s_acPool, bytes);
	return 0;
}

result_t MemoryStream::onread(v8::Handle<v8::Function> func)
{
	return on("read", func);
}

result_t MemoryStream::write(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac)
{
	std::string strBuf;
	data->toString(strBuf);

	m_buffer.write(strBuf.c_str(), (int) strBuf.length());
	m_buffer.seekg(m_buffer.tellp());

	return 0;
}

result_t MemoryStream::asyncWrite(obj_ptr<Buffer_base>& data)
{
	acb_write(s_acPool, data);
	return 0;
}

result_t MemoryStream::onwrite(v8::Handle<v8::Function> func)
{
	return on("write", func);
}

result_t MemoryStream::copyTo(obj_ptr<Stream_base>& stm, int32_t bytes,
		int32_t& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	return copyStream(this, stm, bytes, retVal, ac);
}

result_t MemoryStream::asyncCopyTo(obj_ptr<Stream_base>& stm, int32_t bytes)
{
	acb_copyTo(s_acPool, stm, bytes);
	return 0;
}

result_t MemoryStream::oncopyto(v8::Handle<v8::Function> func)
{
	return on("copyto", func);
}

result_t MemoryStream::stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac)
{
	obj_ptr<Stat> st = new Stat();

	st->init();
	st->m_isMemory = true;
	size(st->size);

	retVal = st;

	return 0;
}

result_t MemoryStream::asyncStat()
{
	acb_stat(s_acPool);
	return 0;
}

result_t MemoryStream::onstat(v8::Handle<v8::Function> func)
{
	return on("stat", func);
}

result_t MemoryStream::onerror(v8::Handle<v8::Function> func)
{
	return on("error", func);
}

result_t MemoryStream::seek(double offset, int32_t whence)
{
	if (whence < io_base::_SEEK_SET || whence > io_base::_SEEK_END)
		return CALL_E_INVALIDARG;

	int64_t p = m_buffer.tellg();
	m_buffer.seekg(0, std::ios::end);
	int64_t sz = m_buffer.tellg();

	if (whence == io_base::_SEEK_CUR)
		offset += p;
	else if (whence == io_base::_SEEK_END)
		offset += sz;

	if (offset < 0)
		offset = 0;
	else if (offset > sz)
		offset = (double)sz;

	m_buffer.seekg((int64_t) offset);
	m_buffer.seekp((int64_t) offset);

	return 0;
}

result_t MemoryStream::tell(double& retVal)
{
	retVal = (double) m_buffer.tellg();
	return 0;
}

result_t MemoryStream::rewind()
{
	m_buffer.seekg(0);
	m_buffer.seekp(0);
	return 0;
}

result_t MemoryStream::size(double& retVal)
{
	int64_t p = m_buffer.tellg();

	m_buffer.seekg(0, std::ios::end);
	retVal = (double) m_buffer.tellg();
	m_buffer.seekg(p, std::ios::beg);

	return 0;
}

result_t MemoryStream::clear()
{
	rewind();
	m_buffer.str("");
	return 0;
}

} /* namespace fibjs */
