/*
 * md_sha3.cpp
 *
 *  Created on: Aug 16, 2023
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include <string.h>
#include "fips202.h"
#include "md_api.h"

namespace fibjs {

static int _start_128(mbedtls_md_context_t* ctx)
{
    shake128_inc_init((shake128incctx*)&ctx->md_ctx);
    return 0;
}

static int _start_256(mbedtls_md_context_t* ctx)
{
    shake256_inc_init((shake256incctx*)&ctx->md_ctx);
    return 0;
}

static int _update_128(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    shake128_absorb((shake128ctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _update_256(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    shake256_absorb((shake256ctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_128(mbedtls_md_context_t* ctx, unsigned char* output)
{
    size_t outlen = 16;
    size_t nblocks = outlen / SHAKE128_RATE;
    uint8_t t[SHAKE128_RATE];

    shake128_squeezeblocks(output, nblocks, (shake128ctx*)&ctx->md_ctx);

    output += nblocks * SHAKE128_RATE;
    outlen -= nblocks * SHAKE128_RATE;

    if (outlen) {
        shake128_squeezeblocks(t, 1, (shake128ctx*)&ctx->md_ctx);
        for (size_t i = 0; i < outlen; ++i) {
            output[i] = t[i];
        }
    }

    shake128_ctx_release((shake128ctx*)&ctx->md_ctx);
    ctx->md_ctx = NULL;
    return 0;
}

static int _finish_256(mbedtls_md_context_t* ctx, unsigned char* output)
{
    size_t outlen = 32;
    size_t nblocks = outlen / SHAKE256_RATE;
    uint8_t t[SHAKE256_RATE];

    shake256_squeezeblocks(output, nblocks, (shake256ctx*)&ctx->md_ctx);

    output += nblocks * SHAKE256_RATE;
    outlen -= nblocks * SHAKE256_RATE;

    if (outlen) {
        shake256_squeezeblocks(t, 1, (shake256ctx*)&ctx->md_ctx);
        for (size_t i = 0; i < outlen; ++i) {
            output[i] = t[i];
        }
    }

    shake256_ctx_release((shake256ctx*)&ctx->md_ctx);
    ctx->md_ctx = NULL;
    return 0;
}

mbedtls_md_info_x mbedtls_shake128_info = {
    { "SHAKE128",
        MBEDTLS_MD_SHAKE128,
        16,
        32 },
    0,
    _start_128,
    _update_128,
    _finish_128
};

mbedtls_md_info_x mbedtls_shake256_info = {
    { "SHAKE256",
        MBEDTLS_MD_SHAKE256,
        32,
        64 },
    0,
    _start_256,
    _update_256,
    _finish_256
};

} /* namespace fibjs */
