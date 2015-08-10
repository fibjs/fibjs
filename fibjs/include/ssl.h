/*
 * ssl.h
 *
 *  Created on: Apr 17, 2014
 *      Author: lion
 */

#ifndef _fj_SSL_H_
#define _fj_SSL_H_

#include "ifs/ssl.h"
#include <mbedtls/polarssl/config.h>
#include <mbedtls/polarssl/entropy.h>
#include <mbedtls/polarssl/ctr_drbg.h>
#include <mbedtls/polarssl/ssl.h>
#include <mbedtls/polarssl/ssl_cache.h>
#include "X509Cert.h"

namespace fibjs
{

class _ssl
{
public:
    _ssl()
    {
        entropy_init(&entropy);
        ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                      (const unsigned char *) "fibjs", 5);

        ssl_cache_init(&m_cache);
        m_authmode = ssl_base::_VERIFY_REQUIRED;

        m_min_version = SSL_MINOR_VERSION_0;
        m_max_version = SSL_MINOR_VERSION_2;
    }

    ~_ssl()
    {
        ssl_cache_free(&m_cache);

        entropy_free(&entropy);
    }

public:
    static result_t setError(int32_t ret);
    obj_ptr<X509Cert> &ca()
    {
        if (!m_ca)
            m_ca = new X509Cert();
        return m_ca;
    }

public:
    ssl_cache_context m_cache;
    entropy_context entropy;
    ctr_drbg_context ctr_drbg;
    int32_t m_authmode;
    int32_t m_min_version;
    int32_t m_max_version;

    obj_ptr<X509Cert_base> m_crt;
    obj_ptr<PKey_base> m_key;

private:
    obj_ptr<X509Cert> m_ca;
};

extern _ssl g_ssl;

} /* namespace fibjs */

#endif /* _fj_SSL_H_ */
