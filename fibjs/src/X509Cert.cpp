/*
 * X509Cert.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "X509Cert.h"
#include "Cipher.h"

namespace fibjs
{

result_t X509Cert_base::_new(obj_ptr<X509Cert_base> &retVal)
{
    retVal = new X509Cert();
    return 0;
}

X509Cert::X509Cert()
{
    x509_crt_init(&m_crt);
}

X509Cert::~X509Cert()
{
    x509_crt_free(&m_crt);
}

result_t X509Cert::load(Buffer_base *DerCrt)
{
    int ret;

    std::string crt;
    DerCrt->toString(crt);

    ret = x509_crt_parse_der(&m_crt, (const unsigned char *)crt.c_str(), crt.length());
    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t X509Cert::load(const char *pemCrt)
{
    int ret;

    ret = x509_crt_parse(&m_crt, (const unsigned char *)pemCrt, qstrlen(pemCrt));
    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

}
