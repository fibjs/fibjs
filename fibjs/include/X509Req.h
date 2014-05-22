/*
 * X509Req.h
 *
 *  Created on: Apr 18, 2014
 *      Author: lion
 */

#include "ifs/X509Req.h"
#include <polarssl/x509_csr.h>
#include "X509Cert.h"

#ifndef _fj_X509REQ_H
#define _fj_X509REQ_H

namespace fibjs
{

class X509Req : public X509Req_base
{
public:
    X509Req();
    ~X509Req();

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // X509Req_base
    virtual result_t load(Buffer_base *derReq);
    virtual result_t load(const char *pemReq);
    virtual result_t loadFile(const char *filename);
    virtual result_t exportPem(std::string &retVal);
    virtual result_t exportDer(obj_ptr<Buffer_base> &retVal);
    virtual result_t sign(const char *issuer, PKey_base *key, v8::Local<v8::Object> opts, obj_ptr<X509Cert_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t get_subject(std::string &retVal);
    virtual result_t get_publicKey(obj_ptr<PKey_base> &retVal);

public:
    result_t create(const char *subject, PKey_base *key, int32_t hash);

private:
    void clear();
    result_t parseString(v8::Local<v8::Value> v, const X509Cert::_name* p);

private:
    x509write_cert m_crt;

private:
    x509_csr m_csr;
};

}

#endif
