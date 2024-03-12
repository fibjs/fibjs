/*
 * KeyObject_priv.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#include "encoding.h"
#include "ifs/crypto.h"
#include "KeyObject.h"
#include "Buffer.h"

namespace fibjs {

result_t crypto_base::createPrivateKey(Buffer_base* key, obj_ptr<KeyObject_base>& retVal)
{
    Buffer* key_buf = Buffer::Cast(key);
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->ParsePrivateKeyPEM((const char*)key_buf->data(), key_buf->length(), NULL);
    if (hr < 0)
        return hr;

    retVal = keyObj;
    return 0;
}

result_t crypto_base::createPrivateKey(v8::Local<v8::Object> key, obj_ptr<KeyObject_base>& retVal)
{
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->createAsymmetricKey(key, KeyObject::kKeyTypePrivate);
    if (hr < 0)
        return hr;

    retVal = keyObj;
    return 0;
}

static int PasswordCallback(char* buf, int size, int rwflag, void* u)
{
    Buffer* passphrase = (Buffer*)u;

    if (passphrase != nullptr) {
        size_t buflen = static_cast<size_t>(size);
        size_t len = passphrase->length();
        if (buflen < len)
            return -1;
        memcpy(buf, passphrase->data(), len);
        return len;
    }

    return -1;
}

result_t KeyObject::ParsePrivateKeyPEM(const char* key_pem, int key_pem_len, Buffer* passphrase)
{
    BIOPointer bio(BIO_new_mem_buf(key_pem, key_pem_len));
    m_pkey = PEM_read_bio_PrivateKey(bio, nullptr, PasswordCallback, passphrase);
    if (m_pkey == nullptr)
        return CHECK_ERROR(Runtime::setError("Private key not recognized"));

    m_keyType = kKeyTypePrivate;

    return 0;
}

static bool IsASN1Sequence(const unsigned char* data, size_t size,
    size_t* data_offset, size_t* data_size)
{
    if (size < 2 || data[0] != 0x30)
        return false;

    if (data[1] & 0x80) {
        size_t n_bytes = data[1] & ~0x80;
        if (n_bytes + 2 > size || n_bytes > sizeof(size_t))
            return false;
        size_t length = 0;
        for (size_t i = 0; i < n_bytes; i++)
            length = (length << 8) | data[i + 2];
        *data_offset = 2 + n_bytes;
        *data_size = std::min(size - 2 - n_bytes, length);
    } else {
        *data_offset = 2;
        *data_size = std::min<size_t>(size - 2, data[1]);
    }

    return true;
}

static bool IsEncryptedPrivateKeyInfo(const unsigned char* data, size_t size)
{
    size_t offset, len;
    if (!IsASN1Sequence(data, size, &offset, &len))
        return false;
    return len >= 1 && data[offset] != 2;
}

result_t KeyObject::ParsePrivateKey(v8::Local<v8::Object> key)
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

    Buffer* passphrase = nullptr;
    obj_ptr<Buffer_base> _passphrase;
    hr = GetConfigValue(isolate, key, "passphrase", _passphrase);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (_passphrase != nullptr)
        passphrase = Buffer::Cast(_passphrase);

    if (format == "pem") {
        return ParsePrivateKeyPEM((const char*)_key->data(), _key->length(), passphrase);
    } else if (format == "der") {
        exlib::string type;
        hr = GetConfigValue(isolate, key, "type", type, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        const unsigned char* p = (unsigned char*)_key->data();
        if (type == "pkcs1")
            m_pkey = d2i_PrivateKey(EVP_PKEY_RSA, nullptr, &p, _key->length());
        else if (type == "pkcs8") {
            BIOPointer bio = BIO_new_mem_buf(p, _key->length());

            if (IsEncryptedPrivateKeyInfo(p, _key->length()))
                m_pkey = d2i_PKCS8PrivateKey_bio(bio, nullptr, PasswordCallback, passphrase);
            else {
                PKCS8Pointer p8inf = d2i_PKCS8_PRIV_KEY_INFO_bio(bio, nullptr);
                if (p8inf)
                    m_pkey = EVP_PKCS82PKEY(p8inf);
            }
        } else if (type == "sec1")
            m_pkey = d2i_PrivateKey(EVP_PKEY_EC, nullptr, &p, _key->length());
        else
            return Runtime::setError("Invalid type");
    } else
        return Runtime::setError("Invalid format");

    if (m_pkey == nullptr)
        return openssl_error();

    m_keyType = kKeyTypePrivate;

    return 0;
}

result_t KeyObject::ExportPrivateKey(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string type = "pkcs8";
    hr = GetConfigValue(isolate, options, "type", type, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string format = "pem";
    GetConfigValue(isolate, options, "format", format, true);

    exlib::string name;
    hr = GetConfigValue(isolate, options, "cipher", name, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    const EVP_CIPHER* cipher = nullptr;
    if (!name.empty()) {
        cipher = EVP_get_cipherbyname(name.c_str());
        if (cipher == nullptr)
            return openssl_error();
    }

    Buffer* pass_buf = NULL;
    obj_ptr<Buffer_base> passphrase;
    hr = GetConfigValue(isolate, options, "passphrase", passphrase);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (passphrase != nullptr) {
        if (cipher == nullptr)
            return Runtime::setError("cipher is required when passphrase is set");
        pass_buf = Buffer::Cast(passphrase);
    }

    BIOPointer bio(BIO_new(BIO_s_mem()));
    bool id_pem = false;
    int ret;

    if (type == "pkcs1") {
        if (EVP_PKEY_id(m_pkey) != EVP_PKEY_RSA)
            return Runtime::setError("pkcs1 only support RSA key");

        RsaPointer rsa = EVP_PKEY_get1_RSA(m_pkey);
        if (format == "pem") {
            ret = PEM_write_bio_RSAPrivateKey(bio, rsa, cipher,
                cipher ? (unsigned char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
            id_pem = true;
        } else if (format == "der") {
            ret = i2d_RSAPrivateKey_bio(bio, rsa);
        } else
            return Runtime::setError("Invalid format");
    } else if (type == "pkcs8") {
        if (format == "pem") {
            ret = PEM_write_bio_PKCS8PrivateKey(bio, m_pkey, cipher,
                cipher ? (char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
            id_pem = true;
        } else if (format == "der") {
            ret = i2d_PKCS8PrivateKey_bio(bio, m_pkey, cipher,
                cipher ? (char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
        } else
            return Runtime::setError("Invalid format");
    } else if (type == "sec1") {
        int nid = EVP_PKEY_id(m_pkey);
        if (nid != EVP_PKEY_EC && nid != EVP_PKEY_SM2)
            return Runtime::setError("spki only support EC and SM2 key");

        ECKeyPointer ec = EVP_PKEY_get1_EC_KEY(m_pkey);
        if (format == "pem") {
            ret = PEM_write_bio_ECPrivateKey(bio, ec, cipher,
                cipher ? (unsigned char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
            id_pem = true;
        } else if (format == "der") {
            ret = i2d_ECPrivateKey_bio(bio, ec);
        } else
            return Runtime::setError("Invalid format");
    } else
        return Runtime::setError("Invalid type");

    if (ret != 1)
        return openssl_error();

    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);

    if (id_pem) {
        retVal = isolate->NewString(bptr->data, bptr->length);
    } else {
        obj_ptr<Buffer_base> buf = new Buffer((const unsigned char*)bptr->data, bptr->length);
        retVal = buf->wrap(isolate);
    }

    return 0;
}

}
