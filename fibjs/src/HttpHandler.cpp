/*
 * HttpHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/mq.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "ifs/zlib.h"

namespace fibjs
{

static std::string s_crossdomain;

result_t HttpHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(HttpHandler* pThis, Stream_base* stm, exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm)
		{
			m_stmBuffered = new BufferedStream(stm);
			m_stmBuffered->set_EOL("\r\n");

			m_req = new HttpRequest();
			m_req->get_response(m_rep);

			m_req->set_maxHeadersCount(pThis->m_maxHeadersCount);
			m_req->set_maxUploadSize(pThis->m_maxUploadSize);

			set(read);
		}

		static int read(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			bool bKeepAlive = false;

			pThis->m_rep->get_keepAlive(bKeepAlive);
			if (!bKeepAlive)
				return pThis->done(CALL_RETURN_NULL);

			pThis->m_zip.Release();
			pThis->m_body.Release();

			pThis->set(invoke);
			return pThis->m_req->readFrom(pThis->m_stmBuffered, pThis);
		}

		static int invoke(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			std::string str;

			pThis->m_req->get_protocol(str);
			pThis->m_rep->set_protocol(str.c_str());

			bool bKeepAlive;

			pThis->m_req->get_keepAlive(bKeepAlive);
			pThis->m_rep->set_keepAlive(bKeepAlive);

			pThis->set(send);

			if (pThis->m_pThis->m_crossDomain)
			{
				pThis->m_req->get_address(str);

				if (!qstrcmp(str.c_str(), "/crossdomain.xml"))
				{
					if (s_crossdomain.empty())
						s_crossdomain.assign(
								"<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>",
								88);

					obj_ptr<MemoryStream> body = new MemoryStream();
					obj_ptr<Buffer> buf = new Buffer(s_crossdomain);

					pThis->m_rep->set_body(body);
					body->write(buf, NULL);

					pThis->m_rep->setHeader("Content-Type", "text/xml");

					return 0;
				}
				else
				{
					Variant origin;

					if (pThis->m_req->firstHeader("origin",
							origin) != CALL_RETURN_NULL)
					{
						pThis->m_rep->setHeader(
								"Access-Control-Allow-Credentials", "true");
						pThis->m_rep->setHeader("Access-Control-Allow-Origin",
								origin);

						pThis->m_req->get_method(str);

						if (!qstricmp(str.c_str(), "options"))
						{
							pThis->m_rep->setHeader(
									"Access-Control-Allow-Methods", "*");
							pThis->m_rep->setHeader(
									"Access-Control-Allow-Headers",
									"CONTENT-TYPE");
							pThis->m_rep->setHeader("Access-Control-Max-Age",
									"1728000");

							return 0;
						}
					}
				}
			}

			return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_req, pThis);
		}

		static int send(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			int32_t s;
			bool t = false;

			pThis->m_rep->get_status(s);
			if (s == 200)
			{
				pThis->m_rep->hasHeader("Last-Modified", t);
				if (!t)
				{
					pThis->m_rep->addHeader("Cache-Control", "no-cache");
					pThis->m_rep->addHeader("Expires", "-1");
				}
			}

			int64_t len;

			pThis->m_rep->get_length(len);

			if (len > 128 && len < 1024 * 1024)
			{
				Variant hdr;

				if (pThis->m_req->firstHeader("Accept-Encoding",
						hdr) != CALL_RETURN_NULL)
				{
					std::string str = hdr.string();
					int type = 0;

					if (qstristr(str.c_str(), "gzip"))
						type = 1;
					else if (qstristr(str.c_str(), "deflate"))
						type = 2;

					if (type != 0)
					{
						if (pThis->m_rep->firstHeader("Content-Type",
								hdr) != CALL_RETURN_NULL)
						{
							str = hdr.string();

							if (qstricmp(str.c_str(), "text/", 5)
									&& qstricmp(str.c_str(),
											"application/x-javascript")
									&& qstricmp(str.c_str(),
											"application/json"))
								type = 0;
						}
						else
							type = 0;
					}

					if (type != 0)
					{
						if (pThis->m_rep->firstHeader("Content-Encoding",
								hdr) != CALL_RETURN_NULL)
							type = 0;
					}

					if (type != 0)
					{
						pThis->m_rep->addHeader("Content-Encoding",
								type == 1 ? "gzip" : "deflate");

						pThis->m_rep->get_body(pThis->m_body);
						pThis->m_body->rewind();

						pThis->m_zip = new MemoryStream();

						pThis->set(zip);

						if (type == 1)
							return zlib_base::gzipTo(pThis->m_body,
									pThis->m_zip, pThis);
						else
							return zlib_base::deflateTo(pThis->m_body,
									pThis->m_zip, -1, pThis);
					}
				}
			}

			pThis->set(close);
			return pThis->m_rep->sendTo(pThis->m_stm, pThis);
		}

		static int zip(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->m_rep->set_body(pThis->m_zip);

			pThis->set(close);
			return pThis->m_rep->sendTo(pThis->m_stm, pThis);
		}

		static int close(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			if (!pThis->m_body)
				pThis->m_rep->get_body(pThis->m_body);

			pThis->set(read);
			if (!pThis->m_body)
				return 0;

			return pThis->m_body->close(pThis);
		}

		virtual int error(int v)
		{
			if (is(send))
			{
				m_rep->set_status(500);
				return 0;
			}

			if (is(invoke))
			{
				m_rep->set_keepAlive(false);
				m_rep->set_status(400);
				set(send);
				return 0;
			}

			return v;
		}

	private:
		obj_ptr<HttpHandler> m_pThis;
		obj_ptr<Stream_base> m_stm;
		obj_ptr<BufferedStream_base> m_stmBuffered;
		obj_ptr<HttpRequest_base> m_req;
		obj_ptr<HttpResponse_base> m_rep;
		obj_ptr<MemoryStream> m_zip;
		obj_ptr<SeekableStream_base> m_body;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Stream_base> stm = Stream_base::getInstance(v);

	if (stm == NULL)
		return CALL_E_BADVARTYPE;

	return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t HttpHandler::get_crossDomain(bool& retVal)
{
	retVal = m_crossDomain;
	return 0;
}

result_t HttpHandler::set_crossDomain(bool newVal)
{
	m_crossDomain = newVal;
	return 0;
}

result_t HttpHandler::get_maxHeadersCount(int32_t& retVal)
{
	retVal = m_maxHeadersCount;
	return 0;
}

result_t HttpHandler::set_maxHeadersCount(int32_t newVal)
{
	if (newVal < 0)
		return CALL_E_OUTRANGE;

	m_maxHeadersCount = newVal;
	return 0;
}

result_t HttpHandler::get_maxUploadSize(int32_t& retVal)
{
	retVal = m_maxUploadSize;
	return 0;
}

result_t HttpHandler::set_maxUploadSize(int32_t newVal)
{
	if (newVal < 0)
		return CALL_E_OUTRANGE;

	m_maxUploadSize = newVal;
	return 0;
}

}

