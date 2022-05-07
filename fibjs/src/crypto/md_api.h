
#include "mbedtls/src/md_wrap.h"
#include "mbedtls/error.h"

#pragma once

#define MBEDTLS_MD_KECCAK256 mbedtls_md_type_t(MBEDTLS_MD_SM3 + 1)

namespace fibjs {
int _md_setup(mbedtls_md_context_t* ctx, mbedtls_md_type_t algo, int hmac);
int _md_starts(mbedtls_md_context_t* ctx);
int _md_update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen);
int _md_finish(mbedtls_md_context_t* ctx, unsigned char* output);
int _md_hmac_starts(mbedtls_md_context_t* ctx, const unsigned char* key, size_t keylen);
int _md_hmac_finish(mbedtls_md_context_t* ctx, unsigned char* output);
int _md_hmac_reset(mbedtls_md_context_t* ctx);

}
