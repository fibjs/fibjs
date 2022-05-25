/*
 * md_blake2.cpp
 *
 *  Created on: May 26, 2022
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include <string.h>
#include "blake2/include/blake2.h"
#include "md_api.h"

namespace fibjs {

static int _start_s(mbedtls_md_context_t* ctx)
{
    blake2s_init((blake2s_state*)ctx->md_ctx, BLAKE2S_OUTBYTES);
    return 0;
}

static int _update_s(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    blake2s_update((blake2s_state*)ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_s(mbedtls_md_context_t* ctx, unsigned char* output)
{
    blake2s_final((blake2s_state*)ctx->md_ctx, output, BLAKE2S_OUTBYTES);
    return 0;
}

mbedtls_md_info_x mbedtls_blake2s_info = {
    { "BLAKE2S",
        MBEDTLS_MD_BLAKE2S,
        32,
        64 },
    sizeof(blake2s_state),
    _start_s,
    _update_s,
    _finish_s
};

static int _start_b(mbedtls_md_context_t* ctx)
{
    blake2b_init((blake2b_state*)ctx->md_ctx, BLAKE2B_OUTBYTES);
    return 0;
}

static int _update_b(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    blake2b_update((blake2b_state*)ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_b(mbedtls_md_context_t* ctx, unsigned char* output)
{
    blake2b_final((blake2b_state*)ctx->md_ctx, output, BLAKE2B_OUTBYTES);
    return 0;
}

mbedtls_md_info_x mbedtls_blake2b_info = {
    { "BLAKE2B",
        MBEDTLS_MD_BLAKE2B,
        64,
        128 },
    sizeof(blake2b_state),
    _start_b,
    _update_b,
    _finish_b
};

static int _start_sp(mbedtls_md_context_t* ctx)
{
    blake2sp_init((blake2sp_state*)ctx->md_ctx, BLAKE2S_OUTBYTES);
    return 0;
}

static int _update_sp(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    blake2sp_update((blake2sp_state*)ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_sp(mbedtls_md_context_t* ctx, unsigned char* output)
{
    blake2sp_final((blake2sp_state*)ctx->md_ctx, output, BLAKE2S_OUTBYTES);
    return 0;
}

mbedtls_md_info_x mbedtls_blake2sp_info = {
    { "BLAKE2SP",
        MBEDTLS_MD_BLAKE2SP,
        32,
        64 },
    sizeof(blake2sp_state),
    _start_sp,
    _update_sp,
    _finish_sp
};

static int _start_bp(mbedtls_md_context_t* ctx)
{
    blake2bp_init((blake2bp_state*)ctx->md_ctx, BLAKE2B_OUTBYTES);
    return 0;
}

static int _update_bp(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    blake2bp_update((blake2bp_state*)ctx->md_ctx, input, ilen);
    return 0;
}

static int _finish_bp(mbedtls_md_context_t* ctx, unsigned char* output)
{
    blake2bp_final((blake2bp_state*)ctx->md_ctx, output, BLAKE2B_OUTBYTES);
    return 0;
}

mbedtls_md_info_x mbedtls_blake2bp_info = {
    { "BLAKE2BP",
        MBEDTLS_MD_BLAKE2BP,
        64,
        128 },
    sizeof(blake2bp_state),
    _start_bp,
    _update_bp,
    _finish_bp
};

} /* namespace fibjs */
