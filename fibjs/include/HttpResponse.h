/*
 * HttpResponse.h
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#include "ifs/HttpResponse.h"
#include "HttpMessage.h"

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

namespace fibjs
{

class HttpResponse: public HttpResponse_base
{

public:
	HttpResponse() :
			m_message(true)
	{
		clear();
		extMemory(4096);
	}

EVENT_SUPPORT()
	;

public:
	// Message_base
	virtual result_t get_value(std::string& retVal);
	virtual result_t set_value(const char* newVal);
	virtual result_t clear();
	virtual result_t sendTo(Stream_base* stm, exlib::AsyncEvent* ac);
	virtual result_t asyncSendTo(Stream_base* stm);
	virtual result_t onsendto(v8::Handle<v8::Function> func);
	virtual result_t readFrom(BufferedStream_base* stm, exlib::AsyncEvent* ac);
	virtual result_t asyncReadFrom(BufferedStream_base* stm);
	virtual result_t onreadfrom(v8::Handle<v8::Function> func);

public:
	// HttpMessage_base
	virtual result_t get_protocol(std::string& retVal);
	virtual result_t set_protocol(const char* newVal);
	virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal);
	virtual result_t set_body(SeekableStream_base* newVal);
	virtual result_t get_contentLength(int64_t& retVal);
	virtual result_t get_keepAlive(bool& retVal);
	virtual result_t set_keepAlive(bool newVal);
	virtual result_t hasHeader(const char* name, bool& retVal);
	virtual result_t firstHeader(const char* name, Variant& retVal);
	virtual result_t allHeader(const char* name, v8::Handle<v8::Array>& retVal);
	virtual result_t addHeader(v8::Handle<v8::Object> map);
	virtual result_t addHeader(const char* name, Variant value);
	virtual result_t setHeader(v8::Handle<v8::Object> map);
	virtual result_t setHeader(const char* name, Variant value);
	virtual result_t removeHeader(const char* name);

public:
	// HttpResponse_base
	virtual result_t get_status(int32_t& retVal);
	virtual result_t set_status(int32_t newVal);

public:
	HttpMessage m_message;
	int32_t m_status;
};

} /* namespace fibjs */
#endif /* HTTPRESPONSE_H_ */
