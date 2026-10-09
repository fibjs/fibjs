/*
 * sign.cpp
 *
 *  Created on: Feb 24, 2024
 *      Author: lion
 */

#include "object.h"
#include "sign.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include "Buffer.h"
#include "KeyObject.h"

namespace fibjs {

static result_t get_sig_opt(Isolate* isolate, v8::Local<v8::Object> key, DSASigEnc& enc, int& padding, int& salt_len)
{
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    exlib::string dsaEncoding;
    hr = GetConfigValue(key, "dsaEncoding", dsaEncoding, true);
    if (hr == 0) {
        if (dsaEncoding == "ieee-p1363")
            enc = kSigEncP1363;
        else if (dsaEncoding != "der")
            return Runtime::setError("Invalid dsaEncoding: '%s'.", dsaEncoding.c_str());
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(key, "padding", padding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(key, "saltLength", salt_len, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    return 0;
}

static bool ApplyRSAOptions(const EVP_PKEY* pkey, EVP_PKEY_CTX* pkctx, int padding, int salt_len)
{
    int pk_id = EVP_PKEY_id(pkey);
    if (pk_id == EVP_PKEY_RSA || pk_id == EVP_PKEY_RSA2 || pk_id == EVP_PKEY_RSA_PSS) {
        if (padding == DEFAULT_PADDING)
            padding = pk_id == EVP_PKEY_RSA_PSS ? RSA_PKCS1_PSS_PADDING : RSA_PKCS1_PADDING;

        if (EVP_PKEY_CTX_set_rsa_padding(pkctx, padding) <= 0)
            return false;
        if (padding == RSA_PKCS1_PSS_PADDING && salt_len != NO_SALTLEN)
            if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkctx, salt_len) <= 0)
                return false;
    }

    return true;
}

unsigned int GetBytesOfRS(EVP_PKEY* pkey)
{
    int bits, base_id = EVP_PKEY_base_id(pkey);

    if (base_id == EVP_PKEY_DSA) {
        const DSA* dsa_key = EVP_PKEY_get0_DSA(pkey);
        // Both r and s are computed mod q, so their width is limited by that of q.
        bits = BN_num_bits(DSA_get0_q(dsa_key));
    } else if (base_id == EVP_PKEY_EC) {
        const EC_KEY* ec_key = EVP_PKEY_get0_EC_KEY(pkey);
        const EC_GROUP* ec_group = EC_KEY_get0_group(ec_key);
        bits = EC_GROUP_order_bits(ec_group);
    } else {
        return kNoDsaSignature;
    }

    return (bits + 7) / 8;
}

static void ConvertSignatureToP1363(EVP_PKEY* pkey, obj_ptr<Buffer>& sig)
{
    unsigned int n = GetBytesOfRS(pkey);
    if (n == kNoDsaSignature)
        return;

    const unsigned char* sig_data = sig->data();
    ECDSASigPointer asn1_sig = d2i_ECDSA_SIG(nullptr, &sig_data, sig->length());

    const BIGNUM* pr = ECDSA_SIG_get0_r(asn1_sig);
    const BIGNUM* ps = ECDSA_SIG_get0_s(asn1_sig);

    unsigned char* sig_buf = sig->data();
    BN_bn2binpad(pr, sig_buf, n);
    sig_buf += n;
    BN_bn2binpad(ps, sig_buf, n);
    sig->resize(2 * n);
}

static void ConvertSignatureToDER(EVP_PKEY* pkey, const unsigned char* sig_data, size_t sig_len, obj_ptr<Buffer>& sig)
{
    unsigned int n = GetBytesOfRS(pkey);
    if (n == kNoDsaSignature)
        return;

    if (sig_len != 2 * n)
        return;

    ECDSASigPointer asn1_sig(ECDSA_SIG_new());
    BIGNUM* r = BN_new();
    BIGNUM* s = BN_new();
    BN_bin2bn(sig_data, n, r);
    BN_bin2bn(sig_data + n, n, s);
    ECDSA_SIG_set0(asn1_sig, r, s);

    size_t sig_len_ = EVP_PKEY_size(pkey);
    sig = new Buffer(NULL, sig_len_);

    unsigned char* data = sig->data();
    int len = i2d_ECDSA_SIG(asn1_sig, &data);

    if (len <= 0)
        return;

    sig->resize(len);
}

Sign::Sign(const EVP_MD* md)
{
    m_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(m_ctx, md, NULL);
}

result_t Sign::update(Union_update_data data, exlib::string codec, obj_ptr<Sign_base>& retVal)
{
    retVal = this;

    if (std::holds_alternative<exlib::string>(data)) {
        // a string is decoded with codec; the buffer form ignores it, as it did
        // before the merge (it had no codec parameter)
        exlib::string _data;
        result_t hr = commonDecode(codec, std::get<exlib::string>(data), _data);
        if (hr < 0)
            return hr;

        EVP_DigestUpdate(m_ctx, (const unsigned char*)_data.c_str(), _data.length());

        return 0;
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));

    EVP_DigestUpdate(m_ctx, buf->data(), buf->length());

    return 0;
}

static bool IsOneShot(const EVP_PKEY* key)
{
    switch (EVP_PKEY_id(key)) {
    case EVP_PKEY_ED25519:
    case EVP_PKEY_ED448:
        return true;
    default:
        return false;
    }
}

result_t Sign::sign(KeyObject_base* key, DSASigEnc enc, int padding, int salt_len,
    exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    unsigned char m[EVP_MAX_MD_SIZE];
    unsigned int m_len;
    if (!EVP_DigestFinal_ex(m_ctx, m, &m_len))
        return openssl_error();

    KeyObject* key_ = (KeyObject*)key;
    EVP_PKEY* pkey = key_->pkey();

    // a secret key has no asymmetric key material; node reports
    // ERR_CRYPTO_INVALID_KEY_OBJECT_TYPE here, the null EVP_PKEY used to crash
    if (pkey == nullptr)
        return Runtime::setError("Sign: invalid key type, expected a private key");

    size_t sig_len = EVP_PKEY_size(pkey);

    if (IsOneShot(pkey))
        return Runtime::setError("One-shot signature algorithms do not support sign");

    obj_ptr<Buffer> sig = new Buffer(NULL, sig_len);

    EVPKeyCtxPointer pkctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!pkctx)
        return openssl_error();

    if (EVP_PKEY_sign_init(pkctx) <= 0)
        return openssl_error();

    if (!ApplyRSAOptions(pkey, pkctx, padding, salt_len))
        return openssl_error();

    if (EVP_PKEY_CTX_set_signature_md(pkctx, EVP_MD_CTX_md(m_ctx)) <= 0)
        return openssl_error();

    if (EVP_PKEY_sign(pkctx, sig->data(), &sig_len, m, m_len) <= 0)
        return openssl_error();

    if (sig_len == 0)
        sig = new Buffer(NULL, 0);
    else {
        sig->resize(sig_len);
        if (enc == kSigEncP1363)
            ConvertSignatureToP1363(pkey, sig);
    }

    return sig->toValue(encoding, retVal);
}

result_t Sign::sign(Union_sign_privateKey privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return sign_keyobj(std::get<obj_ptr<KeyObject_base>>(privateKey), encoding, retVal);

    if (std::holds_alternative<v8::Local<v8::Object>>(privateKey))
        return sign_opts(std::get<v8::Local<v8::Object>>(privateKey), encoding, retVal);

    obj_ptr<KeyObject_base> key;
    result_t hr;

    if (std::holds_alternative<exlib::string>(privateKey))
        hr = crypto_base::createPrivateKey(std::get<exlib::string>(privateKey), key);
    else
        hr = crypto_base::createPrivateKey(std::get<obj_ptr<Buffer_base>>(privateKey), key);

    if (hr != 0)
        return hr;

    return sign_keyobj(key, encoding, retVal);
}

result_t Sign::sign_keyobj(KeyObject_base* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    return sign(privateKey, kSigEncDER, DEFAULT_PADDING, RSA_PSS_SALTLEN_MAX_SIGN, encoding, retVal);
}

result_t Sign::sign_opts(v8::Local<v8::Object> key, exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current(key);
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPrivateKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = key_.As<KeyObject>();

    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = RSA_PSS_SALTLEN_MAX_SIGN;
    hr = get_sig_opt(isolate, key, enc, padding, salt_len);
    if (hr < 0)
        return hr;

    return sign(key__, enc, padding, salt_len, encoding, retVal);
}

Verify::Verify(const EVP_MD* md)
{
    m_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(m_ctx, md, NULL);
}

result_t Verify::update(Union_update_data data, exlib::string codec, obj_ptr<Verify_base>& retVal)
{
    retVal = this;

    if (std::holds_alternative<exlib::string>(data)) {
        // a string is decoded with codec; the buffer form ignores it, as it did
        // before the merge (it had no codec parameter)
        exlib::string _data;
        result_t hr = commonDecode(codec, std::get<exlib::string>(data), _data);
        if (hr < 0)
            return hr;

        EVP_DigestUpdate(m_ctx, (const unsigned char*)_data.c_str(), _data.length());

        return 0;
    }

    Buffer* buf = Buffer::Cast(std::get<obj_ptr<Buffer_base>>(data));

    EVP_DigestUpdate(m_ctx, buf->data(), buf->length());

    return 0;
}

result_t Verify::verify(KeyObject_base* key, const unsigned char* signature, size_t sig_len, DSASigEnc enc,
    int padding, int salt_len, bool& retVal)
{
    unsigned char m[EVP_MAX_MD_SIZE];
    unsigned int m_len;
    if (!EVP_DigestFinal_ex(m_ctx, m, &m_len))
        return openssl_error();

    KeyObject* key_ = (KeyObject*)key;
    EVP_PKEY* pkey = key_->pkey();

    if (pkey == nullptr)
        return Runtime::setError("Verify: invalid key type, expected a public or private key");

    if (IsOneShot(pkey))
        return Runtime::setError("One-shot signature algorithms do not support verify");

    EVPKeyCtxPointer pkctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!pkctx)
        return openssl_error();

    if (EVP_PKEY_verify_init(pkctx) <= 0)
        return openssl_error();

    if (!ApplyRSAOptions(pkey, pkctx, padding, salt_len))
        return openssl_error();

    if (EVP_PKEY_CTX_set_signature_md(pkctx, EVP_MD_CTX_md(m_ctx)) <= 0)
        return openssl_error();

    obj_ptr<Buffer> sig;
    if (enc == kSigEncP1363) {
        ConvertSignatureToDER(pkey, signature, sig_len, sig);
        if (sig) {
            signature = sig->data();
            sig_len = sig->length();
        }
    }

    retVal = EVP_PKEY_verify(pkctx, signature, sig_len, m, m_len) == 1;

    return 0;
}

result_t Verify::verify(KeyObject_base* key, Buffer_base* signature, DSASigEnc enc, int padding, int salt_len,
    bool& retVal)
{
    Buffer* sig = Buffer::Cast(signature);
    return verify(key, (const unsigned char*)sig->data(), sig->length(), enc, padding, salt_len, retVal);
}

result_t Verify::verify(KeyObject_base* key, exlib::string signature, exlib::string encoding, DSASigEnc enc,
    int padding, int salt_len, bool& retVal)
{
    exlib::string _signature;
    result_t hr = commonDecode(encoding, signature, _signature);
    if (hr < 0)
        return hr;
    return verify(key, (const unsigned char*)_signature.c_str(), _signature.length(), enc, padding, salt_len, retVal);
}

result_t Verify::verify(Union_verify_privateKey privateKey, Union_verify_signature signature,
    exlib::string encoding, bool& retVal)
{
    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey)) {
        obj_ptr<KeyObject_base> key = std::get<obj_ptr<KeyObject_base>>(privateKey);

        if (std::holds_alternative<obj_ptr<Buffer_base>>(signature))
            return verify_keyobj(key, std::get<obj_ptr<Buffer_base>>(signature), retVal);

        return verify_keyobj(key, std::get<exlib::string>(signature), encoding, retVal);
    }

    if (std::holds_alternative<v8::Local<v8::Object>>(privateKey)) {
        v8::Local<v8::Object> key = std::get<v8::Local<v8::Object>>(privateKey);

        if (std::holds_alternative<obj_ptr<Buffer_base>>(signature))
            return verify_opts(key, std::get<obj_ptr<Buffer_base>>(signature), retVal);

        return verify_opts(key, std::get<exlib::string>(signature), encoding, retVal);
    }

    obj_ptr<KeyObject_base> key;
    result_t hr;

    if (std::holds_alternative<exlib::string>(privateKey))
        hr = crypto_base::createPublicKey(std::get<exlib::string>(privateKey), key);
    else
        hr = crypto_base::createPublicKey(std::get<obj_ptr<Buffer_base>>(privateKey), key);

    if (hr != 0)
        return hr;

    if (std::holds_alternative<obj_ptr<Buffer_base>>(signature))
        return verify_keyobj(key, std::get<obj_ptr<Buffer_base>>(signature), retVal);

    return verify_keyobj(key, std::get<exlib::string>(signature), encoding, retVal);
}

result_t Verify::verify_keyobj(KeyObject_base* privateKey, Buffer_base* signature, bool& retVal)
{
    return verify(privateKey, signature, kSigEncDER, DEFAULT_PADDING, RSA_PSS_SALTLEN_MAX_SIGN, retVal);
}

result_t Verify::verify_opts(v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPublicKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = key_.As<KeyObject>();

    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = RSA_PSS_SALTLEN_MAX_SIGN;
    hr = get_sig_opt(isolate, key, enc, padding, salt_len);
    if (hr < 0)
        return hr;

    return verify(key__, signature, enc, padding, salt_len, retVal);
}

result_t Verify::verify_keyobj(KeyObject_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal)
{
    return verify(privateKey, signature, encoding, kSigEncDER, DEFAULT_PADDING, RSA_PSS_SALTLEN_MAX_SIGN, retVal);
}

result_t Verify::verify_opts(v8::Local<v8::Object> key, exlib::string signature, exlib::string encoding, bool& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPublicKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = key_.As<KeyObject>();

    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = RSA_PSS_SALTLEN_MAX_SIGN;
    hr = get_sig_opt(isolate, key, enc, padding, salt_len);
    if (hr < 0)
        return hr;

    return verify(key__, signature, encoding, enc, padding, salt_len, retVal);
}

result_t crypto_base::createSign(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Sign_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algorithm.c_str());
    if (!md)
        return Runtime::setError("createSign: unknown algorithm '%s'.", algorithm.c_str());

    retVal = new Sign(md);
    return 0;
}

result_t crypto_base::createVerify(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Verify_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algorithm.c_str());
    if (!md)
        return Runtime::setError("createVerify: unknown algorithm '%s'.", algorithm.c_str());

    retVal = new Verify(md);
    return 0;
}

result_t get_algorithm(Isolate* isolate, v8::Local<v8::Value> algorithm, exlib::string& retVal)
{
    if (algorithm->IsNull() || algorithm->IsUndefined())
        return 0;

    result_t hr = GetArgumentValue(isolate, algorithm, retVal, true);
    if (hr < 0)
        return hr;

    if (retVal.empty())
        return Runtime::setError("Invalid algorithm: algorithm must not be empty.");

    return 0;
}

result_t _sign(exlib::string algorithm, Buffer_base* data, KeyObject_base* privateKey, DSASigEnc enc, int padding, int salt_len, obj_ptr<Buffer_base>& retVal)
{
    const EVP_MD* md = nullptr;
    if (!algorithm.empty()) {
        md = _evp_md_type(algorithm.c_str());
        if (!md)
            return Runtime::setError("Invalid algorithm: '%s'.", algorithm.c_str());
    }

    EVPMDPointer context = EVP_MD_CTX_new();
    EVP_PKEY_CTX* ctx = nullptr;
    KeyObject* key = (KeyObject*)privateKey;
    EVP_PKEY* pkey = key->pkey();

    // a secret key has no asymmetric key material (node reports
    // ERR_CRYPTO_INVALID_KEY_OBJECT_TYPE instead of an openssl error)
    if (pkey == nullptr)
        return Runtime::setError("Sign: invalid key type, expected a private key");

    if (EVP_DigestSignInit(context, &ctx, md, nullptr, pkey) <= 0)
        return openssl_error();

    if (!ApplyRSAOptions(pkey, ctx, padding, salt_len))
        return openssl_error();

    size_t sig_len = EVP_PKEY_size(pkey);
    obj_ptr<Buffer> sig = new Buffer(NULL, sig_len);

    Buffer* buf = Buffer::Cast(data);
    if (EVP_DigestSign(context, sig->data(), &sig_len, buf->data(), buf->length()) <= 0)
        return openssl_error();

    if (sig_len == 0)
        sig = new Buffer(NULL, 0);
    else {
        sig->resize(sig_len);
        if (enc == kSigEncP1363)
            ConvertSignatureToP1363(pkey, sig);
    }

    retVal = sig;

    return 0;
}


result_t _verify(exlib::string algorithm, Buffer_base* data, KeyObject_base* publicKey, Buffer_base* signature,
    DSASigEnc enc, int padding, int salt_len, bool& retVal)
{
    const EVP_MD* md = nullptr;
    if (!algorithm.empty()) {
        md = _evp_md_type(algorithm.c_str());
        if (!md)
            return Runtime::setError("Invalid algorithm: '%s'.", algorithm.c_str());
    }

    EVPMDPointer context = EVP_MD_CTX_new();
    EVP_PKEY_CTX* ctx = nullptr;
    KeyObject* key = (KeyObject*)publicKey;
    EVP_PKEY* pkey = key->pkey();

    if (pkey == nullptr)
        return Runtime::setError("Verify: invalid key type, expected a public or private key");

    if (EVP_DigestVerifyInit(context, &ctx, md, nullptr, pkey) <= 0)
        return openssl_error();

    if (!ApplyRSAOptions(pkey, ctx, padding, salt_len))
        return openssl_error();

    Buffer* buf = Buffer::Cast(data);
    Buffer* sig = Buffer::Cast(signature);

    obj_ptr<Buffer> sig_;
    if (enc == kSigEncP1363) {
        ConvertSignatureToDER(pkey, sig->data(), sig->length(), sig_);
        if (sig_)
            sig = sig_;
    }

    retVal = EVP_DigestVerify(context, sig->data(), sig->length(), buf->data(), buf->length()) == 1;

    return 0;
}



result_t crypto_base::sign(v8::Local<v8::Value> algorithm, Union_sign_data data, Union_sign_key key,
    obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    bool bObject = std::holds_alternative<v8::Local<v8::Object>>(key);

    if (ac.isSync()) {
        Isolate* isolate = ac.isolate();

        // the options object is readable in the synchronous phase only: the
        // callback phase receives an empty handle. It carries the key and the
        // signing parameters.
        ac.ctxv().resize(bObject ? 5 : 1);

        exlib::string algo;
        result_t hr = get_algorithm(isolate, algorithm, algo);
        if (hr < 0)
            return hr;
        ac.ctxv()[0] = algo;

        if (bObject) {
            v8::Local<v8::Object> opt = std::get<v8::Local<v8::Object>>(key);

            obj_ptr<KeyObject_base> key_;
            hr = crypto_base::createPrivateKey(opt, key_);
            if (hr != 0)
                return hr;
            ac.ctxv()[1] = key_;

            DSASigEnc enc = kSigEncDER;
            int padding = DEFAULT_PADDING;
            int salt_len = NO_SALTLEN;
            hr = get_sig_opt(isolate, opt, enc, padding, salt_len);
            if (hr < 0)
                return hr;
            ac.ctxv()[2] = (int)enc;
            ac.ctxv()[3] = padding;
            ac.ctxv()[4] = salt_len;
        }

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    // the algorithm and the object key options were prepared in the sync phase
    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    exlib::string algo = ac.ctxv()[0].string();

    // a string is decoded once, in the async phase
    obj_ptr<Buffer_base> buf;
    if (std::holds_alternative<exlib::string>(data)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", buf);
        if (hr < 0)
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(data);

    obj_ptr<KeyObject_base> key_;
    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = NO_SALTLEN;

    if (bObject) {
        // the key and its options come from the sync phase
        result_t ctx_hr = ac.ctx(4);
        if (ctx_hr < 0)
            return ctx_hr;

        key_ = (KeyObject_base*)ac.ctxv()[1].object();
        if (key_ == NULL)
            return Runtime::setError("crypto: the key options were not read");

        enc = (DSASigEnc)ac.ctxv()[2].intVal();
        padding = ac.ctxv()[3].intVal();
        salt_len = ac.ctxv()[4].intVal();
    } else if (std::holds_alternative<obj_ptr<KeyObject_base>>(key))
        key_ = std::get<obj_ptr<KeyObject_base>>(key);
    else if (std::holds_alternative<obj_ptr<Buffer_base>>(key)) {
        result_t hr = crypto_base::createPrivateKey(std::get<obj_ptr<Buffer_base>>(key), key_);
        if (hr != 0)
            return hr;
    } else {
        // createPrivateKey parses the string itself, it is not decoded here
        result_t hr = crypto_base::createPrivateKey(std::get<exlib::string>(key), key_);
        if (hr != 0)
            return hr;
    }

    return _sign(algo, buf, key_, enc, padding, salt_len, retVal);
}


// Mixed string / Buffer argument lists: every string is decoded once, in the
// async phase, where the public key is parsed and the signature is checked.
// Without these overloads a Buffer argument would fall through to the
// all-strings overload, which would stringify it (lossy) instead of failing.


result_t crypto_base::verify(v8::Local<v8::Value> algorithm, Union_verify_data data, Union_verify_key key,
    Union_verify_signature signature, bool& retVal, AsyncHandle ac)
{
    bool bObject = std::holds_alternative<v8::Local<v8::Object>>(key);

    if (ac.isSync()) {
        Isolate* isolate = ac.isolate();

        // the options object is readable in the synchronous phase only: the
        // callback phase receives an empty handle. It carries the key and the
        // verifying parameters.
        ac.ctxv().resize(bObject ? 5 : 1);

        exlib::string algo;
        result_t hr = get_algorithm(isolate, algorithm, algo);
        if (hr < 0)
            return hr;
        ac.ctxv()[0] = algo;

        if (bObject) {
            v8::Local<v8::Object> opt = std::get<v8::Local<v8::Object>>(key);

            obj_ptr<KeyObject_base> key_;
            hr = crypto_base::createPublicKey(opt, key_);
            if (hr != 0)
                return hr;
            ac.ctxv()[1] = key_;

            DSASigEnc enc = kSigEncDER;
            int padding = DEFAULT_PADDING;
            int salt_len = NO_SALTLEN;
            hr = get_sig_opt(isolate, opt, enc, padding, salt_len);
            if (hr < 0)
                return hr;
            ac.ctxv()[2] = (int)enc;
            ac.ctxv()[3] = padding;
            ac.ctxv()[4] = salt_len;
        }

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    // the algorithm and the object key options were prepared in the sync phase
    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    exlib::string algo = ac.ctxv()[0].string();

    // strings are decoded once, in the async phase
    obj_ptr<Buffer_base> dataBuf;
    if (std::holds_alternative<exlib::string>(data)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", dataBuf);
        if (hr < 0)
            return hr;
    } else
        dataBuf = std::get<obj_ptr<Buffer_base>>(data);

    obj_ptr<Buffer_base> sigBuf;
    if (std::holds_alternative<exlib::string>(signature)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(signature), "utf8", sigBuf);
        if (hr < 0)
            return hr;
    } else
        sigBuf = std::get<obj_ptr<Buffer_base>>(signature);

    obj_ptr<KeyObject_base> key_;
    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = NO_SALTLEN;

    if (bObject) {
        // the key and its options come from the sync phase
        result_t ctx_hr = ac.ctx(4);
        if (ctx_hr < 0)
            return ctx_hr;

        key_ = (KeyObject_base*)ac.ctxv()[1].object();
        if (key_ == NULL)
            return Runtime::setError("crypto: the key options were not read");

        enc = (DSASigEnc)ac.ctxv()[2].intVal();
        padding = ac.ctxv()[3].intVal();
        salt_len = ac.ctxv()[4].intVal();
    } else if (std::holds_alternative<obj_ptr<KeyObject_base>>(key))
        key_ = std::get<obj_ptr<KeyObject_base>>(key);
    else if (std::holds_alternative<obj_ptr<Buffer_base>>(key)) {
        result_t hr = crypto_base::createPublicKey(std::get<obj_ptr<Buffer_base>>(key), key_);
        if (hr != 0)
            return hr;
    } else {
        result_t hr = crypto_base::createPublicKey(std::get<exlib::string>(key), key_);
        if (hr != 0)
            return hr;
    }

    return _verify(algo, dataBuf, key_, sigBuf, enc, padding, salt_len, retVal);
}


}
