/*
 * KeyObject_pub.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#include "encoding.h"
#include "ifs/crypto.h"
#include "KeyObject.h"
#include "Buffer.h"

namespace fibjs {

result_t crypto_base::createPublicKey(Buffer_base* key, obj_ptr<KeyObject_base>& retVal)
{
    Buffer* key_buf = Buffer::Cast(key);
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->ParsePublicKeyPEM((const char*)key_buf->data(), key_buf->length());
    if (hr < 0)
        return hr;

    if (hr == 1)
        return CHECK_ERROR(Runtime::setError("Public key not recognized"));

    retVal = keyObj;
    return 0;
}

result_t crypto_base::createPublicKey(KeyObject_base* key, obj_ptr<KeyObject_base>& retVal)
{
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->createPublicKeyFromKeyObject(key);
    if (hr < 0)
        return hr;

    retVal = keyObj;
    return 0;
}

result_t crypto_base::createPublicKey(v8::Local<v8::Object> key, obj_ptr<KeyObject_base>& retVal)
{
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->createAsymmetricKey(key, KeyObject::kKeyTypePublic);
    if (hr < 0)
        return hr;

    retVal = keyObj;
    return 0;
}

result_t KeyObject::createPublicKeyFromKeyObject(KeyObject_base* key)
{
    KeyObject* keyObj = (KeyObject*)key;
    if (keyObj->type() == kKeyTypeSecret)
        return Runtime::setError("cannot create public key from a secret key.");

    return createPublicKeyFromPKey(keyObj->pkey());
}

result_t KeyObject::createPublicKeyFromPKey(EVP_PKEY* key)
{
    int type = EVP_PKEY_id(key);

    m_pkey = EVP_PKEY_new();

    if (type == EVP_PKEY_SM2) {
        const EC_KEY* ec = EVP_PKEY_get0_EC_KEY(key);

        ECKeyPointer ec1 = EC_KEY_new_by_curve_name(NID_sm2);
        EC_KEY_set_public_key(ec1, EC_KEY_get0_public_key(ec));

        EVP_PKEY_set1_EC_KEY(m_pkey, ec1);
    } else if (!evp_keymgmt_util_copy(m_pkey, key,
                   OSSL_KEYMGMT_SELECT_PUBLIC_KEY | OSSL_KEYMGMT_SELECT_ALL_PARAMETERS))
        return openssl_error();

    m_keyType = kKeyTypePublic;

    return 0;
}

result_t KeyObject::TryParsePublicKey(const BIOPointer& bp, const char* name, const std::function<EVP_PKEY*(const unsigned char** p, long l)>& parse)
{
    unsigned char* der_data;
    long der_len;

    if (PEM_bytes_read_bio(&der_data, &der_len, nullptr, name, bp, nullptr, nullptr) != 1)
        return 1;

    const unsigned char* p = der_data;
    m_pkey = parse(&p, der_len);
    OPENSSL_clear_free(der_data, der_len);

    if (m_pkey == nullptr)
        return openssl_error();

    m_keyType = kKeyTypePublic;

    return 0;
}

result_t KeyObject::ParsePublicKeyPEM(const char* key_pem, int key_pem_len)
{
    BIOPointer bp(BIO_new_mem_buf(const_cast<char*>(key_pem), key_pem_len));
    result_t hr;

    hr = TryParsePublicKey(bp, "PUBLIC KEY", [](const unsigned char** p, long l) {
        return d2i_PUBKEY(nullptr, p, l);
    });
    if (hr != 1)
        return hr;

    BIO_reset(bp);
    hr = TryParsePublicKey(bp, "RSA PUBLIC KEY", [](const unsigned char** p, long l) {
        return d2i_PublicKey(EVP_PKEY_RSA, nullptr, p, l);
    });
    if (hr != 1)
        return hr;

    BIO_reset(bp);
    hr = TryParsePublicKey(bp, "CERTIFICATE", [](const unsigned char** p, long l) {
        X509Pointer x509(d2i_X509(nullptr, p, l));
        return x509 ? X509_get_pubkey(x509) : nullptr;
    });
    if (hr != 1)
        return hr;

    BIO_reset(bp);
    m_pkey = PEM_read_bio_PrivateKey(
        bp, nullptr, [](char*, int, int, void*) {
            return 0;
        },
        nullptr);
    if (!m_pkey)
        return 1;

    m_keyType = kKeyTypePublic;

    return 0;
}

result_t KeyObject::ParsePublicKey(v8::Local<v8::Object> key)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string format = "pem";
    GetConfigValue(isolate, key, "format", format, true);

    obj_ptr<Buffer> _key;
    hr = GetKeyBuffer(isolate, key, _key);
    if (hr < 0)
        return hr;

    if (format == "pem") {
        hr = ParsePublicKeyPEM((const char*)_key->data(), _key->length());
        if (hr <= 0)
            return hr;

        hr = ParsePrivateKey(key);
        if (hr < 0)
            return hr;

        m_keyType = kKeyTypePublic;
        return 0;
    } else if (format == "der") {
        exlib::string type;
        hr = GetConfigValue(isolate, key, "type", type, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        const unsigned char* p = (unsigned char*)_key->data();
        if (type == "pkcs1")
            m_pkey = d2i_PublicKey(EVP_PKEY_RSA, nullptr, &p, _key->length());
        else if (type == "spki")
            m_pkey = d2i_PUBKEY(nullptr, &p, _key->length());
        else
            return Runtime::setError("Invalid type");
    } else
        return Runtime::setError("Invalid format");

    if (m_pkey == nullptr)
        return openssl_error();

    m_keyType = kKeyTypePublic;

    return 0;
}

result_t KeyObject::ExportPublicKey(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string type = "spki";
    hr = GetConfigValue(isolate, options, "type", type, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string format = "pem";
    GetConfigValue(isolate, options, "format", format, true);

    BIOPointer bio(BIO_new(BIO_s_mem()));
    bool is_pem = false;
    int ret;

    if (format == "pem") {
        is_pem = true;

        if (type == "pkcs1") {
            if (EVP_PKEY_id(m_pkey) != EVP_PKEY_RSA)
                return Runtime::setError("pkcs1 only support RSA key");

            RsaPointer rsa = EVP_PKEY_get1_RSA(m_pkey);
            ret = PEM_write_bio_RSAPublicKey(bio, rsa);
        } else if (type == "spki") {
            ret = PEM_write_bio_PUBKEY(bio, m_pkey);
        } else
            return Runtime::setError("Invalid type");
    } else if (format == "der") {
        if (type == "pkcs1") {
            if (EVP_PKEY_id(m_pkey) != EVP_PKEY_RSA)
                return Runtime::setError("pkcs1 only support RSA key");

            RsaPointer rsa = EVP_PKEY_get1_RSA(m_pkey);
            ret = i2d_RSAPublicKey_bio(bio, rsa);
        } else if (type == "spki") {
            ret = i2d_PUBKEY_bio(bio, m_pkey);
        } else
            return Runtime::setError("Invalid type");
    } else
        return Runtime::setError("Invalid format");

    if (ret != 1)
        return openssl_error();

    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);

    if (is_pem) {
        retVal = isolate->NewString(bptr->data, bptr->length);
    } else {
        obj_ptr<Buffer_base> buf = new Buffer((const unsigned char*)bptr->data, bptr->length);
        retVal = buf->wrap(isolate);
    }

    return 0;
}

}
