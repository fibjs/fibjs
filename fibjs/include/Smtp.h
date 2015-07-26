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
    virtual result_t connect(const char *host, int32_t port, int32_t family, AsyncEvent *ac);
    virtual result_t command(const char *cmd, const char *arg, std::string &retVal, AsyncEvent *ac);
    virtual result_t hello(const char *hostname, AsyncEvent *ac);
    virtual result_t login(const char *username, const char *password, AsyncEvent *ac);
    virtual result_t from(const char *address, AsyncEvent *ac);
    virtual result_t to(const char *address, AsyncEvent *ac);
    virtual result_t data(const char *txt, AsyncEvent *ac);
    virtual result_t quit(AsyncEvent *ac);
    virtual result_t get_socket(obj_ptr<Socket_base> &retVal);

private:
    result_t command(const char *cmd, const char *arg, AsyncEvent *ac);

public:
    obj_ptr<Socket_base> m_sock;
    obj_ptr<BufferedStream_base> m_stmBuffered;
};

} /* namespace fibjs */

#endif /* SMTP_H_ */
