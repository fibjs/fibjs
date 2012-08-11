/*
 * HttpRequest.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "HttpRequest.h"
#include "parse.h"

namespace fibjs
{

result_t HttpRequest_base::_new(obj_ptr<HttpRequest_base>& retVal)
{
	retVal = new HttpRequest();
	return 0;
}

result_t HttpRequest::get_protocol(std::string& retVal)
{
	return m_message.get_protocol(retVal);
}

result_t HttpRequest::set_protocol(const char* newVal)
{
	return m_message.set_protocol(newVal);
}

result_t HttpRequest::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
	return m_message.get_headers(retVal);
}

result_t HttpRequest::get_body(obj_ptr<SeekableStream_base>& retVal)
{
	return m_message.get_body(retVal);
}

result_t HttpRequest::set_body(obj_ptr<SeekableStream_base>& newVal)
{
	return m_message.set_body(newVal);
}

result_t HttpRequest::get_contentType(std::string& retVal)
{
	return m_message.get_contentType(retVal);
}

result_t HttpRequest::set_contentType(const char* newVal)
{
	return m_message.set_contentType(newVal);
}

result_t HttpRequest::get_contentLength(int64_t& retVal)
{
	return m_message.get_contentLength(retVal);
}

result_t HttpRequest::get_keepAlive(bool& retVal)
{
	return m_message.get_keepAlive(retVal);
}

result_t HttpRequest::set_keepAlive(bool newVal)
{
	return m_message.set_keepAlive(newVal);
}

result_t HttpRequest::clear()
{
	m_message.clear();
	m_method.clear();
	m_address.clear();

	return 0;
}

result_t HttpRequest::send(obj_ptr<Stream_base>& stm, exlib::AsyncEvent* ac)
{
	return 0;
}

result_t HttpRequest::asyncSend(obj_ptr<Stream_base>& stm)
{
	acb_send(stm);
	return 0;
}

result_t HttpRequest::onsend(v8::Handle<v8::Function> func)
{
	return on("send", func);
}

result_t HttpRequest::read(obj_ptr<BufferedStream_base>& stm,
		exlib::AsyncEvent* ac)
{
	class asyncRead: public asyncEvent
	{
	public:
		asyncRead(HttpRequest* pThis, BufferedStream_base* stm,
				exlib::AsyncEvent* ac) :
				m_bAsync(false), m_pThis(pThis), m_stm(stm), m_ac(ac), m_state(
						0), m_contentLength(0)
		{
		}

		virtual int post(int v)
		{
			result_t hr = v;

			while (hr != CALL_E_PENDDING)
			{
				if (hr < 0)
				{
					if (m_bAsync)
						m_ac->post(hr);
					delete this;
					return hr;
				}

				switch (m_state)
				{
				case 0:
					// begin to read....
					m_state = 1;

					m_pThis->clear();
					hr = m_stm->readLine(m_strLine, this);
					break;
				case 1:
					// request command.
					m_state = 2;

					{
						int p1, p2;
						char ch;
						_parser p(m_strLine);

						p1 = p.pos;
						while (!p.end() && (ch = p.get()) && ch != ' ')
							p.skip();
						p2 = p.pos;
						if (p1 == p2)
						{
							hr = CALL_E_INVALID_DATA;
							break;
						}

						m_pThis->m_method.assign(p.string + p1, p2 - p1);

						p.skipSpace();

						p1 = p.pos;
						while (!p.end() && (ch = p.get()) && ch != ' '
								&& ch != '?')
							p.skip();
						p2 = p.pos;
						if (p1 == p2)
						{
							hr = CALL_E_INVALID_DATA;
							break;
						}

						m_pThis->m_address.assign(p.string + p1, p2 - p1);

						if (ch == '?')
						{
							p.skip();

							p1 = p.pos;
							while (!p.end() && (ch = p.get()) && ch != ' ')
								p.skip();
							p2 = p.pos;
							m_pThis->m_strquery.assign(p.string + p1, p2 - p1);
						}

						p.skipSpace();

						if (p.end())
						{
							hr = CALL_E_INVALID_DATA;
							break;
						}

						hr = m_pThis->set_protocol(p.string + p.pos);
						if (hr < 0)
							break;
					}

					hr = m_stm->readLine(m_strLine, this);
					break;
				case 2:
					// request header....
					if (m_strLine.length() > 0)
					{
						int p2;
						char ch;
						_parser p(m_strLine);

						while (!p.end() && (ch = p.get()) && ch != ' '
								&& ch != ':')
							p.skip();
						p2 = p.pos;
						if (0 == p2 || !p.want(':'))
						{
							hr = CALL_E_INVALID_DATA;
							break;
						}
						p.skipSpace();

						if (p2 == 14
								&& !qstricmp(p.string, "content-length", 14))
							m_contentLength = atoi(p.string + p.pos);

						m_pThis->m_message.addHeader(p.string, p2,
								p.string + p.pos, p.sz - p.pos);

						hr = m_stm->readLine(m_strLine, this);
					}
					else
						m_state = 3;

					break;
				case 3:
					// read body....
					if (m_contentLength > 0)
					{
						m_state = 4;

						hr = m_pThis->get_body(m_body);
						if (hr < 0)
							break;

						obj_ptr<Stream_base> body(m_body);
						hr = m_stm->copyTo(body, m_contentLength, m_copySize,
								this);
					}
					else
						m_state = 5;

					break;
				case 4:
					// read body ok....
					m_state = 5;

					if (m_contentLength != m_copySize)
					{
						hr = CALL_E_INVALID_DATA;
						break;
					}
					else
						m_body->rewind();

					break;
				case 5:
					// done....
					if (m_bAsync)
						m_ac->post(0);
					delete this;
					return 0;
				}
			}

			m_bAsync = true;
			return hr;
		}

	public:
		bool m_bAsync;
		HttpRequest* m_pThis;
		BufferedStream_base* m_stm;
		exlib::AsyncEvent* m_ac;
		obj_ptr<SeekableStream_base> m_body;
		std::string m_strLine;
		int m_state;
		int64_t m_contentLength;
		int64_t m_copySize;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncRead(this, stm, ac))->post(0);
}

result_t HttpRequest::asyncRead(obj_ptr<BufferedStream_base>& stm)
{
	acb_read(stm);
	return 0;
}

result_t HttpRequest::onread(v8::Handle<v8::Function> func)
{
	return on("read", func);
}

result_t HttpRequest::get_method(std::string& retVal)
{
	retVal = m_method;
	return 0;
}

result_t HttpRequest::set_method(const char* newVal)
{
	m_method = newVal;
	return 0;
}

result_t HttpRequest::get_address(std::string& retVal)
{
	retVal = m_address;
	return 0;
}

result_t HttpRequest::set_address(const char* newVal)
{
	m_address = newVal;
	return 0;
}

result_t HttpRequest::get_cookies(obj_ptr<HttpCollection_base>& retVal)
{
	return 0;
}

result_t HttpRequest::get_form(obj_ptr<HttpCollection_base>& retVal)
{
	return 0;
}

result_t HttpRequest::get_query(obj_ptr<HttpCollection_base>& retVal)
{
	return 0;
}

} /* namespace fibjs */
