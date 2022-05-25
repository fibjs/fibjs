/*
 * md_keccak256.cpp
 *
 *  Created on: May 26, 2022
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include <string.h>
#include "sha3.h"
#include "md_api.h"

namespace fibjs {

static int _start_256(mbedtls_md_context_t* ctx)
{
    sha3_Init256(ctx->md_ctx);
    sha3_SetFlags(ctx->md_ctx, SHA3_FLAGS_KECCAK);

    return 0;
}

static int _start_384(mbedtls_md_context_t* ctx)
{
    sha3_Init384(ctx->md_ctx);
    sha3_SetFlags(ctx->md_ctx, SHA3_FLAGS_KECCAK);

    return 0;
}

static int _start_512(mbedtls_md_context_t* ctx)
{
    sha3_Init512(ctx->md_ctx);
    sha3_SetFlags(ctx->md_ctx, SHA3_FLAGS_KECCAK);

    return 0;
}

static int _update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    sha3_Update(ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish(mbedtls_md_context_t* ctx, unsigned char* output)
{
    const void* h = sha3_Finalize(ctx->md_ctx);
    memcpy(output, h, ctx->md_info->size);
    return 0;
}

mbedtls_md_info_x mbedtls_keccak256_info = {
    { "KECCAK256",
        MBEDTLS_MD_KECCAK256,
        32,
        64 },
    sizeof(sha3_context),
    _start_256,
    _update,
    _finish
};

mbedtls_md_info_x mbedtls_keccak384_info = {
    { "KECCAK384",
        MBEDTLS_MD_KECCAK384,
        48,
        128 },
    sizeof(sha3_context),
    _start_512,
    _update,
    _finish
};

mbedtls_md_info_x mbedtls_keccak512_info = {
    { "KECCAK512",
        MBEDTLS_MD_KECCAK512,
        64,
        128 },
    sizeof(sha3_context),
    _start_512,
    _update,
    _finish
};

} /* namespace fibjs */
