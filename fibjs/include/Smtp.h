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
    virtual result_t connect(exlib::string host, int32_t port, int32_t family, AsyncEvent *ac);
    virtual result_t command(exlib::string cmd, exlib::string arg, exlib::string &retVal, AsyncEvent *ac);
    virtual result_t hello(exlib::string hostname, AsyncEvent *ac);
    virtual result_t login(exlib::string username, exlib::string password, AsyncEvent *ac);
    virtual result_t from(exlib::string address, AsyncEvent *ac);
    virtual result_t to(exlib::string address, AsyncEvent *ac);
    virtual result_t data(exlib::string txt, AsyncEvent *ac);
    virtual result_t quit(AsyncEvent *ac);
    virtual result_t get_socket(obj_ptr<Socket_base> &retVal);

private:
    result_t command(exlib::string cmd, exlib::string arg, AsyncEvent *ac);

public:
    obj_ptr<Socket_base> m_sock;
    obj_ptr<BufferedStream_base> m_stmBuffered;
};

} /* namespace fibjs */

#endif /* SMTP_H_ */
