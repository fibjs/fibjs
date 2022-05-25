
#include "mbedtls/src/md_wrap.h"
#include "mbedtls/error.h"

#pragma once

#define MBEDTLS_MD_KECCAK256 mbedtls_md_type_t(MBEDTLS_MD_SM3 + 1)
#define MBEDTLS_MD_KECCAK384 mbedtls_md_type_t(MBEDTLS_MD_SM3 + 2)
#define MBEDTLS_MD_KECCAK512 mbedtls_md_type_t(MBEDTLS_MD_SM3 + 3)
#define MBEDTLS_MD_BLAKE2S mbedtls_md_type_t(MBEDTLS_MD_SM3 + 4)
#define MBEDTLS_MD_BLAKE2B mbedtls_md_type_t(MBEDTLS_MD_SM3 + 5)
#define MBEDTLS_MD_BLAKE2SP mbedtls_md_type_t(MBEDTLS_MD_SM3 + 6)
#define MBEDTLS_MD_BLAKE2BP mbedtls_md_type_t(MBEDTLS_MD_SM3 + 7)
#define MBEDTLS_MD_MAX mbedtls_md_type_t(MBEDTLS_MD_SM3 + 8)

namespace fibjs {

struct mbedtls_md_info_x {
    mbedtls_md_info_t info;
    int32_t ctx_size;
    int (*start)(mbedtls_md_context_t* ctx);
    int (*update)(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen);
    int (*finish)(mbedtls_md_context_t* ctx, unsigned char* output);
};

mbedtls_md_type_t _md_type_from_string(const char* md_name);
int _md_setup(mbedtls_md_context_t* ctx, mbedtls_md_type_t algo, int hmac);
int _md_starts(mbedtls_md_context_t* ctx);
int _md_update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen);
int _md_finish(mbedtls_md_context_t* ctx, unsigned char* output);
int _md_hmac_starts(mbedtls_md_context_t* ctx, const unsigned char* key, size_t keylen);
int _md_hmac_finish(mbedtls_md_context_t* ctx, unsigned char* output);
int _md_hmac_reset(mbedtls_md_context_t* ctx);

}
