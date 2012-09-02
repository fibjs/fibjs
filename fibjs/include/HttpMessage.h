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
	HttpMessage(bool bResponse = false) :
			m_bResponse(bResponse)
	{
		m_headers = new HttpCollection();
		clear();
	}

public:
	result_t get_protocol(std::string& retVal);
	result_t set_protocol(const char* newVal);
	result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
	result_t get_body(obj_ptr<SeekableStream_base>& retVal);
	result_t set_body(SeekableStream_base* newVal);
	result_t get_contentLength(int64_t& retVal);
	result_t get_keepAlive(bool& retVal);
	result_t set_keepAlive(bool newVal);
	result_t hasHeader(const char* name, bool& retVal);
	result_t firstHeader(const char* name, Variant& retVal);
	result_t allHeader(const char* name, v8::Handle<v8::Array>& retVal);
	result_t addHeader(v8::Handle<v8::Object> map);
	result_t addHeader(const char* name, Variant value);
	result_t setHeader(v8::Handle<v8::Object> map);
	result_t setHeader(const char* name, Variant value);
	result_t removeHeader(const char* name);

	result_t clear();

public:
	result_t sendTo(Stream_base* stm, std::string& strCommand,
			exlib::AsyncEvent* ac);
	result_t readFrom(BufferedStream_base* stm, exlib::AsyncEvent* ac);

public:
	void addHeader(const char* name, int szName, const char* value,
			int szValue);
	result_t addHeader(std::string& strLine);
	size_t size();
	size_t getData(char* buf, size_t sz);

public:
	bool m_bResponse;
	std::string m_protocol;
	bool m_keepAlive;
	std::string m_origin;
	std::string m_encoding;
	obj_ptr<HttpCollection> m_headers;
	obj_ptr<SeekableStream_base> m_body;
};

} /* namespace fibjs */
#endif /* HTTPMESSAGE_H_ */
