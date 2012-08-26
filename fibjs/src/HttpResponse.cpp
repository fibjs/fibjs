/*
 * HttpResponse.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: lion
 */

#include "HttpResponse.h"
#include "Buffer.h"

namespace fibjs
{

result_t HttpResponse_base::_new(obj_ptr<HttpResponse_base>& retVal)
{
	retVal = new HttpResponse();
	return 0;
}

result_t HttpResponse::get_protocol(std::string& retVal)
{
	return m_message.get_protocol(retVal);
}

result_t HttpResponse::set_protocol(const char* newVal)
{
	return m_message.set_protocol(newVal);
}

result_t HttpResponse::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
	return m_message.get_headers(retVal);
}

result_t HttpResponse::get_body(obj_ptr<SeekableStream_base>& retVal)
{
	return m_message.get_body(retVal);
}

result_t HttpResponse::set_body(obj_ptr<SeekableStream_base>& newVal)
{
	return m_message.set_body(newVal);
}

result_t HttpResponse::get_contentType(std::string& retVal)
{
	return m_message.get_contentType(retVal);
}

result_t HttpResponse::set_contentType(const char* newVal)
{
	return m_message.set_contentType(newVal);
}

result_t HttpResponse::get_contentLength(int64_t& retVal)
{
	return m_message.get_contentLength(retVal);
}

result_t HttpResponse::get_keepAlive(bool& retVal)
{
	return m_message.get_keepAlive(retVal);
}

result_t HttpResponse::set_keepAlive(bool newVal)
{
	return m_message.set_keepAlive(newVal);
}

result_t HttpResponse::get_value(std::string& retVal)
{
	return 0;
}

result_t HttpResponse::set_value(const char* newVal)
{
	return 0;
}

result_t HttpResponse::clear()
{
	m_message.clear();
	m_status = 200;

	return 0;
}

static const char * const status_lines[] =
{
#define LEVEL_100  0
		"100 Continue", "101 Switching Protocols", "102 Processing",
#define LEVEL_200  3
		"200 OK", "201 Created", "202 Accepted",
		"203 Non-Authoritative Information", "204 No Content",
		"205 Reset Content", "206 Partial Content", "207 Multi-Status",
#define LEVEL_300 11
		"300 Multiple Choices", "301 Moved Permanently", "302 Object moved",
		"303 See Other", "304 Not Modified", "305 Use Proxy", "306 unused",
		"307 Temporary Redirect",
#define LEVEL_400 19
		"400 Bad Request", "401 Authorization Required", "402 Payment Required",
		"403 Forbidden", "404 File Not Found", "405 Method Not Allowed",
		"406 Not Acceptable", "407 Proxy Authentication Required",
		"408 Request Time-out", "409 Conflict", "410 Gone",
		"411 Length Required", "412 Precondition Failed",
		"413 Request Entity Too Large", "414 Request-URI Too Large",
		"415 Unsupported Media Type", "416 Requested Range Not Satisfiable",
		"417 Expectation Failed", "418 Host Not Found", "419 unused",
		"420 unused", "421 unused", "422 Unprocessable Entity", "423 Locked",
		"424 Failed Dependency",
#define LEVEL_500 44
		"500 Internal Server Error", "501 Method Not Implemented",
		"502 Bad Gateway", "503 Service Temporarily Unavailable",
		"504 Gateway Time-out", "505 HTTP Version Not Supported",
		"506 Variant Also Negotiates", "507 Insufficient Storage", "508 unused",
		"509 unused", "510 Not Extended"
#define RESPONSE_CODES 55
		};
static int shortcut[6] =
{ LEVEL_100, LEVEL_200, LEVEL_300, LEVEL_400, LEVEL_500, RESPONSE_CODES };
static unsigned char status_lines_size[RESPONSE_CODES];

static class _init_status_line
{
public:
	_init_status_line()
	{
		int i;

		for (i = 0; i < RESPONSE_CODES; i++)
			status_lines_size[i] = (unsigned char) qstrlen(status_lines[i]);
	}
} s_init_status_line;

#define TINY_SIZE	32768

result_t HttpResponse::sendTo(obj_ptr<Stream_base>& stm, exlib::AsyncEvent* ac)
{
	class asyncSendTo: public asyncState
	{
	public:
		asyncSendTo(HttpResponse* pThis, Stream_base* stm, exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm)
		{
			m_contentLength = 0;
			m_pThis->m_message.get_contentLength(m_contentLength);

			if (m_contentLength > 0 && m_contentLength < TINY_SIZE)
				set(tinybody);
			else
				set(header);
		}

		static int tinybody(asyncState* pState, int n)
		{
			asyncSendTo* pThis = (asyncSendTo*) pState;
			obj_ptr<SeekableStream_base> _body;

			result_t hr = pThis->m_pThis->get_body(_body);
			if (hr < 0)
				return hr;

			pThis->set(header);

			_body->rewind();
			return _body->read((int32_t) pThis->m_contentLength,
					pThis->m_buffer, pThis);
		}

		static int header(asyncState* pState, int n)
		{
			asyncSendTo* pThis = (asyncSendTo*) pState;
			int pos = shortcut[pThis->m_pThis->m_status / 100 - 1]
					+ pThis->m_pThis->m_status % 100;
			size_t sz = status_lines_size[pos] + 11;
			size_t sz1;
			std::string m_strBuf;
			char *pBuf;

			if (pThis->m_buffer != NULL)
			{
				pThis->m_buffer->toString(pThis->m_body);
				pThis->m_buffer.Release();

				if (pThis->m_contentLength != (int) pThis->m_body.length())
					return CALL_E_INVALID_DATA;
			}

			sz1 = pThis->m_pThis->m_message.size();
			pThis->m_pThis->get_protocol(m_strBuf);
			m_strBuf.resize(sz + sz1 + pThis->m_body.length());

			pBuf = &m_strBuf[8];
			*pBuf++ = ' ';
			memcpy(pBuf, status_lines[pos], status_lines_size[pos]);
			pBuf += status_lines_size[pos];
			*pBuf++ = '\r';
			*pBuf++ = '\n';

			pBuf += pThis->m_pThis->m_message.getData(pBuf, sz1);

			if (pThis->m_body.length() > 0)
				memcpy(pBuf, pThis->m_body.c_str(), pThis->m_body.length());

			pThis->m_buffer = new Buffer(m_strBuf);

			if (pThis->m_contentLength == 0 || pThis->m_body.length() > 0)
				pThis->done();
			else
				pThis->set(body);

			return pThis->m_stm->write(pThis->m_buffer, pThis);
		}

		static int body(asyncState* pState, int n)
		{
			asyncSendTo* pThis = (asyncSendTo*) pState;

			if (pThis->m_contentLength == 0)
				return pThis->done();

			obj_ptr<SeekableStream_base> _body;

			result_t hr = pThis->m_pThis->get_body(_body);
			if (hr < 0)
				return hr;

			_body->rewind();

			pThis->set(body_ok);
			return _body->copyTo(pThis->m_stm, pThis->m_contentLength,
					pThis->m_copySize, pThis);
		}

		static int body_ok(asyncState* pState, int n)
		{
			asyncSendTo* pThis = (asyncSendTo*) pState;

			if (pThis->m_contentLength != pThis->m_copySize)
				return CALL_E_INVALID_DATA;

			return pThis->done();
		}

	public:
		HttpResponse* m_pThis;
		obj_ptr<Stream_base> m_stm;
		obj_ptr<Buffer_base> m_buffer;
		int64_t m_contentLength;
		int64_t m_copySize;
		std::string m_body;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncSendTo(this, stm, ac))->post(0);
}

result_t HttpResponse::asyncSendTo(obj_ptr<Stream_base>& stm)
{
	acb_sendTo(stm);
	return 0;
}

result_t HttpResponse::onsendto(v8::Handle<v8::Function> func)
{
	return on("sendto", func);
}

result_t HttpResponse::readFrom(obj_ptr<BufferedStream_base>& stm,
		exlib::AsyncEvent* ac)
{
	return 0;
}

result_t HttpResponse::asyncReadFrom(obj_ptr<BufferedStream_base>& stm)
{
	acb_readFrom(stm);
	return 0;
}

result_t HttpResponse::onreadfrom(v8::Handle<v8::Function> func)
{
	return on("readfrom", func);
}

result_t HttpResponse::get_status(int32_t& retVal)
{
	retVal = m_status;
	return 0;
}

result_t HttpResponse::set_status(int32_t newVal)
{
	if (newVal < 100 || newVal >= 600)
		newVal = 500;
	else
	{
		int n = newVal / 100;
		if (shortcut[n - 1] + newVal % 100 >= shortcut[n])
			newVal = 500;
	}

	m_status = newVal;
	return 0;
}

} /* namespace fibjs */
