/*
 * X509Crl.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "X509Crl.h"
#include "Cipher.h"

namespace fibjs
{

result_t X509Crl_base::_new(obj_ptr<X509Crl_base> &retVal)
{
    retVal = new X509Crl();
    return 0;
}

X509Crl::X509Crl()
{
    x509_crl_init(&m_crl);
}

X509Crl::~X509Crl()
{
    x509_crl_free(&m_crl);
}

result_t X509Crl::load(const char *pemCrl)
{
    int ret;

    ret = x509_crl_parse(&m_crl, (const unsigned char *)pemCrl, qstrlen(pemCrl));
    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

}
