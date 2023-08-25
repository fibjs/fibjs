/*
 * md_sha3.cpp
 *
 *  Created on: Aug 16, 2023
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include <string.h>
#include <fips202/include/fips202.h>
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
    shake128_inc_absorb((shake128incctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _update_256(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    shake256_inc_absorb((shake256incctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_128(mbedtls_md_context_t* ctx, unsigned char* output)
{
    shake128_inc_finalize((shake128incctx*)&ctx->md_ctx);
    shake128_inc_squeeze(output, 16, (shake128incctx*)&ctx->md_ctx);
    shake128_inc_ctx_release((shake128incctx*)&ctx->md_ctx);
    ctx->md_ctx = NULL;
    return 0;
}

static int _finish_256(mbedtls_md_context_t* ctx, unsigned char* output)
{
    shake256_inc_finalize((shake256incctx*)&ctx->md_ctx);
    shake256_inc_squeeze(output, 32, (shake256incctx*)&ctx->md_ctx);
    shake256_inc_ctx_release((shake256incctx*)&ctx->md_ctx);
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
