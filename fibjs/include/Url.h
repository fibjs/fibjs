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

private:
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
