/*
 * HttpRequest.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "HttpRequest.h"
#include "parse.h"
#include "HttpUploadCollection.h"

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

result_t HttpRequest::get_value(std::string& retVal)
{
	retVal = m_address;
	return 0;
}

result_t HttpRequest::set_value(const char* newVal)
{
	m_address = newVal;
	return 0;
}

result_t HttpRequest::clear()
{
	m_message.clear();
	m_method.clear();
	m_address.clear();
	m_response->clear();

	m_cookie.Release();
	m_query.Release();
	m_form.Release();

	return 0;
}

result_t HttpRequest::sendTo(obj_ptr<Stream_base>& stm, exlib::AsyncEvent* ac)
{
	return 0;
}

result_t HttpRequest::asyncSendTo(obj_ptr<Stream_base>& stm)
{
	acb_sendTo(stm);
	return 0;
}

result_t HttpRequest::onsendto(v8::Handle<v8::Function> func)
{
	return on("sendto", func);
}

result_t HttpRequest::readFrom(obj_ptr<BufferedStream_base>& stm,
		exlib::AsyncEvent* ac)
{
	class asyncReadFrom: public asyncState
	{
	public:
		asyncReadFrom(HttpRequest* pThis, BufferedStream_base* stm,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm), m_contentLength(0)
		{
			set(begin);
		}

		static int begin(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;

			pThis->set(command);

			pThis->m_pThis->clear();
			return pThis->m_stm->readLine(pThis->m_strLine, pThis);
		}

		static int command(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;
			result_t hr;

			pThis->set(header);

			int p1, p2;
			_parser p(pThis->m_strLine);

			p1 = p.pos;
			p.skipWord();
			p2 = p.pos;
			if (p1 == p2)
				return CALL_E_INVALID_DATA;

			pThis->m_pThis->m_method.assign(p.string + p1, p2 - p1);

			p.skipSpace();

			p1 = p.pos;
			p.skipWord('?');
			p2 = p.pos;
			if (p1 == p2)
				return CALL_E_INVALID_DATA;

			pThis->m_pThis->m_address.assign(p.string + p1, p2 - p1);

			if (p.get() == '?')
			{
				p.skip();

				p1 = p.pos;
				p.skipWord();
				p2 = p.pos;
				pThis->m_pThis->m_strquery.assign(p.string + p1, p2 - p1);
			}

			p.skipSpace();

			if (p.end())
				return CALL_E_INVALID_DATA;

			hr = pThis->m_pThis->set_protocol(p.string + p.pos);
			if (hr < 0)
				return hr;

			return pThis->m_stm->readLine(pThis->m_strLine, pThis);
		}

		static int header(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;

			if (pThis->m_strLine.length() > 0)
			{
				int p2;
				_parser p(pThis->m_strLine);

				p.skipWord(':');
				p2 = p.pos;
				if (0 == p2 || !p.want(':'))
					return CALL_E_INVALID_DATA;

				p.skipSpace();

				if (p2 == 14 && !qstricmp(p.string, "content-length", 14))
					pThis->m_contentLength = atoi(p.string + p.pos);

				pThis->m_pThis->m_message.addHeader(p.string, p2,
						p.string + p.pos, p.sz - p.pos);

				return pThis->m_stm->readLine(pThis->m_strLine, pThis);
			}

			std::string strProtocol;

			pThis->m_pThis->get_protocol(strProtocol);
			pThis->m_pThis->m_response->set_protocol(strProtocol.c_str());

			bool bKeepAlive;

			pThis->m_pThis->get_keepAlive(bKeepAlive);
			pThis->m_pThis->m_response->set_keepAlive(bKeepAlive);

			if (pThis->m_contentLength == 0)
				return pThis->done(0);

			pThis->set(body);

			pThis->m_pThis->get_body(pThis->m_body);

			pThis->m_stmBody = pThis->m_body;
			return pThis->m_stm->copyTo(pThis->m_stmBody,
					pThis->m_contentLength, pThis->m_copySize, pThis);
		}

		static int body(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;

			if (pThis->m_contentLength != pThis->m_copySize)
				return CALL_E_INVALID_DATA;

			pThis->m_body->rewind();

			return pThis->done(0);
		}

	public:
		HttpRequest* m_pThis;
		BufferedStream_base* m_stm;
		obj_ptr<SeekableStream_base> m_body;
		obj_ptr<Stream_base> m_stmBody;
		std::string m_strLine;
		int64_t m_contentLength;
		int64_t m_copySize;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncReadFrom(this, stm, ac))->post(0);
}

result_t HttpRequest::asyncReadFrom(obj_ptr<BufferedStream_base>& stm)
{
	acb_readFrom(stm);
	return 0;
}

result_t HttpRequest::onreadfrom(v8::Handle<v8::Function> func)
{
	return on("readfrom", func);
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

result_t HttpRequest::get_response(obj_ptr<HttpResponse_base>& retVal)
{
	retVal = m_response;
	return 0;
}

void HttpRequest::parse(std::string& str, obj_ptr<HttpCollection_base>& col,
		char split)
{
	obj_ptr<HttpCollection> c = new HttpCollection();

	const char* pstr = str.c_str();
	int nSize = (int) str.length();
	const char* pstrTemp;
	std::string strKey, strValue;

	while (nSize)
	{
		while (nSize && *pstr == ' ')
		{
			pstr++;
			nSize--;
		}

		pstrTemp = pstr;
		while (nSize && *pstr != '=' && *pstr != split)
		{
			pstr++;
			nSize--;
		}

		if (pstr > pstrTemp)
			decodeURI(pstrTemp, (int) (pstr - pstrTemp), strKey);
		else
			strKey.clear();

		if (nSize && *pstr == '=')
		{
			nSize--;
			pstr++;
		}

		pstrTemp = pstr;
		while (nSize && *pstr != split)
		{
			pstr++;
			nSize--;
		}

		if (!strKey.empty())
			if (pstr > pstrTemp)
				decodeURI(pstrTemp, (int) (pstr - pstrTemp), strValue);
			else
				strValue.clear();

		if (nSize)
		{
			nSize--;
			pstr++;
		}

		if (!strKey.empty())
			c->add(strKey, strValue);
	}

	col = c;
}

result_t HttpRequest::get_cookie(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_cookie == NULL)
	{
		std::string strCookie;

		header("cookie", strCookie);
		parse(strCookie, m_cookie, ';');
	}

	retVal = m_cookie;
	return 0;
}

result_t HttpRequest::get_form(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_form == NULL)
	{
		int64_t len = 0;

		get_contentLength(len);
		if (len == 0)
			m_form = new HttpCollection();
		else
		{
			std::string strType;
			bool bUpload = false;

			get_contentType(strType);

			if (!qstricmp(strType.c_str(), "multipart/form-data;", 20))
				bUpload = true;
			else if (qstricmp(strType.c_str(),
					"application/x-www-form-urlencoded"))
				return CALL_E_INVALID_DATA;

			obj_ptr<Buffer_base> buf;
			obj_ptr<SeekableStream_base> _body;

			get_body(_body);
			_body->rewind();
			result_t hr = _body->read((int32_t) len, buf, NULL);
			if (hr < 0)
				return hr;

			std::string strForm;
			buf->toString(strForm);

			if (bUpload)
			{
				obj_ptr<HttpUploadCollection> col = new HttpUploadCollection();
				col->parse(strForm, strType.c_str());
				m_form = col;
			}
			else
				parse(strForm, m_form, '&');
		}
	}

	retVal = m_form;

	return 0;
}

result_t HttpRequest::get_query(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_query == NULL)
		parse(m_strquery, m_query, '&');

	retVal = m_query;
	return 0;
}

} /* namespace fibjs */
