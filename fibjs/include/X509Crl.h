/*
 * X509Crl.h
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/X509Crl.h"
#include <mbedtls/mbedtls/x509_crl.h>

namespace fibjs {

class X509Crl : public X509Crl_base {
public:
    X509Crl();
    X509Crl(X509Crl* root, int32_t no);
    ~X509Crl();

public:
    // X509Crl_base
    virtual result_t load(Buffer_base* derCrl);
    virtual result_t load(exlib::string pemCrl);
    virtual result_t loadFile(exlib::string filename);
    virtual result_t dump(bool pem, v8::Local<v8::Array>& retVal);
    virtual result_t clear();
    virtual result_t get_version(int32_t& retVal);
    virtual result_t get_issuer(exlib::string& retVal);
    virtual result_t get_serials(v8::Local<v8::Array>& retVal);
    virtual result_t get_thisUpdate(date_t& retVal);
    virtual result_t get_nextUpdate(date_t& retVal);
    virtual result_t get_next(obj_ptr<X509Crl_base>& retVal);

private:
    mbedtls_x509_crl* get_crl();

private:
    mbedtls_x509_crl m_crl;
    obj_ptr<X509Crl> m_root;
    int32_t m_no;
};
}
