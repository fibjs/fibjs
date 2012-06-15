/*
 * BufferedReader.h
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "ifs/BufferedReader.h"
#include "vector"

#ifndef BUFFEREDREADER_H_
#define BUFFEREDREADER_H_

namespace fibjs
{

class BufferedReader: public fibjs::BufferedReader_base
{
public:
	BufferedReader(Stream_base* stm) :
			m_stm(stm), m_pos(0), m_size(0)
	{
	}

	virtual ~BufferedReader()
	{
	}

public:
	// BufferedReader_base
	virtual result_t readText(int32_t size, std::string& retVal, exlib::AsyncEvent* ac);
	virtual result_t readLine(std::string& retVal, exlib::AsyncEvent* ac);
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t readUntil(const char* mk, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);

public:
	obj_ptr<Stream_base> m_stm;
	std::vector<std::string> m_bufs;
	int m_pos, m_size;
};

} /* namespace fibjs */
#endif /* BUFFEREDREADER_H_ */
