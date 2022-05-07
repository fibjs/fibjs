/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include <string.h>
#include "sha3.h"
#include "md_api.h"

namespace fibjs {

static const mbedtls_md_info_t mbedtls_keccak256_info = {
    "KECCAK256",
    MBEDTLS_MD_KECCAK256,
    32,
    64,
};

int _md_setup(mbedtls_md_context_t* ctx, mbedtls_md_type_t algo, int hmac)
{
    if (algo == MBEDTLS_MD_KECCAK256) {
        ctx->md_info = &mbedtls_keccak256_info;
        ctx->md_ctx = NULL;
        ctx->hmac_ctx = NULL;

        ctx->md_ctx = calloc(1, sizeof(sha3_context));
        if (ctx->md_ctx == NULL)
            return (MBEDTLS_ERR_MD_ALLOC_FAILED);

        if (hmac != 0) {
            ctx->hmac_ctx = calloc(2, mbedtls_keccak256_info.block_size);
            if (ctx->hmac_ctx == NULL) {
                mbedtls_md_free(ctx);
                return (MBEDTLS_ERR_MD_ALLOC_FAILED);
            }
        }

        return 0;
    }

    return mbedtls_md_setup(ctx, mbedtls_md_info_from_type(algo), hmac);
}

int _md_starts(mbedtls_md_context_t* ctx)
{
    if (ctx->md_info->type == MBEDTLS_MD_KECCAK256) {
        sha3_Init256(ctx->md_ctx);
        sha3_SetFlags(ctx->md_ctx, SHA3_FLAGS_KECCAK);

        return 0;
    }

    return mbedtls_md_starts(ctx);
}

int _md_update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    if (ctx->md_info->type == MBEDTLS_MD_KECCAK256) {
        sha3_Update(ctx->md_ctx, input, ilen);
        return 0;
    }

    return mbedtls_md_update(ctx, input, ilen);
}

int _md_finish(mbedtls_md_context_t* ctx, unsigned char* output)
{
    if (ctx->md_info->type == MBEDTLS_MD_KECCAK256) {
        const void* h = sha3_Finalize(ctx->md_ctx);
        memcpy(output, h, 32);
        return 0;
    }

    return mbedtls_md_finish(ctx, output);
}

int _md_hmac_starts(mbedtls_md_context_t* ctx, const unsigned char* key, size_t keylen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char sum[MBEDTLS_MD_MAX_SIZE];
    unsigned char *ipad, *opad;
    size_t i;

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);

    if (keylen > (size_t)ctx->md_info->block_size) {
        if ((ret = _md_starts(ctx)) != 0)
            goto cleanup;
        if ((ret = _md_update(ctx, key, keylen)) != 0)
            goto cleanup;
        if ((ret = _md_finish(ctx, sum)) != 0)
            goto cleanup;

        keylen = ctx->md_info->size;
        key = sum;
    }

    ipad = (unsigned char*)ctx->hmac_ctx;
    opad = (unsigned char*)ctx->hmac_ctx + ctx->md_info->block_size;

    memset(ipad, 0x36, ctx->md_info->block_size);
    memset(opad, 0x5C, ctx->md_info->block_size);

    for (i = 0; i < keylen; i++) {
        ipad[i] = (unsigned char)(ipad[i] ^ key[i]);
        opad[i] = (unsigned char)(opad[i] ^ key[i]);
    }

    if ((ret = _md_starts(ctx)) != 0)
        goto cleanup;
    if ((ret = _md_update(ctx, ipad,
             ctx->md_info->block_size))
        != 0)
        goto cleanup;

cleanup:
    mbedtls_platform_zeroize(sum, sizeof(sum));

    return (ret);
}

int _md_hmac_finish(mbedtls_md_context_t* ctx, unsigned char* output)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char tmp[MBEDTLS_MD_MAX_SIZE];
    unsigned char* opad;

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);

    opad = (unsigned char*)ctx->hmac_ctx + ctx->md_info->block_size;

    if ((ret = _md_finish(ctx, tmp)) != 0)
        return (ret);
    if ((ret = _md_starts(ctx)) != 0)
        return (ret);
    if ((ret = _md_update(ctx, opad,
             ctx->md_info->block_size))
        != 0)
        return (ret);
    if ((ret = _md_update(ctx, tmp,
             ctx->md_info->size))
        != 0)
        return (ret);
    return (_md_finish(ctx, output));
}

int _md_hmac_reset(mbedtls_md_context_t* ctx)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char* ipad;

    if (ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL)
        return (MBEDTLS_ERR_MD_BAD_INPUT_DATA);

    ipad = (unsigned char*)ctx->hmac_ctx;

    if ((ret = _md_starts(ctx)) != 0)
        return (ret);
    return (_md_update(ctx, ipad, ctx->md_info->block_size));
}

} /* namespace fibjs */
