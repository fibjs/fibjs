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

			set(read);
		}

		static int read(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			bool bKeepAlive = false;

			pThis->m_rep->get_keepAlive(bKeepAlive);
			if (!bKeepAlive)
				return pThis->done(CALL_RETURN_NULL);

			pThis->set(invoke);
			return pThis->m_req->readFrom(pThis->m_stmBuffered, pThis);
		}

		static int invoke(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			std::string strProtocol;

			pThis->m_req->get_protocol(strProtocol);
			pThis->m_rep->set_protocol(strProtocol.c_str());

			bool bKeepAlive;

			pThis->m_req->get_keepAlive(bKeepAlive);
			pThis->m_rep->set_keepAlive(bKeepAlive);

			pThis->set(send);

			if (pThis->m_pThis->m_crossDomain)
			{
				std::string url;

				pThis->m_req->get_address(url);

				if (!qstrcmp(url.c_str(), "/crossdomain.xml"))
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
					std::string method;

					if (pThis->m_req->firstHeader("origin",
							origin) != CALL_RETURN_NULL)
					{
						pThis->m_rep->setHeader(
								"Access-Control-Allow-Credentials", "true");
						pThis->m_rep->setHeader("Access-Control-Allow-Origin",
								origin);

						pThis->m_req->get_method(method);

						if (!qstricmp(method.c_str(), "options"))
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
					pThis->m_rep->addHeader("Cache-Control", "private");
					pThis->m_rep->addHeader("Expires", "-1");
				}
			}

			pThis->set(read);
			return pThis->m_rep->sendTo(pThis->m_stm, pThis);
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

}

