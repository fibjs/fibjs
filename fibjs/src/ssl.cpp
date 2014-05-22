/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "ifs/ssl.h"
#include "SslSocket.h"
#include "X509Cert.h"
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

result_t ssl_base::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    retVal = g_ssl.m_ca;
    return 0;
}

result_t ssl_base::get_verification(int32_t &retVal)
{
    retVal = g_ssl.m_authmode;
    return 0;
}

result_t ssl_base::set_verification(int32_t newVal)
{
    if (newVal < ssl_base::_VERIFY_NONE || newVal > ssl_base::_VERIFY_REQUIRED)
        return CALL_E_INVALIDARG;

    g_ssl.m_authmode = newVal;
    return 0;
}

}
