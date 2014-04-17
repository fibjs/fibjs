/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "ifs/ssl.h"
#include "SslSocket.h"
#include "polarssl/error.h"

namespace fibjs
{

_ssl g_ssl;

result_t _ssl::setError(int ret)
{
    char msg[128];

    polarssl_strerror(ret, msg, sizeof(msg));
    return Runtime::setError(msg);
}

result_t ssl_base::connect(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                           exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new SslSocket();
    return retVal->connect(s, ac);
}

result_t ssl_base::accept(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                          exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new SslSocket();
    return retVal->accept(s, ac);
}

}
