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
    return result_t();
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
    if (hr.failed())
        return hr;

    return PKEY_cipher<EVP_PKEY_cipher_init, EVP_PKEY_cipher>(key_, padding, buffer, retVal);
}

// the key factories of the IDL take a union (plans/idl-union-types-2026-10-02.md):
// the PKEY_cipher templates key on a Buffer, so these adapt them
static result_t pkey_create_private_key(Buffer_base* key, obj_ptr<KeyObject_base>& retVal)
{
    return crypto_base::createPrivateKey(crypto_base::Union_createPrivateKey_key(obj_ptr<Buffer_base>(key)), retVal);
}

static result_t pkey_create_public_key(Buffer_base* key, obj_ptr<KeyObject_base>& retVal)
{
    return crypto_base::createPublicKey(crypto_base::Union_createPublicKey_key(obj_ptr<Buffer_base>(key)), retVal);
}

template <typename BufferUnion>
static result_t encrypt_buffer(BufferUnion& buffer, exlib::string encoding, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<exlib::string>(buffer))
        return Buffer_base::from(std::get<exlib::string>(buffer), encoding, retVal);

    retVal = std::get<obj_ptr<Buffer_base>>(buffer);
    return result_t();
}

static result_t encrypt_label(EncryptOptions* options, obj_ptr<Buffer_base>& retVal)
{
    if (!options || !options->oaepLabel.has_value())
        return result_t();

    auto& label = options->oaepLabel.value();
    if (std::holds_alternative<obj_ptr<Buffer_base>>(label)) {
        retVal = std::get<obj_ptr<Buffer_base>>(label);
        return result_t();
    }

    if (std::holds_alternative<std::shared_ptr<v8::BackingStore>>(label)) {
        auto store = std::get<std::shared_ptr<v8::BackingStore>>(label);
        retVal = new Buffer((const unsigned char*)store->Data(), store->ByteLength());
        return result_t();
    }

    return Buffer_base::from(std::get<exlib::string>(label), "utf8", retVal);
}

static result_t encrypt_digest(EncryptOptions* options, const EVP_MD*& digest)
{
    exlib::string oaepHash = options ? options->oaepHash.value_or("sha1") : "sha1";

    digest = _evp_md_type(oaepHash.c_str());
    if (!digest)
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("Invalid oaepHash: '%s'.", oaepHash.c_str());

    return result_t();
}

result_t crypto_base::privateDecrypt(EncryptOptions* options, Union_privateDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<KeyObject_base> key_;
    result_t hr = create_key_object(nullptr, *options, KeyObject::kKeyTypePrivate, key_);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer_base> buf;
    hr = encrypt_buffer(buffer, options->encoding.value_or("utf8"), buf);
    if (hr.failed())
        return hr;

    int padding = options->padding.value_or(RSA_PKCS1_OAEP_PADDING);

    const EVP_MD* digest;
    hr = encrypt_digest(options, digest);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer_base> label;
    hr = encrypt_label(options, label);
    if (hr.failed())
        return hr;

    return PKEY_cipher<EVP_PKEY_decrypt_init, EVP_PKEY_decrypt>(
        key_.As<KeyObject>()->pkey(), padding, digest, label, buf, retVal);
}

result_t crypto_base::privateDecrypt(Union_privateDecrypt_privateKey privateKey, Union_privateDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    // a string buffer is declared together with the options form only: it is
    // decoded with the `encoding` option, which a plain key cannot carry
    if (std::holds_alternative<exlib::string>(buffer))
        return result_t::type_mismatch();

    obj_ptr<Buffer_base> buf;
    result_t hr = encrypt_buffer(buffer, "utf8", buf);
    if (hr.failed())
        return hr;

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_decrypt_init, EVP_PKEY_decrypt>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_OAEP_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr.failed())
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_private_key, EVP_PKEY_decrypt_init, EVP_PKEY_decrypt>(keyBuf, RSA_PKCS1_OAEP_PADDING, buf, retVal);
}
result_t crypto_base::privateEncrypt(EncryptOptions* options, Union_privateEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<KeyObject_base> key_;
    result_t hr = create_key_object(nullptr, *options, KeyObject::kKeyTypePrivate, key_);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer_base> buf;
    hr = encrypt_buffer(buffer, options->encoding.value_or("utf8"), buf);
    if (hr.failed())
        return hr;

    int padding = options->padding.value_or(RSA_PKCS1_PADDING);

    return PKEY_cipher<EVP_PKEY_sign_init, EVP_PKEY_sign>(
        key_.As<KeyObject>()->pkey(), padding, nullptr, nullptr, buf, retVal);
}

result_t crypto_base::privateEncrypt(Union_privateEncrypt_privateKey privateKey, Union_privateEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<exlib::string>(buffer))
        return result_t::type_mismatch();

    obj_ptr<Buffer_base> buf;
    result_t hr = encrypt_buffer(buffer, "utf8", buf);
    if (hr.failed())
        return hr;

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_sign_init, EVP_PKEY_sign>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr.failed())
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_private_key, EVP_PKEY_sign_init, EVP_PKEY_sign>(keyBuf, RSA_PKCS1_PADDING, buf, retVal);
}
result_t crypto_base::publicDecrypt(EncryptOptions* options, Union_publicDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<KeyObject_base> key_;
    result_t hr = create_key_object(nullptr, *options, KeyObject::kKeyTypePublic, key_);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer_base> buf;
    hr = encrypt_buffer(buffer, options->encoding.value_or("utf8"), buf);
    if (hr.failed())
        return hr;

    int padding = options->padding.value_or(RSA_PKCS1_PADDING);

    return PKEY_cipher<EVP_PKEY_verify_recover_init, EVP_PKEY_verify_recover>(
        key_.As<KeyObject>()->pkey(), padding, nullptr, nullptr, buf, retVal);
}

result_t crypto_base::publicDecrypt(Union_publicDecrypt_publicKey privateKey, Union_publicDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<exlib::string>(buffer))
        return result_t::type_mismatch();

    obj_ptr<Buffer_base> buf;
    result_t hr = encrypt_buffer(buffer, "utf8", buf);
    if (hr.failed())
        return hr;

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_verify_recover_init, EVP_PKEY_verify_recover>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr.failed())
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_public_key, EVP_PKEY_verify_recover_init, EVP_PKEY_verify_recover>(keyBuf, RSA_PKCS1_PADDING, buf, retVal);
}
result_t crypto_base::publicEncrypt(EncryptOptions* options, Union_publicEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    obj_ptr<KeyObject_base> key_;
    result_t hr = create_key_object(nullptr, *options, KeyObject::kKeyTypePublic, key_);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer_base> buf;
    hr = encrypt_buffer(buffer, options->encoding.value_or("utf8"), buf);
    if (hr.failed())
        return hr;

    int padding = options->padding.value_or(RSA_PKCS1_OAEP_PADDING);

    const EVP_MD* digest;
    hr = encrypt_digest(options, digest);
    if (hr.failed())
        return hr;

    obj_ptr<Buffer_base> label;
    hr = encrypt_label(options, label);
    if (hr.failed())
        return hr;

    return PKEY_cipher<EVP_PKEY_encrypt_init, EVP_PKEY_encrypt>(
        key_.As<KeyObject>()->pkey(), padding, digest, label, buf, retVal);
}

result_t crypto_base::publicEncrypt(Union_publicEncrypt_publicKey privateKey, Union_publicEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal)
{
    if (std::holds_alternative<exlib::string>(buffer))
        return result_t::type_mismatch();

    obj_ptr<Buffer_base> buf;
    result_t hr = encrypt_buffer(buffer, "utf8", buf);
    if (hr.failed())
        return hr;

    if (std::holds_alternative<obj_ptr<KeyObject_base>>(privateKey))
        return PKEY_cipher<EVP_PKEY_encrypt_init, EVP_PKEY_encrypt>(
            std::get<obj_ptr<KeyObject_base>>(privateKey), RSA_PKCS1_OAEP_PADDING, buf, retVal);

    // a string key is the PEM text itself
    obj_ptr<Buffer_base> keyBuf;

    if (std::holds_alternative<exlib::string>(privateKey)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(privateKey), "utf8", keyBuf);
        if (hr.failed())
            return hr;
    } else
        keyBuf = std::get<obj_ptr<Buffer_base>>(privateKey);

    return PKEY_cipher<pkey_create_public_key, EVP_PKEY_encrypt_init, EVP_PKEY_encrypt>(keyBuf, RSA_PKCS1_OAEP_PADDING, buf, retVal);
}
}
