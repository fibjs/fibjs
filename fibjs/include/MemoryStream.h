/*
 * MemoryStream.h
 *
 *  Created on: Jun 29, 2012
 *      Author: lion
 */

#include "ifs/os.h"
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
	MemoryStream()
	{
		os_base::time(m_time);
	}

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncRead(int32_t bytes);
	virtual result_t onread(v8::Handle<v8::Function> func);
	virtual result_t write(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac);
	virtual result_t asyncWrite(obj_ptr<Buffer_base>& data);
	virtual result_t onwrite(v8::Handle<v8::Function> func);
	virtual result_t copyTo(obj_ptr<Stream_base>& stm, int32_t bytes, int32_t& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncCopyTo(obj_ptr<Stream_base>& stm, int32_t bytes);
	virtual result_t oncopyto(v8::Handle<v8::Function> func);
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncStat();
	virtual result_t onstat(v8::Handle<v8::Function> func);
	virtual result_t onerror(v8::Handle<v8::Function> func);

public:
	// MemoryStream_base
	virtual result_t seek(double offset, int32_t whence);
	virtual result_t tell(double& retVal);
	virtual result_t rewind();
	virtual result_t size(double& retVal);
	virtual result_t setTime(int64_t d);
	virtual result_t clear();

private:
	std::stringstream m_buffer;
	int64_t m_time;
};

} /* namespace fibjs */
#endif /* MEMORYSTREAM_H_ */
