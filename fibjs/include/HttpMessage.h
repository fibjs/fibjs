/*
 * HttpMessage.h
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "HttpCollection.h"
#include "MemoryStream.h"

#ifndef HTTPMESSAGE_H_
#define HTTPMESSAGE_H_

namespace fibjs
{

class HttpMessage
{
public:
	HttpMessage()
	{
		m_headers = new HttpCollection();
		clear();
	}

public:
	result_t get_protocol(std::string& retVal);
	result_t set_protocol(const char* newVal);
	result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
	result_t get_body(obj_ptr<SeekableStream_base>& retVal);
	result_t set_body(obj_ptr<SeekableStream_base>& newVal);
	result_t get_contentType(std::string& retVal);
	result_t set_contentType(const char* newVal);
	result_t get_contentLength(int64_t& retVal);
	result_t get_keepAlive(bool& retVal);
	result_t set_keepAlive(bool newVal);
	result_t clear();

public:
	void addHeader(const char* name, int szName, const char* value, int szValue)
	{
		m_headers->add(name, szName, value, szValue);
	}

public:
	std::string m_protocol;
	std::string m_contentType;
	bool m_keepAlive;
	obj_ptr<HttpCollection> m_headers;
	obj_ptr<SeekableStream_base> m_body;
};

} /* namespace fibjs */
#endif /* HTTPMESSAGE_H_ */
