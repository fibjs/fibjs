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
    } else if (format == "raw") {
        exlib::string namedCurve;
        hr = GetConfigValue(isolate, key, "namedCurve", namedCurve, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        int32_t key_type = EVP_PKEY_EC;

        if (namedCurve == "ed25519")
            key_type = EVP_PKEY_ED25519;
        else if (namedCurve == "ed448")
            key_type = EVP_PKEY_ED448;
        else if (namedCurve == "x25519")
            key_type = EVP_PKEY_X25519;
        else if (namedCurve == "x448")
            key_type = EVP_PKEY_X448;

        if (key_type == EVP_PKEY_EC) {
            int32_t cid = GetCurveFromName(namedCurve.c_str());
            if (cid == NID_undef)
                return Runtime::setError("Invalid curve name");

            ECPointer ec = EC_KEY_new_by_curve_name(cid);
            if (ec == nullptr)
                return Runtime::setError("Invalid namedCurve");

            BignumPointer d = BN_bin2bn(_key->data(), _key->length(), nullptr);
            const EC_GROUP* group = EC_KEY_get0_group(ec);
            ECPointPointer point = EC_POINT_new(group);

            if (EC_POINT_mul(group, point, d, nullptr, nullptr, nullptr) != 1)
                return openssl_error();

            if (EC_KEY_set_public_key(ec, point) != 1)
                return openssl_error();

            if (EC_KEY_set_private_key(ec, d) != 1)
                return openssl_error();

            d.release();

            m_pkey = EVP_PKEY_new();
            EVP_PKEY_set1_EC_KEY(m_pkey, ec);
        } else {
            m_pkey = EVP_PKEY_new_raw_private_key(key_type, nullptr, _key->data(), _key->length());
            if (m_pkey == nullptr)
                return openssl_error();
        }
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
    int ret;

    if (format == "pem") {
        if (type == "pkcs1") {
            if (EVP_PKEY_id(m_pkey) != EVP_PKEY_RSA)
                return Runtime::setError("pkcs1 only support RSA key");

            RsaPointer rsa = EVP_PKEY_get1_RSA(m_pkey);
            ret = PEM_write_bio_RSAPrivateKey(bio, rsa, cipher,
                cipher ? (unsigned char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
        } else if (type == "pkcs8") {
            ret = PEM_write_bio_PKCS8PrivateKey(bio, m_pkey, cipher,
                cipher ? (char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
        } else if (type == "sec1") {
            int nid = EVP_PKEY_id(m_pkey);
            if (nid != EVP_PKEY_EC && nid != EVP_PKEY_SM2)
                return Runtime::setError("spki only support EC and SM2 key");

            ECKeyPointer ec = EVP_PKEY_get1_EC_KEY(m_pkey);
            ret = PEM_write_bio_ECPrivateKey(bio, ec, cipher,
                cipher ? (unsigned char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
        } else
            return Runtime::setError("Invalid type");

        if (ret != 1)
            return openssl_error();

        BUF_MEM* bptr;
        BIO_get_mem_ptr(bio, &bptr);

        retVal = isolate->NewString(bptr->data, bptr->length);
    } else if (format == "der") {
        if (type == "pkcs1") {
            if (EVP_PKEY_id(m_pkey) != EVP_PKEY_RSA)
                return Runtime::setError("pkcs1 only support RSA key");

            RsaPointer rsa = EVP_PKEY_get1_RSA(m_pkey);
            ret = i2d_RSAPrivateKey_bio(bio, rsa);
        } else if (type == "pkcs8") {
            ret = i2d_PKCS8PrivateKey_bio(bio, m_pkey, cipher,
                cipher ? (char*)pass_buf->data() : nullptr,
                cipher ? pass_buf->length() : 0, nullptr, nullptr);
        } else if (type == "sec1") {
            int nid = EVP_PKEY_id(m_pkey);
            if (nid != EVP_PKEY_EC && nid != EVP_PKEY_SM2)
                return Runtime::setError("spki only support EC and SM2 key");

            ECKeyPointer ec = EVP_PKEY_get1_EC_KEY(m_pkey);
            ret = i2d_ECPrivateKey_bio(bio, ec);

        } else
            return Runtime::setError("Invalid type");

        if (ret != 1)
            return openssl_error();

        BUF_MEM* bptr;
        BIO_get_mem_ptr(bio, &bptr);

        obj_ptr<Buffer_base> buf = new Buffer((const unsigned char*)bptr->data, bptr->length);
        retVal = buf->wrap(isolate);
    } else if (format == "raw") {
        int nid = EVP_PKEY_id(m_pkey);
        if (nid == EVP_PKEY_EC || nid == EVP_PKEY_SM2) {
            const EC_KEY* ec = EVP_PKEY_get0_EC_KEY(m_pkey);
            const EC_GROUP* group = EC_KEY_get0_group(ec);

            int degree_bits = EC_GROUP_get_degree(group);
            int degree_bytes = (degree_bits / CHAR_BIT) + (7 + (degree_bits % CHAR_BIT)) / 8;

            const BIGNUM* d = EC_KEY_get0_private_key(ec);
            obj_ptr<Buffer> buf = new Buffer(nullptr, degree_bytes);
            BN_bn2binpad(d, buf->data(), degree_bytes);

            retVal = buf->wrap(isolate);
        } else if (nid == EVP_PKEY_ED25519 || nid == EVP_PKEY_ED448 || nid == EVP_PKEY_X25519 || nid == EVP_PKEY_X448) {
            size_t len = 0;
            EVP_PKEY_get_raw_private_key(m_pkey, nullptr, &len);

            obj_ptr<Buffer> buf = new Buffer(nullptr, len);
            EVP_PKEY_get_raw_private_key(m_pkey, buf->data(), &len);

            retVal = buf->wrap(isolate);
        } else
            return Runtime::setError("only support EC, SM2, ED25519, ED448, X25519, X448 key");
    } else
        return Runtime::setError("Invalid format");

    return 0;
}

}
