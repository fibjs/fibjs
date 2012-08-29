/*
 * HttpMessage.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: lion
 */

#include "HttpMessage.h"

namespace fibjs
{

void HttpMessage::addHeader(const char* name, int szName, const char* value,
		int szValue)
{
	if (szName == 12 && !qstricmp(name, "content-type", szName))
		m_contentType.assign(value, szValue);
	else if (szName == 10 && !qstricmp(name, "connection", szName))
		m_keepAlive = !qstricmp(value, "keep-alive", 10);
	else
		m_headers->add(name, szName, value, szValue);
}

size_t HttpMessage::size()
{
	size_t sz = 2 + m_headers->size();
	int64_t l;

	// connection 10
	sz += 10 + 4 + (m_keepAlive ? 10 : 5);

	// content-type 12
	if (m_contentType.length() > 0)
		sz += 12 + 4 + m_contentType.length();

	// content-length 14
	get_contentLength(l);
	if (l > 0)
	{
		sz += 14 + 4;
		while (l > 0)
		{
			l /= 10;
			sz++;
		}
	}
	else
		sz += 19;

	return sz;
}

inline void cp(char* buf, size_t sz, size_t& pos, const char* str, size_t szStr)
{
	buf += pos;

	pos += szStr;
	if (pos > sz)
	{
		szStr -= pos - sz;
		pos = sz;
	}

	memcpy(buf, str, szStr);
}

size_t HttpMessage::getData(char* buf, size_t sz)
{
	size_t pos = m_headers->getData(buf, sz);
	int64_t l;

	// connection 10
	cp(buf, sz, pos, "Connection: ", 12);
	if (m_keepAlive)
		cp(buf, sz, pos, "keep-alive\r\n", 12);
	else
		cp(buf, sz, pos, "close\r\n", 7);

	// content-type 12
	if (m_contentType.length() > 0)
	{
		cp(buf, sz, pos, "Content-Type: ", 14);
		cp(buf, sz, pos, m_contentType.c_str(), m_contentType.length());
		cp(buf, sz, pos, "\r\n", 2);
	}

	// content-length 14
	get_contentLength(l);
	if (l > 0)
	{
		char s[32];
		char* p;
		int n;

		cp(buf, sz, pos, "Content-Length: ", 16);
		p = s + 32;
		*--p = 0;
		*--p = '\n';
		*--p = '\r';
		n = 2;

		while (l > 0)
		{
			*--p = l % 10 + '0';
			n++;
			l /= 10;
		}

		cp(buf, sz, pos, p, n);
	}
	else
		cp(buf, sz, pos, "Content-Length: 0\r\n", 19);

	cp(buf, sz, pos, "\r\n", 2);

	return pos;
}

result_t HttpMessage::get_protocol(std::string& retVal)
{
	retVal = m_protocol;
	return 0;
}

result_t HttpMessage::set_protocol(const char* newVal)
{
	if (qstrcmp(newVal, "HTTP/", 5) || !qisdigit(newVal[5]) || newVal[6] != '.'
			|| !qisdigit(newVal[7]) || newVal[8])
		return CALL_E_INVALIDARG;

	m_keepAlive = ((newVal[5] - '0') * 10 + newVal[7] - '0') > 10;

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

result_t HttpMessage::set_body(SeekableStream_base* newVal)
{
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
	m_protocol.assign("HTTP/1.1", 8);
	m_contentType.assign("text/html", 9);
	m_keepAlive = true;

	m_headers->clear();

	m_body.Release();

	return 0;
}

} /* namespace fibjs */
