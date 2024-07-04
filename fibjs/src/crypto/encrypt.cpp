/*
 * encrypt.cpp
 *
 *  Created on: feb 22, 2024
 *      Author: lion
 */

#include "object.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include "Buffer.h"
#include "KeyObject.h"

namespace fibjs {

typedef int (*EVP_PKEY_cipher_init_t)(EVP_PKEY_CTX* ctx);
typedef int (*EVP_PKEY_cipher_t)(EVP_PKEY_CTX* ctx, unsigned char* out, size_t* outlen, const unsigned char* in, size_t inlen);
typedef result_t (*createKey_t)(Buffer_base*, obj_ptr<KeyObject_base>&);
typedef result_t (*createKeyOpt_t)(v8::Local<v8::Object>, obj_ptr<KeyObject_base>&);

static bool SetRsaOaepLabel(const EVPKeyCtxPointer& ctx, Buffer_base* label)
{
    if (!label)
        return true;

    Buffer* label_ = Buffer::Cast(label);
    if (label_->length() > 0) {
        void* label_copy = OPENSSL_memdup(label_->data(), label_->length());
        if (!label_copy)
            return false;
        int ret = EVP_PKEY_CTX_set0_rsa_oaep_label(ctx, (unsigned char*)label_copy, label_->length());
        if (ret <= 0) {
            OPENSSL_free(label_copy);
            return false;
        }
    }
    return true;
}

template <EVP_PKEY_cipher_init_t EVP_PKEY_cipher_init, EVP_PKEY_cipher_t EVP_PKEY_cipher>
result_t PKEY_cipher(EVP_PKEY* pkey, int padding, const EVP_MD* digest,
    Buffer_base* oaep_label, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    EVPKeyCtxPointer ctx(EVP_PKEY_CTX_new(pkey, nullptr));
    if (!ctx)
        return openssl_error();

    if (EVP_PKEY_cipher_init(ctx) <= 0)
        return openssl_error();

    if (EVP_PKEY_id(pkey) == EVP_PKEY_RSA) {
        if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0)
            return openssl_error();
    }

    if (digest != nullptr)
        if (EVP_PKEY_CTX_set_rsa_oaep_md(ctx, digest) <= 0)
            return openssl_error();

    if (!SetRsaOaepLabel(ctx, oaep_label))
        return openssl_error();

    Buffer* buffer_ = Buffer::Cast(buffer);
    size_t out_len = 0;
    if (EVP_PKEY_cipher(ctx, nullptr, &out_len, buffer_->data(), buffer_->length()) <= 0)
        return openssl_error();

    obj_ptr<Buffer> out = new Buffer(NULL, out_len);
    if (EVP_PKEY_cipher(ctx, (unsigned char*)out->data(), &out_len, buffer_->data(), buffer_->length()) <= 0)
        return openssl_error();

    if (out_len > 0)
        out->resize(out_len);
    else
        out = new Buffer(NULL, 0);

    retVal = out;
    return 0;
}

template <EVP_PKEY_cipher_init_t EVP_PKEY_cipher_init, EVP_PKEY_cipher_t EVP_PKEY_cipher>
result_t PKEY_cipher(KeyObject_base* key, int padding, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    KeyObject* key_ = (KeyObject*)key;
    return PKEY_cipher<EVP_PKEY_cipher_init, EVP_PKEY_cipher>(key_->pkey(), padding, nullptr, nullptr, buffer, retVal);
}

template <createKey_t createKey, EVP_PKEY_cipher_init_t EVP_PKEY_cipher_init, EVP_PKEY_cipher_t EVP_PKEY_cipher>
result_t PKEY_cipher(Buffer_base* key, int padding, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<KeyObject_base> key_;
    result_t hr = createKey(key, key_);
    if (hr != 0)
        return hr;

    return PKEY_cipher<EVP_PKEY_cipher_init, EVP_PKEY_cipher>(key_, padding, buffer, retVal);
}

template <createKeyOpt_t createKey, EVP_PKEY_cipher_init_t EVP_PKEY_cipher_init, EVP_PKEY_cipher_t EVP_PKEY_cipher, bool useo_aep>
result_t PKEY_cipher(v8::Local<v8::Object> key, int padding, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::GetCurrent(key);
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> v;
    result_t hr;

    obj_ptr<KeyObject_base> key_;
    hr = createKey(key, key_);
    if (hr < 0)
        return hr;
    KeyObject* key__ = key_.As<KeyObject>();

    hr = GetConfigValue(isolate, key, "padding", padding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string encoding = "utf8";
    hr = GetConfigValue(isolate, key, "encoding", encoding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    const EVP_MD* digest = nullptr;
    if (useo_aep) {
        exlib::string oaepHash = "sha1";
        hr = GetConfigValue(isolate, key, "oaepHash", oaepHash, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        digest = _evp_md_type(oaepHash.c_str());
        if (!digest)
            return Runtime::setError("Invalid oaepHash");
    }

    obj_ptr<Buffer_base> oaep_label;
    hr = GetConfigValue(isolate, key, "oaepLabel", v);
    if (hr == 0) {
        hr = GetArgumentValue(isolate, v, oaep_label, false, encoding.c_str());
        if (hr < 0)
            return hr;
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    obj_ptr<Buffer_base> buffer_;
    hr = GetArgumentValue(isolate, buffer, buffer_, false, encoding.c_str());
    if (hr < 0)
        return hr;

    return PKEY_cipher<EVP_PKEY_cipher_init, EVP_PKEY_cipher>(key__->pkey(), padding, digest, oaep_label, buffer_, retVal);
}

result_t crypto_base::privateDecrypt(Buffer_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPrivateKey,
        EVP_PKEY_decrypt_init,
        EVP_PKEY_decrypt>(privateKey, RSA_PKCS1_OAEP_PADDING, buffer, retVal);
}

result_t crypto_base::privateDecrypt(KeyObject_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        EVP_PKEY_decrypt_init,
        EVP_PKEY_decrypt>(privateKey, RSA_PKCS1_OAEP_PADDING, buffer, retVal);
}

result_t crypto_base::privateDecrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPrivateKey,
        EVP_PKEY_decrypt_init,
        EVP_PKEY_decrypt,
        true>(key, RSA_PKCS1_OAEP_PADDING, buffer, retVal);
}

result_t crypto_base::privateEncrypt(Buffer_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPrivateKey,
        EVP_PKEY_sign_init,
        EVP_PKEY_sign>(privateKey, RSA_PKCS1_PADDING, buffer, retVal);
}

result_t crypto_base::privateEncrypt(KeyObject_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        EVP_PKEY_sign_init,
        EVP_PKEY_sign>(privateKey, RSA_PKCS1_PADDING, buffer, retVal);
}

result_t crypto_base::privateEncrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPrivateKey,
        EVP_PKEY_sign_init,
        EVP_PKEY_sign,
        false>(key, RSA_PKCS1_PADDING, buffer, retVal);
}

result_t crypto_base::publicDecrypt(Buffer_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPublicKey,
        EVP_PKEY_verify_recover_init,
        EVP_PKEY_verify_recover>(publicKey, RSA_PKCS1_PADDING, buffer, retVal);
}

result_t crypto_base::publicDecrypt(KeyObject_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        EVP_PKEY_verify_recover_init,
        EVP_PKEY_verify_recover>(publicKey, RSA_PKCS1_PADDING, buffer, retVal);
}

result_t crypto_base::publicDecrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPublicKey,
        EVP_PKEY_verify_recover_init,
        EVP_PKEY_verify_recover,
        false>(key, RSA_PKCS1_PADDING, buffer, retVal);
}

result_t crypto_base::publicEncrypt(Buffer_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPublicKey,
        EVP_PKEY_encrypt_init,
        EVP_PKEY_encrypt>(publicKey, RSA_PKCS1_OAEP_PADDING, buffer, retVal);
}

result_t crypto_base::publicEncrypt(KeyObject_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        EVP_PKEY_encrypt_init,
        EVP_PKEY_encrypt>(publicKey, RSA_PKCS1_OAEP_PADDING, buffer, retVal);
}

result_t crypto_base::publicEncrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal)
{
    return PKEY_cipher<
        crypto_base::createPublicKey,
        EVP_PKEY_encrypt_init,
        EVP_PKEY_encrypt,
        true>(key, RSA_PKCS1_OAEP_PADDING, buffer, retVal);
}

}
