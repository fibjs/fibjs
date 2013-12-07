/*
 * HttpRequest.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "HttpRequest.h"
#include "parse.h"
#include "HttpUploadCollection.h"
#include "Url.h"

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

result_t HttpRequest::set_body(SeekableStream_base* newVal)
{
	return m_message.set_body(newVal);
}

result_t HttpRequest::get_length(int64_t& retVal)
{
	return m_message.get_length(retVal);
}

result_t HttpRequest::get_keepAlive(bool& retVal)
{
	return m_message.get_keepAlive(retVal);
}

result_t HttpRequest::set_keepAlive(bool newVal)
{
	return m_message.set_keepAlive(newVal);
}

result_t HttpRequest::get_maxHeadersCount(int32_t& retVal)
{
	return m_message.get_maxHeadersCount(retVal);
}

result_t HttpRequest::set_maxHeadersCount(int32_t newVal)
{
	return m_message.set_maxHeadersCount(newVal);
}

result_t HttpRequest::get_maxUploadSize(int32_t& retVal)
{
	return m_message.get_maxUploadSize(retVal);
}

result_t HttpRequest::set_maxUploadSize(int32_t newVal)
{
	return m_message.get_maxUploadSize(newVal);
}

result_t HttpRequest::hasHeader(const char* name, bool& retVal)
{
	return m_message.hasHeader(name, retVal);
}

result_t HttpRequest::firstHeader(const char* name, Variant& retVal)
{
	return m_message.firstHeader(name, retVal);
}

result_t HttpRequest::allHeader(const char* name, v8::Handle<v8::Array>& retVal)
{
	return m_message.allHeader(name, retVal);
}

result_t HttpRequest::addHeader(v8::Handle<v8::Object> map)
{
	return m_message.addHeader(map);
}

result_t HttpRequest::addHeader(const char* name, Variant value)
{
	return m_message.addHeader(name, value);
}

result_t HttpRequest::setHeader(v8::Handle<v8::Object> map)
{
	return m_message.setHeader(map);
}

result_t HttpRequest::setHeader(const char* name, Variant value)
{
	return m_message.setHeader(name, value);
}

result_t HttpRequest::removeHeader(const char* name)
{
	return m_message.removeHeader(name);
}

result_t HttpRequest::get_value(std::string& retVal)
{
	return m_message.get_value(retVal);
}

result_t HttpRequest::set_value(const char* newVal)
{
	return m_message.set_value(newVal);
}

result_t HttpRequest::get_params(obj_ptr<List_base>& retVal)
{
	return m_message.get_params(retVal);
}

result_t HttpRequest::set_params(List_base* newVal)
{
	return m_message.set_params(newVal);
}

result_t HttpRequest::get_result(Variant& retVal)
{
	return m_message.get_result(retVal);
}

result_t HttpRequest::set_result(Variant newVal)
{
	return m_message.set_result(newVal);
}

result_t HttpRequest::clear()
{
	m_message.clear();
	m_method.assign("GET", 3);
	m_address.assign("/", 1);
	m_queryString.clear();

	m_response->clear();

	m_cookie.Release();
	m_query.Release();
	m_form.Release();

	return 0;
}

result_t HttpRequest::sendTo(Stream_base* stm, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	std::string strCommand = m_method;
	std::string strProtocol;

	strCommand += ' ';
	strCommand.append(m_address);
	if (!m_queryString.empty())
	{
		strCommand += '?';
		strCommand.append(m_queryString);
	}

	get_protocol(strProtocol);
	strCommand += ' ';
	strCommand.append(strProtocol);

	return m_message.sendTo(stm, strCommand, ac);
}

result_t HttpRequest::readFrom(BufferedStream_base* stm, exlib::AsyncEvent* ac)
{
	class asyncReadFrom: public asyncState
	{
	public:
		asyncReadFrom(HttpRequest* pThis, BufferedStream_base* stm,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm)
		{
			m_pThis->clear();
			set(begin);
		}

		static int begin(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;

			pThis->set(command);
			return pThis->m_stm->readLine(HTTP_MAX_LINE, pThis->m_strLine,
					pThis);
		}

		static int command(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;
			_parser p(pThis->m_strLine);
			result_t hr;

			if (!p.getWord(pThis->m_pThis->m_method))
				return Runtime::setError("bad method.");

			p.skipSpace();

			if (!p.getWord(pThis->m_pThis->m_address, '?'))
				return Runtime::setError("bad address.");

			pThis->m_pThis->m_message.set_value(pThis->m_pThis->m_address);

			if (p.want('?'))
				p.getWord(pThis->m_pThis->m_queryString);

			p.skipSpace();

			if (p.end())
				return Runtime::setError("bad protocol.");

			hr = pThis->m_pThis->set_protocol(p.now());
			if (hr < 0)
				return hr;

			pThis->done(0);
			return pThis->m_pThis->m_message.readFrom(pThis->m_stm, pThis);
		}

	public:
		obj_ptr<HttpRequest> m_pThis;
		obj_ptr<BufferedStream_base> m_stm;
		std::string m_strLine;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncReadFrom(this, stm, ac))->post(0);
}

result_t HttpRequest::get_method(std::string& retVal)
{
	retVal = m_method;
	return 0;
}

result_t HttpRequest::get_stream(obj_ptr<Stream_base>& retVal)
{
	return m_message.get_stream(retVal);
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

result_t HttpRequest::get_queryString(std::string& retVal)
{
	retVal = m_queryString;
	return 0;
}

result_t HttpRequest::set_queryString(const char* newVal)
{
	m_queryString = newVal;
	return 0;
}

result_t HttpRequest::get_response(obj_ptr<HttpResponse_base>& retVal)
{
	retVal = m_response;
	return 0;
}

void HttpRequest::parse(std::string& str, char split,
		obj_ptr<HttpCollection_base>& retVal)
{
	obj_ptr < HttpCollection > c = new HttpCollection();

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
			Url::decodeURI(pstrTemp, (int) (pstr - pstrTemp), strKey);
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
		{
			if (pstr > pstrTemp)
				Url::decodeURI(pstrTemp, (int) (pstr - pstrTemp), strValue);
			else
				strValue.clear();
		}

		if (nSize)
		{
			nSize--;
			pstr++;
		}

		if (!strKey.empty())
			c->add(strKey, strValue);
	}

	retVal = c;
}

result_t HttpRequest::get_cookie(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_cookie == NULL)
	{
		std::string strCookie;

		header("cookie", strCookie);
		parse(strCookie, ';', m_cookie);
	}

	retVal = m_cookie;
	return 0;
}

result_t HttpRequest::get_form(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_form == NULL)
	{
		int64_t len = 0;

		get_length(len);
		if (len == 0)
			m_form = new HttpCollection();
		else
		{
			std::string strType;
			bool bUpload = false;
			Variant v;

			if (firstHeader("Content-Type", v) == CALL_RETURN_NULL)
				return Runtime::setError("unknown form format.");

			strType = v.string();

			if (!qstricmp(strType.c_str(), "multipart/form-data;", 20))
				bUpload = true;
			else if (qstricmp(strType.c_str(),
					"application/x-www-form-urlencoded", 33))
				return Runtime::setError("unknown form format.");

			obj_ptr < Buffer_base > buf;
			obj_ptr < SeekableStream_base > _body;

			get_body (_body);
			_body->rewind();
			result_t hr = _body->read((int32_t) len, buf, NULL);
			if (hr < 0)
				return hr;

			std::string strForm;
			buf->toString(strForm);

			if (bUpload)
			{
				obj_ptr < HttpUploadCollection > col =
						new HttpUploadCollection();
				col->parse(strForm, strType.c_str());
				m_form = col;
			}
			else
				parse(strForm, '&', m_form);
		}
	}

	retVal = m_form;

	return 0;
}

result_t HttpRequest::get_query(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_query == NULL)
		parse(m_queryString, '&', m_query);

	retVal = m_query;
	return 0;
}

} /* namespace fibjs */
