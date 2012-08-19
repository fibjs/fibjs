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
	HttpResponse()
	{
		clear();
		extMemory(4096);
	}

	EVENT_SUPPORT();

public:
	// HttpMessage_base
	virtual result_t get_protocol(std::string& retVal);
	virtual result_t set_protocol(const char* newVal);
	virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal);
	virtual result_t set_body(obj_ptr<SeekableStream_base>& newVal);
	virtual result_t get_contentType(std::string& retVal);
	virtual result_t set_contentType(const char* newVal);
	virtual result_t get_contentLength(int64_t& retVal);
	virtual result_t get_keepAlive(bool& retVal);
	virtual result_t set_keepAlive(bool newVal);
	virtual result_t clear();
	virtual result_t send(obj_ptr<Stream_base>& stm, exlib::AsyncEvent* ac);
	virtual result_t asyncSend(obj_ptr<Stream_base>& stm);
	virtual result_t onsend(v8::Handle<v8::Function> func);
	virtual result_t read(obj_ptr<BufferedStream_base>& stm, exlib::AsyncEvent* ac);
	virtual result_t asyncRead(obj_ptr<BufferedStream_base>& stm);
	virtual result_t onread(v8::Handle<v8::Function> func);

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
