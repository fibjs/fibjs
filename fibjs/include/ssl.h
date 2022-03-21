/*
 * ssl.h
 *
 *  Created on: Apr 17, 2014
 *      Author: lion
 */

#pragma once

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "ifs/ssl.h"
#include <mbedtls/mbedtls/mbedtls_config.h>
#include <mbedtls/mbedtls/entropy.h>
#include <mbedtls/mbedtls/ctr_drbg.h>
#include <mbedtls/mbedtls/platform.h>
#include <mbedtls/mbedtls/ssl.h>
#include <mbedtls/mbedtls/ssl_cache.h>
#include "X509Cert.h"

namespace fibjs {

class _ssl {
public:
    _ssl()
    {
        mbedtls_entropy_init(&entropy);

        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
            (const unsigned char*)"fibjs", 5);

        mbedtls_ssl_cache_init(&m_cache);
        m_authmode = ssl_base::C_VERIFY_REQUIRED;
    }

    ~_ssl()
    {
        mbedtls_ssl_cache_free(&m_cache);

        mbedtls_entropy_free(&entropy);
    }

public:
    static result_t setError(int32_t ret);

public:
    mbedtls_ssl_cache_context m_cache;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    int32_t m_authmode;

    obj_ptr<X509Cert_base> m_crt;
    obj_ptr<PKey_base> m_key;

private:
    obj_ptr<X509Cert> m_ca;
};

extern _ssl g_ssl;

} /* namespace fibjs */
