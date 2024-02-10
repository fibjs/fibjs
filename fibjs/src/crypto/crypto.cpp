/*
 * crypto.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/crypto.h"
#include "Cipher.h"
#include "Buffer.h"
#include "Digest.h"
#include "PKey.h"
#include "ECKey.h"
#include "PKey_rsa.h"
#include "X509Cert.h"
#include "X509Crl.h"
#include "X509Req.h"
#include "ssl.h"
#include <time.h>
#include <string.h>
#include <mbedtls/mbedtls/pkcs5.h>
#include "md_api.h"

namespace fibjs {

DECLARE_MODULE(crypto);

result_t crypto_base::createHash(exlib::string algo, obj_ptr<Digest_base>& retVal)
{
    exlib::qstrupr(algo);
    if (algo == "RMD160")
        algo = "RIPEMD160";

    mbedtls_md_type_t algo_id = _md_type_from_string(algo.c_str());
    if (algo_id == MBEDTLS_MD_NONE)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new Digest(algo_id);

    return 0;
}

result_t crypto_base::createHmac(exlib::string algo, Buffer_base* key,
    obj_ptr<Digest_base>& retVal)
{
    exlib::qstrupr(algo);
    if (algo == "RMD160")
        algo = "RIPEMD160";

    mbedtls_md_type_t algo_id = _md_type_from_string(algo.c_str());
    if (algo_id == MBEDTLS_MD_NONE)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return hash_base::hmac(algo_id, key, NULL, retVal);
}

result_t crypto_base::loadCert(exlib::string filename, obj_ptr<X509Cert_base>& retVal)
{
    return X509Cert::loadFile(filename, retVal);
}

result_t crypto_base::loadCrl(exlib::string filename, obj_ptr<X509Crl_base>& retVal)
{
    return X509Crl::loadFile(filename, retVal);
}

result_t crypto_base::loadReq(exlib::string filename, obj_ptr<X509Req_base>& retVal)
{
    return X509Req::loadFile(filename, retVal);
}

result_t crypto_base::loadPKey(exlib::string filename, obj_ptr<PKey_base>& retVal)
{
    return PKey::loadFile(filename, retVal);
}

result_t crypto_base::generateKey(int32_t size, obj_ptr<PKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return PKey_rsa::generateKey(size, retVal);
}

result_t crypto_base::generateKey(exlib::string curve, obj_ptr<PKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return ECKey::generateKey(curve, retVal);
}

inline int pkcs5_pbkdf1(mbedtls_md_context_t* ctx, const unsigned char* password,
    size_t plen, const unsigned char* salt, size_t slen,
    unsigned int iteration_count,
    uint32_t key_length, unsigned char* output)
{
    int ret;
    unsigned int i;
    unsigned char md1[MBEDTLS_MD_MAX_SIZE];
    unsigned char work[MBEDTLS_MD_MAX_SIZE];
    unsigned char md_size = mbedtls_md_get_size(ctx->md_info);
    size_t use_len;
    unsigned char* out_p = output;
    bool bFirst = true;

    if (iteration_count > 0xFFFFFFFF)
        return (MBEDTLS_ERR_PKCS5_BAD_INPUT_DATA);

    while (key_length) {
        if ((ret = _md_starts(ctx)) != 0)
            return (ret);

        if (!bFirst)
            if ((ret = _md_update(ctx, md1, md_size)) != 0)
                return (ret);
        bFirst = false;

        if ((ret = _md_update(ctx, password, plen)) != 0)
            return (ret);

        if ((ret = _md_update(ctx, salt, slen)) != 0)
            return (ret);

        if ((ret = _md_finish(ctx, work)) != 0)
            return (ret);

        memcpy(md1, work, md_size);

        for (i = 1; i < iteration_count; i++) {
            if ((ret = _md_starts(ctx)) != 0)
                return (ret);

            if ((ret = _md_update(ctx, md1, md_size)) != 0)
                return (ret);

            if ((ret = _md_finish(ctx, work)) != 0)
                return (ret);

            memcpy(md1, work, md_size);
        }

        use_len = (key_length < md_size) ? key_length : md_size;
        memcpy(out_p, work, use_len);

        key_length -= (uint32_t)use_len;
        out_p += use_len;
    }

    return (0);
}

result_t crypto_base::pbkdf1(Buffer_base* password, Buffer_base* salt, int32_t iterations,
    int32_t size, int32_t algo, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (iterations < 1 || size < 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (algo < hash_base::C_MD5 || algo > hash_base::C_SM3)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Buffer* buf_pass = Buffer::Cast(password);
    Buffer* buf_salt = Buffer::Cast(salt);
    obj_ptr<Buffer> buf_key = new Buffer(NULL, size);

    mbedtls_md_context_t ctx;
    _md_setup(&ctx, (mbedtls_md_type_t)algo, 1);
    pkcs5_pbkdf1(&ctx, buf_pass->data(), buf_pass->length(), buf_salt->data(), buf_salt->length(),
        iterations, size, buf_key->data());
    mbedtls_md_free(&ctx);

    retVal = buf_key;

    return 0;
}

result_t crypto_base::pbkdf1(Buffer_base* password, Buffer_base* salt, int32_t iterations,
    int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (iterations < 1 || size < 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::qstrupr(algoName);
    mbedtls_md_type_t algo_id = _md_type_from_string(algoName.c_str());
    if (algo_id == MBEDTLS_MD_NONE)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return pbkdf1(password, salt, iterations, size, algo_id, retVal, ac);
}

result_t crypto_base::pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations,
    int32_t size, int32_t algo, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (iterations < 1 || size < 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (algo < hash_base::C_MD5 || algo > hash_base::C_SM3)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Buffer* buf_pass = Buffer::Cast(password);
    Buffer* buf_salt = Buffer::Cast(salt);
    obj_ptr<Buffer> buf_key = new Buffer(NULL, size);

    mbedtls_md_context_t ctx;
    _md_setup(&ctx, (mbedtls_md_type_t)algo, 1);
    mbedtls_pkcs5_pbkdf2_hmac(&ctx, buf_pass->data(), buf_pass->length(), buf_salt->data(), buf_salt->length(),
        iterations, size, buf_key->data());
    mbedtls_md_free(&ctx);

    retVal = buf_key;

    return 0;
}

result_t crypto_base::pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations,
    int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (iterations < 1 || size < 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::qstrupr(algoName);
    mbedtls_md_type_t algo_id = _md_type_from_string(algoName.c_str());
    if (algo_id == MBEDTLS_MD_NONE)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return pbkdf2(password, salt, iterations, size, algo_id, retVal, ac);
}

extern obj_ptr<NArray> g_hashes;

result_t crypto_base::getHashes(v8::Local<v8::Array>& retVal)
{
    v8::Local<v8::Value> v;
    g_hashes->valueOf(v);

    retVal = v8::Local<v8::Array>::Cast(v);

    return 0;
}
}
