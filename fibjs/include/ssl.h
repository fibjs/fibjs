/*
 * ssl.h
 *
 *  Created on: Apr 17, 2014
 *      Author: lion
 */

#pragma once

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include <mbedtls/mbedtls/mbedtls_config.h>
#include <mbedtls/mbedtls/entropy.h>
#include <mbedtls/mbedtls/ctr_drbg.h>
#include <mbedtls/mbedtls/platform.h>
#include <mbedtls/mbedtls/ssl.h>
#include <mbedtls/mbedtls/ssl_cache.h>

namespace fibjs {

class _ssl {
public:
    _ssl()
    {
        mbedtls_entropy_init(&entropy);

        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
            (const unsigned char*)"fibjs", 5);
    }

public:
    static result_t setError(int32_t ret);

public:
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
};

extern _ssl g_ssl;

} /* namespace fibjs */
