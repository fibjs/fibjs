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

namespace fibjs {

class Smtp : public Smtp_base {
public:
    Smtp()
        : m_timeout(0)
    {
    }

public:
    // Smtp_base
    virtual result_t connect(exlib::string url, AsyncEvent* ac);
    virtual result_t command(exlib::string cmd, exlib::string arg, exlib::string& retVal, AsyncEvent* ac);
    virtual result_t hello(exlib::string hostname, AsyncEvent* ac);
    virtual result_t login(exlib::string username, exlib::string password, AsyncEvent* ac);
    virtual result_t from(exlib::string address, AsyncEvent* ac);
    virtual result_t to(exlib::string address, AsyncEvent* ac);
    virtual result_t data(exlib::string txt, AsyncEvent* ac);
    virtual result_t quit(AsyncEvent* ac);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_socket(obj_ptr<Stream_base>& retVal);

private:
    result_t command(exlib::string cmd, exlib::string arg, AsyncEvent* ac);

public:
    int32_t m_timeout;
    obj_ptr<Stream_base> m_conn;
    obj_ptr<BufferedStream_base> m_stmBuffered;
};

} /* namespace fibjs */

#endif /* SMTP_H_ */
