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

static const unsigned int kNoDsaSignature = std::numeric_limits<unsigned int>::max();
static const int DEFAULT_PADDING = std::numeric_limits<int>::max();
static const int NO_SALTLEN = std::numeric_limits<int>::max();

static result_t get_sig_opt(Isolate* isolate, v8::Local<v8::Object> key, DSASigEnc& enc, int& padding, int& salt_len)
{
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    exlib::string dsaEncoding;
    hr = GetConfigValue(isolate, key, "dsaEncoding", dsaEncoding, true);
    if (hr == 0) {
        if (dsaEncoding == "ieee-p1363")
            enc = kSigEncP1363;
        else if (dsaEncoding != "der")
            return Runtime::setError("Invalid dsaEncoding");
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, key, "padding", padding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, key, "saltLength", salt_len, true);
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

    unsigned char* data = nullptr;
    int len = i2d_ECDSA_SIG(asn1_sig, &data);

    if (len <= 0)
        return;

    sig = new Buffer(data, len);
}

Sign::Sign(const EVP_MD* md)
{
    m_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(m_ctx, md, NULL);
}

result_t Sign::update(Buffer_base* data, obj_ptr<Sign_base>& retVal)
{
    Buffer* buf = Buffer::Cast(data);

    EVP_DigestUpdate(m_ctx, buf->data(), buf->length());
    retVal = this;

    return 0;
}

result_t Sign::update(exlib::string data, exlib::string codec, obj_ptr<Sign_base>& retVal)
{
    exlib::string _data;
    result_t hr = commonDecode(codec, data, _data);
    if (hr < 0)
        return hr;

    EVP_DigestUpdate(m_ctx, (const unsigned char*)_data.c_str(), _data.length());
    retVal = this;

    return 0;
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
    size_t sig_len = EVP_PKEY_size(pkey);

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

result_t Sign::sign(Buffer_base* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    obj_ptr<KeyObject_base> key;
    result_t hr = crypto_base::createPrivateKey(privateKey, key);
    if (hr != 0)
        return hr;

    return sign(key, encoding, retVal);
}

result_t Sign::sign(KeyObject_base* privateKey, exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    return sign(privateKey, kSigEncDER, DEFAULT_PADDING, RSA_PSS_SALTLEN_MAX_SIGN, encoding, retVal);
}

result_t Sign::sign(v8::Local<v8::Object> key, exlib::string encoding, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current(key);
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPrivateKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = (KeyObject*)(KeyObject_base*)key_;

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

result_t Verify::update(Buffer_base* data, obj_ptr<Verify_base>& retVal)
{
    Buffer* buf = Buffer::Cast(data);

    EVP_DigestUpdate(m_ctx, buf->data(), buf->length());
    retVal = this;

    return 0;
}

result_t Verify::update(exlib::string data, exlib::string codec, obj_ptr<Verify_base>& retVal)
{
    exlib::string _data;
    result_t hr = commonDecode(codec, data, _data);
    if (hr < 0)
        return hr;

    EVP_DigestUpdate(m_ctx, (const unsigned char*)_data.c_str(), _data.length());
    retVal = this;

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

result_t Verify::verify(Buffer_base* privateKey, Buffer_base* signature, bool& retVal)
{
    obj_ptr<KeyObject_base> key;
    result_t hr = crypto_base::createPublicKey(privateKey, key);
    if (hr != 0)
        return hr;

    return verify(key, signature, retVal);
}

result_t Verify::verify(KeyObject_base* privateKey, Buffer_base* signature, bool& retVal)
{
    return verify(privateKey, signature, kSigEncDER, DEFAULT_PADDING, RSA_PSS_SALTLEN_MAX_SIGN, retVal);
}

result_t Verify::verify(v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPublicKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = (KeyObject*)(KeyObject_base*)key_;

    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = RSA_PSS_SALTLEN_MAX_SIGN;
    hr = get_sig_opt(isolate, key, enc, padding, salt_len);
    if (hr < 0)
        return hr;

    return verify(key__, signature, enc, padding, salt_len, retVal);
}

result_t Verify::verify(Buffer_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal)
{
    obj_ptr<KeyObject_base> key;
    result_t hr = crypto_base::createPublicKey(privateKey, key);
    if (hr != 0)
        return hr;

    return verify(key, signature, encoding, retVal);
}

result_t Verify::verify(KeyObject_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal)
{
    return verify(privateKey, signature, encoding, kSigEncDER, DEFAULT_PADDING, RSA_PSS_SALTLEN_MAX_SIGN, retVal);
}

result_t Verify::verify(v8::Local<v8::Object> key, exlib::string signature, exlib::string encoding, bool& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = crypto_base::createPublicKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = (KeyObject*)(KeyObject_base*)key_;

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
        return Runtime::setError("Invalid algorithm");

    retVal = new Sign(md);
    return 0;
}

result_t crypto_base::createVerify(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Verify_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algorithm.c_str());
    if (!md)
        return Runtime::setError("Invalid algorithm");

    retVal = new Verify(md);
    return 0;
}

result_t _sign(v8::Local<v8::Value> algorithm, Buffer_base* data, KeyObject_base* privateKey, DSASigEnc enc, int padding, int salt_len, obj_ptr<Buffer_base>& retVal)
{
    result_t hr;
    const EVP_MD* md = nullptr;
    if (algorithm->IsString()) {
        exlib::string algo;
        hr = GetArgumentValue(Isolate::current(), algorithm, algo, true);
        if (hr < 0)
            return hr;

        md = _evp_md_type(algo.c_str());
        if (!md)
            return Runtime::setError("Invalid algorithm");
    }

    EVPMDPointer context = EVP_MD_CTX_new();
    EVP_PKEY_CTX* ctx = nullptr;
    KeyObject* key = (KeyObject*)privateKey;
    EVP_PKEY* pkey = key->pkey();

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

result_t crypto_base::sign(v8::Local<v8::Value> algorithm, Buffer_base* data, Buffer_base* privateKey, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<KeyObject_base> key_;
    result_t hr = crypto_base::createPrivateKey(privateKey, key_);
    if (hr != 0)
        return hr;

    return _sign(algorithm, data, key_, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t crypto_base::sign(v8::Local<v8::Value> algorithm, Buffer_base* data, KeyObject_base* privateKey, obj_ptr<Buffer_base>& retVal)
{
    return _sign(algorithm, data, privateKey, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t crypto_base::sign(v8::Local<v8::Value> algorithm, Buffer_base* data, v8::Local<v8::Object> key, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<KeyObject_base> key_;
    result_t hr = crypto_base::createPrivateKey(key, key_);
    if (hr != 0)
        return hr;

    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = NO_SALTLEN;
    hr = get_sig_opt(Isolate::current(), key, enc, padding, salt_len);
    if (hr < 0)
        return hr;

    return _sign(algorithm, data, key_, enc, padding, salt_len, retVal);
}

result_t _verify(v8::Local<v8::Value> algorithm, Buffer_base* data, KeyObject_base* publicKey, Buffer_base* signature,
    DSASigEnc enc, int padding, int salt_len, bool& retVal)
{
    result_t hr;
    const EVP_MD* md = nullptr;
    if (algorithm->IsString()) {
        exlib::string algo;
        hr = GetArgumentValue(Isolate::current(), algorithm, algo, true);
        if (hr < 0)
            return hr;

        md = _evp_md_type(algo.c_str());
        if (!md)
            return Runtime::setError("Invalid algorithm");
    }

    EVPMDPointer context = EVP_MD_CTX_new();
    EVP_PKEY_CTX* ctx = nullptr;
    KeyObject* key = (KeyObject*)publicKey;
    EVP_PKEY* pkey = key->pkey();

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

result_t crypto_base::verify(v8::Local<v8::Value> algorithm, Buffer_base* data, Buffer_base* publicKey, Buffer_base* signature, bool& retVal)
{
    obj_ptr<KeyObject_base> key_;
    result_t hr = crypto_base::createPublicKey(publicKey, key_);
    if (hr != 0)
        return hr;

    return _verify(algorithm, data, key_, signature, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t crypto_base::verify(v8::Local<v8::Value> algorithm, Buffer_base* data, KeyObject_base* publicKey, Buffer_base* signature, bool& retVal)
{
    return _verify(algorithm, data, publicKey, signature, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t crypto_base::verify(v8::Local<v8::Value> algorithm, Buffer_base* data, v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal)
{
    obj_ptr<KeyObject_base> key_;
    result_t hr = crypto_base::createPublicKey(key, key_);
    if (hr != 0)
        return hr;

    DSASigEnc enc = kSigEncDER;
    int padding = DEFAULT_PADDING;
    int salt_len = NO_SALTLEN;
    hr = get_sig_opt(Isolate::current(), key, enc, padding, salt_len);
    if (hr < 0)
        return hr;

    return _verify(algorithm, data, key_, signature, enc, padding, salt_len, retVal);
}

}
