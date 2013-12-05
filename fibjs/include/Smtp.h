/*
 * Smtp.h
 *
 *  Created on: Dec 5, 2013
 *      Author: lion
 */

#ifndef SMTP_H_
#define SMTP_H_

#include "ifs/Smtp.h"
#include "BufferedStream.h"

namespace fibjs
{

class Smtp: public Smtp_base
{
public:
	// Smtp_base
	virtual result_t connect(const char* host, int32_t port, int32_t family, exlib::AsyncEvent* ac);
	virtual result_t command(const char* cmd, const char* arg, std::string& retVal, exlib::AsyncEvent* ac);
	virtual result_t hello(const char* hostname, exlib::AsyncEvent* ac);
	virtual result_t login(const char* username, const char* password, exlib::AsyncEvent* ac);
	virtual result_t from(const char* adress, exlib::AsyncEvent* ac);
	virtual result_t to(const char* adress, exlib::AsyncEvent* ac);
	virtual result_t data(const char* txt, exlib::AsyncEvent* ac);
	virtual result_t quit(exlib::AsyncEvent* ac);
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
	virtual result_t set_socket(Socket_base* newVal);

private:
	result_t command(const char* cmd, const char* arg, exlib::AsyncEvent* ac);

public:
	obj_ptr<Socket_base> m_sock;
};

} /* namespace fibjs */

#endif /* SMTP_H_ */
