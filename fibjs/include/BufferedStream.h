/*
 * BufferedStream.h
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "ifs/BufferedStream.h"
#include "vector"

#ifndef BUFFEREDSTREAM_H_
#define BUFFEREDSTREAM_H_

namespace fibjs
{

class BufferedStream: public fibjs::BufferedStream_base
{
public:
	BufferedStream(Stream_base* stm) :
			m_stm(stm), m_pos(0), m_size(0)
	{
	}

	virtual ~BufferedStream()
	{
	}

	EVENT_SUPPORT();

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncRead(int32_t bytes);
	virtual result_t onread(v8::Handle<v8::Function> func);
	virtual result_t write(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac);
	virtual result_t asyncWrite(obj_ptr<Buffer_base>& data);
	virtual result_t onwrite(v8::Handle<v8::Function> func);
	virtual result_t copyTo(obj_ptr<Stream_base>& stm, int64_t bytes, int64_t& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncCopyTo(obj_ptr<Stream_base>& stm, int64_t bytes);
	virtual result_t oncopyto(v8::Handle<v8::Function> func);
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncStat();
	virtual result_t onstat(v8::Handle<v8::Function> func);
	virtual result_t onerror(v8::Handle<v8::Function> func);

public:
	// BufferedStream_base
	virtual result_t readText(int32_t size, std::string& retVal, exlib::AsyncEvent* ac);
	virtual result_t readLine(std::string& retVal, exlib::AsyncEvent* ac);
	virtual result_t readUntil(int32_t mk, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t get_EOL(std::string& retVal);
	virtual result_t set_EOL(const char* newVal);

public:
	obj_ptr<Stream_base> m_stm;
	std::vector<std::string> m_bufs;
	int m_pos, m_size;
};

} /* namespace fibjs */
#endif /* BUFFEREDSTREAM_H_ */
