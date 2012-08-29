/*
 * Url.h
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#include "ifs/Url.h"

#ifndef URL_H_
#define URL_H_

namespace fibjs
{

class Url: public Url_base
{
public:
	Url()
	{
		extMemory(1024);
	}

	Url(const Url& u);

public:
	// object_base
	result_t toString(std::string& retVal);

public:
	// Url_base
	virtual result_t parse(const char* url);
	virtual result_t format(v8::Handle<v8::Object> args);
	virtual result_t resolve(const char* to, obj_ptr<Url_base>& retVal);
	virtual result_t normalize();
	virtual result_t get_href(std::string& retVal);
	virtual result_t get_protocol(std::string& retVal);
	virtual result_t get_slashes(int32_t& retVal);
	virtual result_t get_auth(std::string& retVal);
	virtual result_t get_username(std::string& retVal);
	virtual result_t get_password(std::string& retVal);
	virtual result_t get_host(std::string& retVal);
	virtual result_t get_hostname(std::string& retVal);
	virtual result_t get_port(std::string& retVal);
	virtual result_t get_path(std::string& retVal);
	virtual result_t get_pathname(std::string& retVal);
	virtual result_t get_search(std::string& retVal);
	virtual result_t get_query(std::string& retVal);
	virtual result_t get_hash(std::string& retVal);

private:
	void clear();

	void parseProtocol(const char*& url);
	void put_protocol(std::string str);

	void parseAuth(const char*& url);
	void parseHost(const char*& url);
	void parsePath(const char*& url);
	void parseQuery(const char*& url);
	void parseHash(const char*& url);

public:
	inline static void decodeURI(const char* url, int sz, std::string& retVal)
	{
		if (sz < 0)
			sz = (int)qstrlen(url);

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
			if (ch == '%' && l > 2 && qishex(src[1]) && qishex(src[2]))
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

			if (ch == '%' && l > 2 && qishex(src[1]) && qishex(src[2]))
			{
				*bstr++ = (qhex(src[1]) << 4) + qhex(src[2]);
				src += 2;
				l -= 2;
			}
			else
				*bstr++ = ch;
		}

		retVal = str;
	}

	inline static void encodeURI(const char* url, int sz, std::string& retVal,
			const char* tab)
	{
		static const char *hex = "0123456789ABCDEF";

		if (sz < 0)
			sz = (int)qstrlen(url);

		if (sz == 0)
			return;

		int len, l;
		const char* src;
		unsigned char ch;
		char* bstr;
		std::string str;

		for (len = 0, src = url, l = sz; l > 0; len++, l--)
		{
			ch = (unsigned char) *src++;
			if (ch < 0x20 || ch >= 0x80 || tab[ch - 0x20] == ' ')
				len += 2;
		}

		str.resize(len);
		bstr = &str[0];

		for (src = url, l = sz; l > 0; l--)
		{
			ch = (unsigned char) *src++;
			if (ch >= 0x20 && ch < 0x80 && tab[ch - 0x20] != ' ')
				*bstr++ = ch;
			else
			{
				*bstr++ = '%';

				*bstr++ = hex[(ch >> 4) & 15];
				*bstr++ = hex[ch & 15];
			}
		}

		retVal = str;
	}
public:
	std::string m_protocol;
	bool m_slashes;
	bool m_defslashes;
	std::string m_username;
	std::string m_password;
	std::string m_hostname;
	std::string m_port;
	std::string m_pathname;
	std::string m_query;
	std::string m_hash;
	bool m_ipv6;
};

} /* namespace fibjs */
#endif /* URL_H_ */
