/*
 * hash.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "Digest.h"
#include "Buffer.h"
#include "KeyObject.h"
#include "crypto_util.h"
#include <openssl/kdf.h>
#include <boost/preprocessor.hpp>

namespace fibjs {

std::vector<exlib::string> g_hashes;
class init_hashes {
public:
    init_hashes()
    {
        EVP_MD_do_all_sorted([](const EVP_MD* md,
                                 const char* from, const char* to,
                                 void* x) {
            if (from)
                g_hashes.push_back(from);
        },
            NULL);
    }
} s_init_hashes;

static const struct {
    const char* name;
    const char* md_name;
} s_algos[] = {
    { "dss1", "sha1" },
    { "sha3_256", "sha3-256" },
    { "sha3_384", "sha3-384" },
    { "sha3_512", "sha3-512" },
    { "blake2s", "blake2s256" },
    { "blake2b", "blake2b512" }
};

const EVP_MD* _evp_md_type(const char* algo)
{
    for (int i = 0; i < ARRAYSIZE(s_algos); i++)
        if (!qstricmp(algo, s_algos[i].name)) {
            algo = s_algos[i].md_name;
            break;
        }

    return EVP_get_digestbyname(algo);
}

result_t crypto_base::createHash(exlib::string algo, obj_ptr<Digest_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algo.c_str());
    if (md) {
        retVal = new Digest(md);
        return 0;
    }

    return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "createHash: unknown algorithm '%s'.", algo.c_str()));
}

static result_t _createHmac(exlib::string algo, const char* key, size_t keylen,
    obj_ptr<Digest_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algo.c_str());
    if (md) {
        if (EVP_MD_get_flags(md) & EVP_MD_FLAG_XOF)
            return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "createHmac: XOF hash '%s' is not supported.", algo.c_str()));

        retVal = new Digest(md, key, keylen);
        return 0;
    }

    return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "createHmac: unknown algorithm '%s'.", algo.c_str()));
}

// a string alternative of a KDF argument is decoded as utf8, once, in the
// asynchronous phase
template <typename T>
static result_t union_to_buffer(T& v, obj_ptr<Buffer_base>& buf)
{
    if (std::holds_alternative<exlib::string>(v))
        return Buffer_base::from(std::get<exlib::string>(v), "utf8", buf);

    buf = std::get<obj_ptr<Buffer_base>>(v);
    return 0;
}

result_t crypto_base::createHmac(exlib::string algo, Union_createHmac_key key,
    obj_ptr<Digest_base>& retVal)
{
    if (std::holds_alternative<obj_ptr<KeyObject_base>>(key)) {
        KeyObject* ko = (KeyObject*)std::get<obj_ptr<KeyObject_base>>(key).get();
        if (ko->type() != KeyObject::kKeyTypeSecret)
            return CHECK_ERROR(Runtime::setError("createHmac: Invalid key type"));

        return _createHmac(algo, (const char*)ko->data(), ko->length(), retVal);
    }

    obj_ptr<Buffer_base> keyBuf;
    result_t hr = union_to_buffer(key, keyBuf);
    if (hr < 0)
        return hr;

    Buffer* buf = Buffer::Cast(keyBuf);
    return _createHmac(algo, (const char*)buf->data(), buf->length(), retVal);
}

result_t crypto_base::hash(exlib::string algorithm, Union_hash_data data,
    exlib::string outputEncoding, v8::Local<v8::Value>& retVal)
{
    const EVP_MD* md = _evp_md_type(algorithm.c_str());
    if (!md)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "hash: unknown algorithm '%s'.", algorithm.c_str()));

    obj_ptr<Buffer_base> dataBuf;
    result_t hr = union_to_buffer(data, dataBuf);
    if (hr < 0)
        return hr;

    Buffer* buf = Buffer::Cast(dataBuf);
    obj_ptr<Buffer> ret = new Buffer(NULL, EVP_MD_size(md));

    EVP_Digest((const unsigned char*)buf->data(), buf->length(), (unsigned char*)ret->data(), NULL, md, NULL);

    return ret->toValue(outputEncoding, retVal);
}

result_t crypto_base::hkdf(exlib::string algoName, Union_hkdf_password password, Union_hkdf_salt salt,
    Union_hkdf_info info, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (size < 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "hkdf: size must be positive, received %d.", size));

    const EVP_MD* md = _evp_md_type(algoName.c_str());
    if (!md)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "hkdf: unknown algorithm '%s'.", algoName.c_str()));

    obj_ptr<Buffer_base> passwordBuf, saltOut, infoOut;
    result_t hr;

    hr = union_to_buffer(password, passwordBuf);
    if (hr < 0)
        return hr;

    hr = union_to_buffer(salt, saltOut);
    if (hr < 0)
        return hr;

    hr = union_to_buffer(info, infoOut);
    if (hr < 0)
        return hr;

    Buffer* buf = Buffer::Cast(passwordBuf);
    Buffer* saltBuf = Buffer::Cast(saltOut);
    Buffer* infoBuf = Buffer::Cast(infoOut);
    obj_ptr<Buffer> ret = new Buffer(NULL, size);
    EVPKeyCtxPointer pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    size_t keylen = size;

    if (EVP_PKEY_derive_init(pctx) <= 0
        || EVP_PKEY_CTX_set_hkdf_md(pctx, md) <= 0
        || EVP_PKEY_CTX_set1_hkdf_salt(pctx, (const unsigned char*)saltBuf->data(), saltBuf->length()) <= 0
        || EVP_PKEY_CTX_set1_hkdf_key(pctx, (const unsigned char*)buf->data(), buf->length()) <= 0
        || EVP_PKEY_CTX_add1_hkdf_info(pctx, (const unsigned char*)infoBuf->data(), infoBuf->length()) <= 0
        || EVP_PKEY_derive(pctx, (unsigned char*)ret->data(), &keylen) <= 0) {
        return openssl_error();
    }

    retVal = ret;

    return 0;
}

result_t crypto_base::pbkdf2(Union_pbkdf2_password password, Union_pbkdf2_salt salt, int32_t iterations,
    int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (iterations < 1 || size < 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "pbkdf2: iterations and size must be positive."));

    const EVP_MD* md = _evp_md_type(algoName.c_str());
    if (!md)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "pbkdf2: unknown algorithm '%s'.", algoName.c_str()));

    obj_ptr<Buffer_base> passwordBuf, saltOut;
    result_t hr;

    hr = union_to_buffer(password, passwordBuf);
    if (hr < 0)
        return hr;

    hr = union_to_buffer(salt, saltOut);
    if (hr < 0)
        return hr;

    Buffer* buf = Buffer::Cast(passwordBuf);
    Buffer* saltBuf = Buffer::Cast(saltOut);
    obj_ptr<Buffer> ret = new Buffer(NULL, size);

    int32_t hr2 = PKCS5_PBKDF2_HMAC((const char*)buf->data(), buf->length(),
        (const unsigned char*)saltBuf->data(), saltBuf->length(),
        iterations, md, size, ret->data());
    if (hr2 != 1)
        return openssl_error();

    retVal = ret;
    return 0;
}

class ScryptOptions : public obj_base {
public:
    LOAD_OPTIONS(ScryptOptions, (N)(r)(p)(maxmem));

public:
    std::optional<int64_t> N = 16384; // CPU/memory cost parameter (must be power of 2)
    std::optional<int32_t> r = 8; // Block size parameter
    std::optional<int32_t> p = 1; // Parallelization parameter
    std::optional<int64_t> maxmem = 32 * 1024 * 1024; // Default 32MB
};

result_t scrypt_load_options(v8::Local<v8::Object> options, AsyncEvent* ac)
{
    obj_ptr<ScryptOptions> opt;
    Isolate* isolate = Isolate::current(options);
    result_t hr = ScryptOptions::load(options, opt);
    if (hr < 0)
        return hr;

    // Validate N is a power of 2 and greater than 1
    uint64_t N = opt->N.value();
    if (N < 2 || (N & (N - 1)) != 0)
        return CHECK_ERROR(Runtime::setError("scrypt: N must be a power of 2 greater than 1, received %lld.", (long long)N));

    // Validate r and p are not zero
    if (opt->r.value() == 0 || opt->p.value() == 0)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "scrypt: r and p must be positive."));

    ac->m_ctx.resize(1);
    ac->m_ctx[0] = opt;

    return CALL_E_NOSYNC;
}

result_t crypto_base::scrypt(Union_scrypt_password password, Union_scrypt_salt salt, int32_t keylen,
    v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return scrypt_load_options(options, ac);

    if (keylen < 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "scrypt: keylen must be positive, received %d.", keylen));

    result_t ctx_hr = ac->ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    ScryptOptions* opt = (ScryptOptions*)ac->m_ctx[0].object();

    obj_ptr<Buffer_base> passwordBuf, saltOut;
    result_t hr;

    hr = union_to_buffer(password, passwordBuf);
    if (hr < 0)
        return hr;

    hr = union_to_buffer(salt, saltOut);
    if (hr < 0)
        return hr;

    Buffer* pwd = Buffer::Cast(passwordBuf);
    Buffer* saltBuf = Buffer::Cast(saltOut);
    obj_ptr<Buffer> ret = new Buffer(NULL, keylen);

    int32_t hr2 = EVP_PBE_scrypt((const char*)pwd->data(), pwd->length(),
        (const unsigned char*)saltBuf->data(), saltBuf->length(),
        opt->N.value(), opt->r.value(), opt->p.value(), opt->maxmem.value(),
        ret->data(), keylen);

    if (hr2 != 1)
        return openssl_error();

    retVal = ret;
    return 0;
}

result_t crypto_base::getHashes(std::vector<exlib::string>& retVal)
{
    retVal = g_hashes;
    return 0;
}

} /* namespace fibjs */
