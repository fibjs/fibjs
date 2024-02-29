/*
 * X509CertificateRequest.h
 *
 *  Created on: Fec 27, 2024
 *      Author: lion
 */

#pragma once

#include "crypto_util.h"
#include "ifs/X509CertificateRequest.h"
#include "KeyObject.h"

namespace fibjs {

class X509CertificateRequest : public X509CertificateRequest_base {
public:
    // X509CertificateRequest_base
    virtual result_t get_subject(exlib::string& retVal);
    virtual result_t get_publicKey(obj_ptr<KeyObject_base>& retVal);
    virtual result_t get_subjectAltName(exlib::string& retVal);
    virtual result_t get_infoAccess(exlib::string& retVal);
    virtual result_t get_raw(obj_ptr<Buffer_base>& retVal);
    virtual result_t get_pem(exlib::string& retVal);
    virtual result_t checkPrivateKey(KeyObject_base* privateKey, bool& retVal);
    virtual result_t issue(v8::Local<v8::Object> options, obj_ptr<X509Certificate_base>& retVal);

public:
    virtual result_t toString(exlib::string& retVal);

public:
    result_t load_csr(Buffer_base* csr);
    result_t create(v8::Local<v8::Object> options);
    result_t fill_option(X509* cert, v8::Local<v8::Object> options, const char* key, int nid, const char** names);

private:
    X509ReqPointer m_csr;
    obj_ptr<KeyObject_base> m_publicKey;
};

}
