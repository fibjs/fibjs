/*
 * MemoryCloneStream.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: lion
 */

#include "MemoryStream.h"
#include "Stream.h"
#include "Stat.h"
#include "Buffer.h"

namespace fibjs
{

result_t MemoryStream::CloneStream::read(int32_t bytes,
		obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac)
{
	std::string strBuf;

	int sz;
	sz = (int) m_buffer.length() - m_pos;

	if (bytes < 0 || bytes > sz)
		bytes = sz;

	if (bytes > 0)
	{
		if (m_pos == 0 && bytes == sz)
		{
			strBuf = m_buffer;
			m_pos = (int) m_buffer.length();
		}
		else
		{
			strBuf = m_buffer.substr(m_pos, bytes);
			m_pos += bytes;
		}
	}

	if (strBuf.length() == 0)
		return CALL_RETURN_NULL;

	retVal = new Buffer(strBuf);

	return 0;
}

result_t MemoryStream::CloneStream::asyncRead(int32_t bytes)
{
	acb_read(bytes);
	return 0;
}

result_t MemoryStream::CloneStream::onread(v8::Handle<v8::Function> func)
{
	return on("read", func);
}

result_t MemoryStream::CloneStream::readAll(obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	return read(-1, retVal, ac);
}

result_t MemoryStream::CloneStream::asyncReadAll()
{
	acb_readAll();
	return 0;
}

result_t MemoryStream::CloneStream::onreadall(v8::Handle<v8::Function> func)
{
	return on("readall", func);
}

result_t MemoryStream::CloneStream::write(Buffer_base* data,
		exlib::AsyncEvent* ac)
{
	return CALL_E_INVALID_CALL;
}

result_t MemoryStream::CloneStream::asyncWrite(Buffer_base* data)
{
	return CALL_E_INVALID_CALL;
}

result_t MemoryStream::CloneStream::onwrite(v8::Handle<v8::Function> func)
{
	return CALL_E_INVALID_CALL;
}

result_t MemoryStream::CloneStream::copyTo(Stream_base* stm, int64_t bytes,
		int64_t& retVal, exlib::AsyncEvent* ac)
{
	return copyStream(this, stm, bytes, retVal, ac);
}

result_t MemoryStream::CloneStream::asyncCopyTo(Stream_base* stm, int64_t bytes)
{
	acb_copyTo(stm, bytes);
	return 0;
}

result_t MemoryStream::CloneStream::oncopyto(v8::Handle<v8::Function> func)
{
	return on("copyto", func);
}

result_t MemoryStream::CloneStream::stat(obj_ptr<Stat_base>& retVal,
		exlib::AsyncEvent* ac)
{
	obj_ptr<Stat> st = new Stat();

	st->init();
	st->m_isMemory = true;
	st->mtime = st->ctime = m_time;
	size(st->size);

	retVal = st;

	return 0;
}

result_t MemoryStream::CloneStream::asyncStat()
{
	acb_stat();
	return 0;
}

result_t MemoryStream::CloneStream::onstat(v8::Handle<v8::Function> func)
{
	return on("stat", func);
}

result_t MemoryStream::CloneStream::onerror(v8::Handle<v8::Function> func)
{
	return on("error", func);
}

result_t MemoryStream::CloneStream::seek(int64_t offset, int32_t whence)
{
	if (whence == io_base::_SEEK_SET)
		m_pos = (int) offset;
	else if (whence == io_base::_SEEK_CUR)
		m_pos += (int) offset;
	else if (whence == io_base::_SEEK_END)
		m_pos = (int) offset + (int) m_buffer.length();
	else
		return CALL_E_INVALIDARG;

	if (m_pos < 0)
		m_pos = 0;
	else if (m_pos > (int) m_buffer.length())
		m_pos = (int) m_buffer.length();

	return 0;
}

result_t MemoryStream::CloneStream::tell(int64_t& retVal)
{
	retVal = m_pos;
	return 0;
}

result_t MemoryStream::CloneStream::rewind()
{
	m_pos = 0;
	return 0;
}

result_t MemoryStream::CloneStream::size(int64_t& retVal)
{
	retVal = m_buffer.length();
	return 0;
}

result_t MemoryStream::CloneStream::setTime(date_t d)
{
	return CALL_E_INVALID_CALL;
}

result_t MemoryStream::CloneStream::clone(obj_ptr<MemoryStream_base>& retVal)
{
	retVal = new CloneStream(m_buffer, m_time);
	return 0;
}

result_t MemoryStream::CloneStream::clear()
{
	rewind();
	m_buffer.clear();

	m_time.now();

	return 0;
}

}
