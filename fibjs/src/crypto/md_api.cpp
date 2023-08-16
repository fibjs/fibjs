/*
 * Digest.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include <string.h>
#include "md_api.h"

namespace fibjs {

extern mbedtls_md_info_x mbedtls_sha3_256_info;
extern mbedtls_md_info_x mbedtls_sha3_384_info;
extern mbedtls_md_info_x mbedtls_sha3_512_info;
extern mbedtls_md_info_x mbedtls_shake128_info;
extern mbedtls_md_info_x mbedtls_shake256_info;
extern mbedtls_md_info_x mbedtls_keccak256_info;
extern mbedtls_md_info_x mbedtls_keccak384_info;
extern mbedtls_md_info_x mbedtls_keccak512_info;
extern mbedtls_md_info_x mbedtls_blake2s_info;
extern mbedtls_md_info_x mbedtls_blake2b_info;
extern mbedtls_md_info_x mbedtls_blake2sp_info;
extern mbedtls_md_info_x mbedtls_blake2bp_info;

static mbedtls_md_info_x* md_infos[] = {
    &mbedtls_sha3_256_info,
    &mbedtls_sha3_384_info,
    &mbedtls_sha3_512_info,
    &mbedtls_shake128_info,
    &mbedtls_shake256_info,
    &mbedtls_keccak256_info,
    &mbedtls_keccak384_info,
    &mbedtls_keccak512_info,
    &mbedtls_blake2s_info,
    &mbedtls_blake2b_info,
    &mbedtls_blake2sp_info,
    &mbedtls_blake2bp_info
};

mbedtls_md_type_t _md_type_from_string(const char* md_name)
{
    for (int32_t i = 0; i < ARRAYSIZE(md_infos); i++) {
        if (!qstrcmp(md_name, md_infos[i]->info.name))
            return md_infos[i]->info.type;
    }

    const mbedtls_md_info_t* mi = mbedtls_md_info_from_string(md_name);
    if (!mi)
        return MBEDTLS_MD_NONE;

    return mbedtls_md_get_type(mi);
}

int _md_setup(mbedtls_md_context_t* ctx, mbedtls_md_type_t algo, int hmac)
{
    if (algo > MBEDTLS_MD_SM3 && algo < MBEDTLS_MD_MAX) {
        mbedtls_md_info_x* infox = md_infos[algo - MBEDTLS_MD_SM3 - 1];

        ctx->md_info = &infox->info;
        ctx->md_ctx = NULL;
        ctx->hmac_ctx = NULL;

        if (infox->ctx_size) {
            ctx->md_ctx = calloc(1, infox->ctx_size);
            if (ctx->md_ctx == NULL)
                return (MBEDTLS_ERR_MD_ALLOC_FAILED);
        }

        if (hmac != 0) {
            ctx->hmac_ctx = calloc(2, infox->info.block_size);
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
    if (ctx->md_info->type > MBEDTLS_MD_SM3 && ctx->md_info->type < MBEDTLS_MD_MAX) {
        mbedtls_md_info_x* infox = (mbedtls_md_info_x*)ctx->md_info;
        return infox->start(ctx);
    }

    return mbedtls_md_starts(ctx);
}

int _md_update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen)
{
    if (ctx->md_info->type > MBEDTLS_MD_SM3 && ctx->md_info->type < MBEDTLS_MD_MAX) {
        mbedtls_md_info_x* infox = (mbedtls_md_info_x*)ctx->md_info;
        return infox->update(ctx, input, ilen);
    }

    return mbedtls_md_update(ctx, input, ilen);
}

int _md_finish(mbedtls_md_context_t* ctx, unsigned char* output)
{
    if (ctx->md_info->type > MBEDTLS_MD_SM3 && ctx->md_info->type < MBEDTLS_MD_MAX) {
        mbedtls_md_info_x* infox = (mbedtls_md_info_x*)ctx->md_info;
        return infox->finish(ctx, output);
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
