/*
 * ssl.h
 *
 *  Created on: Apr 17, 2014
 *      Author: lion
 */

#ifndef _fj_SSL_H_
#define _fj_SSL_H_

#include "ifs/ssl.h"
#include <mbedtls/mbedtls/config.h>
#include <mbedtls/mbedtls/entropy.h>
#include <mbedtls/mbedtls/ctr_drbg.h>
#include <mbedtls/mbedtls/platform.h>
#include <mbedtls/mbedtls/ssl.h>
#include <mbedtls/mbedtls/ssl_cache.h>
#include "X509Cert.h"

namespace fibjs
{

class _ssl
{
public:
    _ssl()
    {
        mbedtls_entropy_init(&entropy);

        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                              (const unsigned char *) "fibjs", 5);

        mbedtls_ssl_cache_init(&m_cache);
        m_authmode = ssl_base::_VERIFY_REQUIRED;

        m_min_version = MBEDTLS_SSL_MINOR_VERSION_0;
        m_max_version = MBEDTLS_SSL_MINOR_VERSION_3;
    }

    ~_ssl()
    {
        mbedtls_ssl_cache_free(&m_cache);

        mbedtls_entropy_free(&entropy);
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
    mbedtls_ssl_cache_context m_cache;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
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
