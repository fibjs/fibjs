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

static int _start_256(mbedtls_md_context_t* ctx)
{
    sha3_256_inc_init((sha3_256incctx*)&ctx->md_ctx);
    return 0;
}

static int _start_384(mbedtls_md_context_t* ctx)
{
    sha3_384_inc_init((sha3_384incctx*)&ctx->md_ctx);
    return 0;
}

static int _start_512(mbedtls_md_context_t* ctx)
{
    sha3_512_inc_init((sha3_512incctx*)&ctx->md_ctx);
    return 0;
}

static int _update_256(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    sha3_256_inc_absorb((sha3_256incctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _update_384(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    sha3_384_inc_absorb((sha3_384incctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _update_512(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    sha3_512_inc_absorb((sha3_512incctx*)&ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_256(mbedtls_md_context_t* ctx, unsigned char* output)
{
    sha3_256_inc_finalize(output, (sha3_256incctx*)&ctx->md_ctx, 0x06);
    ctx->md_ctx = NULL;
    return 0;
}

static int _finish_384(mbedtls_md_context_t* ctx, unsigned char* output)
{
    sha3_384_inc_finalize(output, (sha3_384incctx*)&ctx->md_ctx, 0x06);
    ctx->md_ctx = NULL;
    return 0;
}

static int _finish_512(mbedtls_md_context_t* ctx, unsigned char* output)
{
    sha3_512_inc_finalize(output, (sha3_512incctx*)&ctx->md_ctx, 0x06);
    ctx->md_ctx = NULL;
    return 0;
}

mbedtls_md_info_x mbedtls_sha3_256_info = {
    { "SHA3_256",
        MBEDTLS_MD_SHA3_256,
        32,
        64 },
    0,
    _start_256,
    _update_256,
    _finish_256
};

mbedtls_md_info_x mbedtls_sha3_384_info = {
    { "SHA3_384",
        MBEDTLS_MD_SHA3_384,
        48,
        128 },
    0,
    _start_384,
    _update_384,
    _finish_384
};

mbedtls_md_info_x mbedtls_sha3_512_info = {
    { "SHA3_512",
        MBEDTLS_MD_SHA3_512,
        64,
        128 },
    0,
    _start_512,
    _update_512,
    _finish_512
};

} /* namespace fibjs */
