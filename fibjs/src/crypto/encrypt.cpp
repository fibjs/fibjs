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
result_t PKEY_cipher(v8::Local<v8::Object> key, int padding, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal)
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

    hr = GetConfigValue(key, "padding", padding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    const EVP_MD* digest = nullptr;
    if (useo_aep) {
        exlib::string oaepHash = "sha1";
        hr = GetConfigValue(key, "oaepHash", oaepHash, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        digest = _evp_md_type(oaepHash.c_str());
        if (!digest)
            return Runtime::setError("Invalid oaepHash: '%s'.", oaepHash.c_str());
    }

    obj_ptr<Buffer_base> oaep_label;
    hr = GetConfigValue(key, "oaepLabel", v);
    if (hr == 0) {
        hr = GetArgumentValue(isolate, v, oaep_label, false);
        if (hr < 0)
            return hr;
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    return PKEY_cipher<EVP_PKEY_cipher_init, EVP_PKEY_cipher>(key__->pkey(), padding, digest, oaep_label, buffer, retVal);
}

// The string buffer of the options-key form decodes with the
// `encoding` option of the key object (default utf8) and forwards.
static result_t optionsStringToBuffer(v8::Local<v8::Object> key, exlib::string str, obj_ptr<Buffer_base>& retVal)
{
    exlib::string encoding = "utf8";

    result_t hr = GetConfigValue(key, "encoding", encoding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    return Buffer_base::from(str, encoding, retVal);
}

// the key factories of the IDL take a union (plans/idl-union-types-2026-10-02.md):
// the PKEY_cipher templates key on a Buffer, so these adapt them
static result_t pkey_create_private_key(Buffer_base* key, obj_ptr<KeyObject_base>& retVal)
{
    return crypto_base::createPrivateKey(crypto_base::Union_createPrivateKey_key(obj_ptr<Buffer_base>(key)), retVal);
}

static result_t pkey_create_private_key(v8::Local<v8::Object> key, obj_ptr<KeyObject_base>& retVal)
{
    return crypto_base::createPrivateKey(crypto_base::Union_createPrivateKey_key(key), retVal);
}

static result_t pkey_create_public_key(Buffer_base* key, obj_ptr<KeyObject_base>& retVal)
{
    return crypto_base::createPublicKey(crypto_base::Union_createPublicKey_key(obj_ptr<Buffer_base>(key)), retVal);
}

static result_t pkey_create_public_key(v8::Local<v8::Object> key, obj_ptr<KeyObject_base>& retVal)
{
    return crypto_base::createPublicKey(crypto_base::Union_createPublicKey_key(key), retVal);
}

result_t crypto_base::privateDecrypt(Union_privateDecrypt_privateKey privateKey, Union_privateDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    bool bOptions = std::holds_alternative<v8::Local<v8::Object>>(privateKey);

    // a string buffer is declared together with the options object only: it is
    // decoded with the `encoding` option, which a plain key cannot carry
    if (std::holds_alternative<exlib::string>(buffer) && !bOptions)
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<exlib::string>(buffer)) {
        result_t hr = optionsStringToBuffer(std::get<v8::Local<v8::Object>>(privateKey),
            std::get<exlib::string>(buffer), buf);
        if (hr < 0)
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(buffer);

    if (bOptions)
        return PKEY_cipher<pkey_create_private_key, EVP_PKEY_decrypt_init, EVP_PKEY_decrypt, true>(
            std::get<v8::Local<v8::Object>>(privateKey), RSA_PKCS1_OAEP_PADDING, buf, retVal);

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_decrypt_init, EVP_PKEY_decrypt>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_OAEP_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr < 0)
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_private_key, EVP_PKEY_decrypt_init, EVP_PKEY_decrypt>(keyBuf, RSA_PKCS1_OAEP_PADDING, buf, retVal);
}
result_t crypto_base::privateEncrypt(Union_privateEncrypt_privateKey privateKey, Union_privateEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    bool bOptions = std::holds_alternative<v8::Local<v8::Object>>(privateKey);

    // a string buffer is declared together with the options object only: it is
    // decoded with the `encoding` option, which a plain key cannot carry
    if (std::holds_alternative<exlib::string>(buffer) && !bOptions)
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<exlib::string>(buffer)) {
        result_t hr = optionsStringToBuffer(std::get<v8::Local<v8::Object>>(privateKey),
            std::get<exlib::string>(buffer), buf);
        if (hr < 0)
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(buffer);

    if (bOptions)
        return PKEY_cipher<pkey_create_private_key, EVP_PKEY_sign_init, EVP_PKEY_sign, false>(
            std::get<v8::Local<v8::Object>>(privateKey), RSA_PKCS1_PADDING, buf, retVal);

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_sign_init, EVP_PKEY_sign>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr < 0)
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_private_key, EVP_PKEY_sign_init, EVP_PKEY_sign>(keyBuf, RSA_PKCS1_PADDING, buf, retVal);
}
result_t crypto_base::publicDecrypt(Union_publicDecrypt_publicKey privateKey, Union_publicDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    bool bOptions = std::holds_alternative<v8::Local<v8::Object>>(privateKey);

    // a string buffer is declared together with the options object only: it is
    // decoded with the `encoding` option, which a plain key cannot carry
    if (std::holds_alternative<exlib::string>(buffer) && !bOptions)
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<exlib::string>(buffer)) {
        result_t hr = optionsStringToBuffer(std::get<v8::Local<v8::Object>>(privateKey),
            std::get<exlib::string>(buffer), buf);
        if (hr < 0)
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(buffer);

    if (bOptions)
        return PKEY_cipher<pkey_create_public_key, EVP_PKEY_verify_recover_init, EVP_PKEY_verify_recover, false>(
            std::get<v8::Local<v8::Object>>(privateKey), RSA_PKCS1_PADDING, buf, retVal);

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_verify_recover_init, EVP_PKEY_verify_recover>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr < 0)
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_public_key, EVP_PKEY_verify_recover_init, EVP_PKEY_verify_recover>(keyBuf, RSA_PKCS1_PADDING, buf, retVal);
}
result_t crypto_base::publicEncrypt(Union_publicEncrypt_publicKey privateKey, Union_publicEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    bool bOptions = std::holds_alternative<v8::Local<v8::Object>>(privateKey);

    // a string buffer is declared together with the options object only: it is
    // decoded with the `encoding` option, which a plain key cannot carry
    if (std::holds_alternative<exlib::string>(buffer) && !bOptions)
        return CHECK_ERROR(CALL_E_TYPEMISMATCH);

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<exlib::string>(buffer)) {
        result_t hr = optionsStringToBuffer(std::get<v8::Local<v8::Object>>(privateKey),
            std::get<exlib::string>(buffer), buf);
        if (hr < 0)
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(buffer);

    if (bOptions)
        return PKEY_cipher<pkey_create_public_key, EVP_PKEY_encrypt_init, EVP_PKEY_encrypt, true>(
            std::get<v8::Local<v8::Object>>(privateKey), RSA_PKCS1_OAEP_PADDING, buf, retVal);

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_encrypt_init, EVP_PKEY_encrypt>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_OAEP_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr < 0)
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_public_key, EVP_PKEY_encrypt_init, EVP_PKEY_encrypt>(keyBuf, RSA_PKCS1_OAEP_PADDING, buf, retVal);
}
}
