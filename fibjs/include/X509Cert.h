/*
 * X509Cert.h
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "ifs/X509Cert.h"
#include <polarssl/x509_crt.h>

#ifndef _fj_X509CERT_H
#define _fj_X509CERT_H

namespace fibjs
{

class X509Cert : public X509Cert_base
{
public:
    X509Cert();
    ~X509Cert();

public:
    // X509Cert_base
    virtual result_t load(Buffer_base *DerCert);
    virtual result_t load(const char *txtCert);
    virtual result_t loadFile(const char *filename);
    virtual result_t dump(v8::Local<v8::Array> &retVal);
    virtual result_t clear();

private:
    x509_crt m_crt;
};

}

#endif
