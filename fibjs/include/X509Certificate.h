/*
 * X509Certificate.h
 *
 *  Created on: Fec 26, 2024
 *      Author: lion
 */

#pragma once

#include "crypto_util.h"
#include "ifs/X509Certificate.h"
#include "KeyObject.h"

namespace fibjs {

class X509Certificate : public X509Certificate_base {
public:
    X509Certificate(X509* cert = nullptr)
        : m_cert(cert)
    {
    }

public:
    // X509Certificate_base
    virtual result_t get_subject(exlib::string& retVal);
    virtual result_t get_serialNumber(exlib::string& retVal);
    virtual result_t get_publicKey(obj_ptr<KeyObject_base>& retVal);
    virtual result_t get_subjectAltName(exlib::string& retVal);
    virtual result_t get_infoAccess(exlib::string& retVal);
    virtual result_t get_issuer(exlib::string& retVal);
    virtual result_t get_ca(bool& retVal);
    virtual result_t get_pathlen(int32_t& retVal);
    virtual result_t get_keyUsage(v8::Local<v8::Array>& retVal);
    virtual result_t get_type(v8::Local<v8::Array>& retVal);
    virtual result_t get_validFrom(exlib::string& retVal);
    virtual result_t get_validTo(exlib::string& retVal);
    virtual result_t get_raw(obj_ptr<Buffer_base>& retVal);
    virtual result_t get_pem(exlib::string& retVal);
    virtual result_t get_fingerprint(exlib::string& retVal);
    virtual result_t get_fingerprint256(exlib::string& retVal);
    virtual result_t get_fingerprint512(exlib::string& retVal);
    virtual result_t next(obj_ptr<X509Certificate_base>& retVal);
    virtual result_t checkEmail(exlib::string email, v8::Local<v8::Object> options, exlib::string& retVal);
    virtual result_t checkHost(exlib::string name, v8::Local<v8::Object> options, exlib::string& retVal);
    virtual result_t checkIP(exlib::string ip, exlib::string& retVal);
    virtual result_t checkIssued(X509Certificate_base* issuer, bool& retVal);
    virtual result_t checkPrivateKey(KeyObject_base* privateKey, bool& retVal);
    virtual result_t verify(KeyObject_base* publicKey, bool& retVal);

public:
    virtual result_t toString(exlib::string& retVal);

public:
    result_t load_cert(Buffer_base* cert);
    result_t load_cert(v8::Local<v8::Array> certs);
    result_t get_x509_array(int32_t nid, const char** names, v8::Local<v8::Array>& retVal);

private:
    X509Pointer m_cert;
    obj_ptr<X509Certificate> m_next;
    obj_ptr<KeyObject_base> m_publicKey;
};

}
