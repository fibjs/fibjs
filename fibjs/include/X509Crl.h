/*
 * X509Crl.h
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "ifs/X509Crl.h"
#include <polarssl/x509_crl.h>

#ifndef _fj_X509CRL_H
#define _fj_X509CRL_H

namespace fibjs
{

class X509Crl : public X509Crl_base
{
public:
    X509Crl();
    ~X509Crl();

public:
    // X509Crl_base
    virtual result_t load(const char *pemCrl);

private:
    x509_crl m_crl;
};

}

#endif
