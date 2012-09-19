/*
 * MemoryStream.h
 *
 *  Created on: Jun 29, 2012
 *      Author: lion
 */

#include "ifs/os.h"
#include "ifs/fs.h"
#include "ifs/MemoryStream.h"
#include <sstream>

#ifndef MEMORYSTREAM_H_
#define MEMORYSTREAM_H_

namespace fibjs
{

class MemoryStream: public MemoryStream_base
{
	EVENT_SUPPORT();

public:
	class CloneStream: public MemoryStream_base
	{
		EVENT_SUPPORT();

	public:
		CloneStream(std::string buffer, date_t tm) :
			m_buffer(buffer), m_time(tm), m_pos(0)
		{
			extMemory((int)m_buffer.length());
		}

	public:
		// Stream_base
		virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
		virtual result_t asyncRead(int32_t bytes);
		virtual result_t onread(v8::Handle<v8::Function> func);
		virtual result_t write(Buffer_base* data, exlib::AsyncEvent* ac);
		virtual result_t asyncWrite(Buffer_base* data);
		virtual result_t onwrite(v8::Handle<v8::Function> func);
		virtual result_t close(exlib::AsyncEvent* ac);
		virtual result_t asyncClose();
		virtual result_t onclose(v8::Handle<v8::Function> func);
		virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, exlib::AsyncEvent* ac);
		virtual result_t asyncCopyTo(Stream_base* stm, int64_t bytes);
		virtual result_t oncopyto(v8::Handle<v8::Function> func);
		virtual result_t onerror(v8::Handle<v8::Function> func);

	public:
		// SeekableStream_base
		virtual result_t seek(int64_t offset, int32_t whence);
		virtual result_t tell(int64_t& retVal);
		virtual result_t rewind();
		virtual result_t size(int64_t& retVal);
		virtual result_t readAll(obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
		virtual result_t asyncReadAll();
		virtual result_t onreadall(v8::Handle<v8::Function> func);
		virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
		virtual result_t asyncStat();
		virtual result_t onstat(v8::Handle<v8::Function> func);

	public:
		// MemoryStream_base
		virtual result_t setTime(date_t d);
		virtual result_t clone(obj_ptr<MemoryStream_base>& retVal);
		virtual result_t clear();

	private:
		std::string m_buffer;
		date_t m_time;
		int m_pos;
	};

public:
	MemoryStream()
	{
		m_time.now();
	}

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncRead(int32_t bytes);
	virtual result_t onread(v8::Handle<v8::Function> func);
	virtual result_t write(Buffer_base* data, exlib::AsyncEvent* ac);
	virtual result_t asyncWrite(Buffer_base* data);
	virtual result_t onwrite(v8::Handle<v8::Function> func);
	virtual result_t close(exlib::AsyncEvent* ac);
	virtual result_t asyncClose();
	virtual result_t onclose(v8::Handle<v8::Function> func);
	virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncCopyTo(Stream_base* stm, int64_t bytes);
	virtual result_t oncopyto(v8::Handle<v8::Function> func);
	virtual result_t onerror(v8::Handle<v8::Function> func);

public:
	// SeekableStream_base
	virtual result_t seek(int64_t offset, int32_t whence);
	virtual result_t tell(int64_t& retVal);
	virtual result_t rewind();
	virtual result_t size(int64_t& retVal);
	virtual result_t readAll(obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncReadAll();
	virtual result_t onreadall(v8::Handle<v8::Function> func);
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncStat();
	virtual result_t onstat(v8::Handle<v8::Function> func);

public:
	// MemoryStream_base
	virtual result_t setTime(date_t d);
	virtual result_t clone(obj_ptr<MemoryStream_base>& retVal);
	virtual result_t clear();

private:
	std::stringstream m_buffer;
	date_t m_time;
};

} /* namespace fibjs */
#endif /* MEMORYSTREAM_H_ */
