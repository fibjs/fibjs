/*
 * X509Cert.h
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "ifs/X509Cert.h"
#include <polarssl/polarssl/x509_crt.h>

#ifndef _fj_X509CERT_H
#define _fj_X509CERT_H

namespace fibjs
{

class X509Cert : public X509Cert_base
{
public:
    X509Cert();
    X509Cert(X509Cert *root, int no);
    ~X509Cert();

public:
    // X509Cert_base
    virtual result_t load(Buffer_base *derCert);
    virtual result_t load(const char *txtCert);
    virtual result_t loadFile(const char *filename);
    virtual result_t loadRootCerts();
    virtual result_t verify(X509Cert_base *cert, bool &retVal, exlib::AsyncEvent *ac);
    virtual result_t dump(v8::Local<v8::Array> &retVal);
    virtual result_t clear();
    virtual result_t get_version(int32_t &retVal);
    virtual result_t get_serial(std::string &retVal);
    virtual result_t get_issuer(std::string &retVal);
    virtual result_t get_subject(std::string &retVal);
    virtual result_t get_notBefore(date_t &retVal);
    virtual result_t get_notAfter(date_t &retVal);
    virtual result_t get_ca(bool &retVal);
    virtual result_t get_pathlen(int32_t &retVal);
    virtual result_t get_usage(std::string &retVal);
    virtual result_t get_type(std::string &retVal);
    virtual result_t get_publicKey(obj_ptr<PKey_base> &retVal);
    virtual result_t get_next(obj_ptr<X509Cert_base> &retVal);


public:
    result_t load(const x509_crt *crt);

public:
    struct _name
    {
        int32_t id;
        const char *name;
    };

    static _name g_usages[];
    static _name g_types[];

public:
    x509_crt m_crt;

private:
    x509_crt *get_crt();

private:
    struct _cert
    {
        int32_t size;
        const char *data;
    };

    static _cert g_root_ca[];

private:
    obj_ptr<X509Cert> m_root;
    int m_no;
};

}

#endif
