/*
 * SslHandler.h
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#ifndef _fj_SSLHANDLER_H_
#define _fj_SSLHANDLER_H_

#include "ifs/SslHandler.h"
#include "SslSocket.h"

namespace fibjs
{

class SslHandler: public SslHandler_base
{
    FIBER_FREE();
    
public:
    // object_base
    virtual result_t dispose()
    {
        return 0;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            exlib::AsyncEvent *ac);

public:
    // SslHandler_base
    virtual result_t get_verification(int32_t &retVal);
    virtual result_t set_verification(int32_t newVal);
    virtual result_t get_ca(obj_ptr<X509Cert_base> &retVal);
    virtual result_t get_handler(obj_ptr<Handler_base> &retVal);
    virtual result_t set_handler(Handler_base *newVal);

public:
    result_t init(v8::Local<v8::Array> certs, v8::Local<v8::Value> hdlr);
    result_t init(X509Cert_base *crt, PKey_base *key, v8::Local<v8::Value> hdlr);

private:
    Handler_base* m_hdlr;
    obj_ptr<SslSocket_base> m_socket;
};

} /* namespace fibjs */
#endif /* _fj_SSLHANDLER_H_ */
