/*
 * HttpRequest.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "ifs/HttpRequest.h"
#include "HttpMessage.h"
#include "HttpResponse.h"
#include "HttpCollection.h"

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

namespace fibjs
{

class HttpRequest: public HttpRequest_base
{
public:
	HttpRequest()
	{
		m_response = new HttpResponse();
		clear();
		extMemory(4096);
	}

EVENT_SUPPORT()
	;

public:
	// Message_base
	virtual result_t get_value(std::string& retVal);
	virtual result_t set_value(const char* newVal);
	virtual result_t get_params(obj_ptr<List_base>& retVal);
	virtual result_t set_params(List_base* newVal);
	virtual result_t get_result(Variant& retVal);
	virtual result_t set_result(Variant newVal);
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
	// HttpRequest_base
	virtual result_t get_method(std::string& retVal);
	virtual result_t set_method(const char* newVal);
	virtual result_t get_address(std::string& retVal);
	virtual result_t set_address(const char* newVal);
	virtual result_t get_queryString(std::string& retVal);
	virtual result_t set_queryString(const char* newVal);
	virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal);
	virtual result_t get_cookie(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_form(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_query(obj_ptr<HttpCollection_base>& retVal);

public:
	void header(const char* name, std::string& retVal)
	{
		Variant varCookie;
		obj_ptr<HttpCollection_base> hdrs;

		m_message.get_headers(hdrs);
		hdrs->first("cookie", varCookie);
		retVal = varCookie.string();
	}

	void parse(std::string& str, char split,
			obj_ptr<HttpCollection_base>& retVal);

private:
	HttpMessage m_message;
	std::string m_method;
	std::string m_address;
	std::string m_queryString;
	obj_ptr<HttpResponse_base> m_response;
	obj_ptr<HttpCollection_base> m_cookie;
	obj_ptr<HttpCollection_base> m_query;
	obj_ptr<HttpCollection_base> m_form;
};

} /* namespace fibjs */
#endif /* HTTPREQUEST_H_ */
