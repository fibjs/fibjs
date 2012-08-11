/*
 * HttpMessage.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "HttpMessage.h"

namespace fibjs
{

result_t HttpMessage::get_protocol(std::string& retVal)
{
	retVal = m_protocol;
	return 0;
}

result_t HttpMessage::set_protocol(const char* newVal)
{
	if (qstrcmp(newVal, "HTTP/", 5) || !qisnumber(newVal[5]) || newVal[6] != '.'
			|| !qisnumber(newVal[7]) || newVal[8])
		return CALL_E_INVALIDARG;

	m_protocol = newVal;
	return 0;
}

result_t HttpMessage::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
	retVal = m_headers;
	return 0;
}

result_t HttpMessage::get_body(obj_ptr<SeekableStream_base>& retVal)
{
	if (m_body == NULL)
		m_body = new MemoryStream();

	retVal = m_body;

	return 0;
}

result_t HttpMessage::set_body(obj_ptr<SeekableStream_base>& newVal)
{
	if (m_body != NULL)
		return CALL_E_INVALID_CALL;

	m_body = newVal;

	return 0;
}

result_t HttpMessage::get_contentType(std::string& retVal)
{
	retVal = m_contentType;
	return 0;
}

result_t HttpMessage::set_contentType(const char* newVal)
{
	m_contentType = newVal;
	return 0;
}

result_t HttpMessage::get_contentLength(int64_t& retVal)
{
	if (m_body == NULL)
	{
		retVal = 0;
		return 0;
	}
	return m_body->size(retVal);
}

result_t HttpMessage::get_keepAlive(bool& retVal)
{
	retVal = m_keepAlive;
	return 0;
}

result_t HttpMessage::set_keepAlive(bool newVal)
{
	m_keepAlive = newVal;
	return 0;
}

result_t HttpMessage::clear()
{
	m_protocol.clear();
	m_contentType.clear();
	m_keepAlive = false;

	m_headers->clear();

	m_body.Release();

	return 0;
}

} /* namespace fibjs */
