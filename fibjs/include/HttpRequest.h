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

#define HEXDATA(ch)		((ch) >= '0' && (ch) <= '9' ? (ch) - '0' : \
						(ch) >= 'a' && (ch) <= 'f' ? (ch) - 'a' + 10 : \
						(ch) >= 'A' && (ch) <= 'F' ? (ch) - 'A' + 10 : 0)

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
	virtual result_t clear();
	virtual result_t sendTo(obj_ptr<Stream_base>& stm, exlib::AsyncEvent* ac);
	virtual result_t asyncSendTo(obj_ptr<Stream_base>& stm);
	virtual result_t onsendto(v8::Handle<v8::Function> func);
	virtual result_t readFrom(obj_ptr<BufferedStream_base>& stm,
			exlib::AsyncEvent* ac);
	virtual result_t asyncReadFrom(obj_ptr<BufferedStream_base>& stm);
	virtual result_t onreadfrom(v8::Handle<v8::Function> func);

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

public:
	// HttpRequest_base
	virtual result_t get_method(std::string& retVal);
	virtual result_t set_method(const char* newVal);
	virtual result_t get_address(std::string& retVal);
	virtual result_t set_address(const char* newVal);
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

	inline void decodeURI(const char* url, int sz, std::string& retVal)
	{
		if (sz == 0)
			return;

		int len, l;
		const char* src;
		unsigned char ch;
		char* bstr;
		std::string str;

		for (len = 0, src = url, l = sz; l > 0; src++, len++, l--)
		{
			ch = (unsigned char) *src;
			if (ch == '%' && qishex(src[1]) && qishex(src[2]))
			{
				src += 2;
				l -= 2;
			}
		}

		str.resize(len);
		bstr = &str[0];

		for (len = 0, src = url, l = sz; l > 0; src++, len++, l--)
		{
			ch = (unsigned char) *src;

			if (ch == '%' && qishex(src[1]) && qishex(src[2]))
			{
				*bstr++ = (HEXDATA(src[1]) << 4) + HEXDATA(src[2]);
				src += 2;
				l -= 2;
			}
			else
				*bstr++ = ch;
		}

		retVal = str;
	}

private:
	HttpMessage m_message;
	std::string m_method;
	std::string m_address;
	std::string m_strquery;
	obj_ptr<HttpResponse_base> m_response;
	obj_ptr<HttpCollection_base> m_cookie;
	obj_ptr<HttpCollection_base> m_query;
	obj_ptr<HttpCollection_base> m_form;
};

} /* namespace fibjs */
#endif /* HTTPREQUEST_H_ */
