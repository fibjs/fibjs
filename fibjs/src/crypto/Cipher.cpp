/*
 * Cipher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/crypto.h"
#include "crypto.h"
#include "StringBuffer.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"
#include "encoding.h"
#include <string.h>

namespace fibjs {

#define SIZE_COUNT 4

static const char* s_modes[] = {
    "", "-ECB", "-CBC", "-CFB64", "-CFB128", "-OFB", "-CTR", "-GCM", "", "-CCM", "-XTS", "-POLY1305"
};

#define MODE_COUNT ARRAYSIZE(s_modes)

static struct _cipher_size {
    const char* name;
    const mbedtls_cipher_info_t* cis[MODE_COUNT];
} s_sizes[][SIZE_COUNT] = {
    { { "AES-128", {} },
        { "AES-192", {} },
        { "AES-256", {} } },
    { { "DES", {} } },
    { { "DES-EDE3", {} } },
    { { "CAMELLIA-128", {} },
        { "CAMELLIA-192", {} },
        { "CAMELLIA-256", {} } },
    { { "ARIA-128", {} },
        { "ARIA-192", {} },
        { "ARIA-256", {} } },
    { { "CHACHA20", {} } },
    { { "SM4", {} } }
};

#define PROVIDER_COUNT ARRAYSIZE(s_sizes)

obj_ptr<NArray> g_ciphers;
class cipher_initer {
public:
    cipher_initer()
    {
        g_ciphers = new NArray();

        EVP_CIPHER_do_all_sorted([](const EVP_CIPHER* ciph, const char* from,
                                     const char* to, void* x) {
            if (from)
                g_ciphers->append(from);
        },
            NULL);

        int32_t i, j, k;

        for (i = 0; i < PROVIDER_COUNT; i++)
            for (j = 0; j < SIZE_COUNT; j++)
                if (s_sizes[i][j].name)
                    for (k = 1; k < MODE_COUNT; k++) {
                        exlib::string name = s_sizes[i][j].name;

                        name.append(s_modes[k]);
                        s_sizes[i][j].cis[k] = mbedtls_cipher_info_from_string(name.c_str());
                    }
    }
} s_cipher_initer;

result_t Cipher_base::_new(int32_t provider, int32_t mode, Buffer_base* key,
    Buffer_base* iv, obj_ptr<Cipher_base>& retVal, v8::Local<v8::Object> This)
{
    if (provider < crypto_base::C_AES || provider > crypto_base::C_SM4)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid provider"));
    if (mode < crypto_base::C_ECB || mode > crypto_base::C_POLY1305)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid mode"));

    exlib::string strKey;
    const mbedtls_cipher_info_t* info = NULL;
    bool bFoundMode = false;

    key->toString(strKey);
    size_t keylen = strKey.length();

    if (keylen == 0)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid key size"));

    if (keylen == 16 && provider == crypto_base::C_DES_EDE3) {
        strKey.append(strKey.c_str(), 8);
        keylen = 24;
    }

    for (int32_t i = 0; i < SIZE_COUNT; i++) {
        const mbedtls_cipher_info_t* mod_info = s_sizes[provider - crypto_base::C_AES][i].cis[mode];
        if (mod_info) {
            bFoundMode = true;
            if (mod_info->key_bitlen == keylen * 8) {
                info = mod_info;
                break;
            }
        }
    }

    if (!bFoundMode)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid mode"));

    if (info == NULL)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid key size"));

    obj_ptr<Cipher> ci = new Cipher(info);

    exlib::string striv;

    if (iv)
        iv->toString(striv);

    result_t hr = ci->init(strKey, striv);
    if (hr < 0)
        return hr;

    retVal = ci;

    return 0;
}

result_t Cipher_base::_new(int32_t provider, int32_t mode, Buffer_base* key,
    obj_ptr<Cipher_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(provider, mode, key, NULL, retVal);
}

result_t Cipher_base::_new(int32_t provider, Buffer_base* key,
    obj_ptr<Cipher_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(provider, crypto_base::C_STREAM, key, NULL, retVal);
}

Cipher::Cipher(const mbedtls_cipher_info_t* info)
    : m_info(info)
{
    mbedtls_cipher_setup(&m_ctx, m_info);

    if (m_iv.length())
        mbedtls_cipher_set_iv(&m_ctx, (unsigned char*)m_iv.c_str(), m_iv.length());
}

Cipher::~Cipher()
{
    if (m_key.length())
        memset(m_key.data(), 0, m_key.length());

    if (m_iv.length())
        memset(m_iv.data(), 0, m_iv.length());

    mbedtls_cipher_free(&m_ctx);
}

void Cipher::reset()
{
    mbedtls_cipher_free(&m_ctx);
    mbedtls_cipher_setup(&m_ctx, m_info);

    if (m_iv.length())
        mbedtls_cipher_set_iv(&m_ctx, (unsigned char*)m_iv.c_str(), m_iv.length());
}

result_t Cipher::init(exlib::string& key, exlib::string& iv)
{
    m_key = key;
    m_iv = iv;

    if (m_iv.length() && mbedtls_cipher_set_iv(&m_ctx, (unsigned char*)m_iv.c_str(), m_iv.length())) {
        m_iv.resize(0);
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid iv size"));
    }

    return 0;
}

result_t Cipher::get_name(exlib::string& retVal)
{
    retVal = mbedtls_cipher_get_name(&m_ctx);
    return 0;
}

result_t Cipher::get_keySize(int32_t& retVal)
{
    retVal = mbedtls_cipher_get_key_bitlen(&m_ctx);
    return 0;
}

result_t Cipher::get_ivSize(int32_t& retVal)
{
    retVal = mbedtls_cipher_get_iv_size(&m_ctx);
    return 0;
}

result_t Cipher::get_blockSize(int32_t& retVal)
{
    retVal = mbedtls_cipher_get_block_size(&m_ctx);
    return 0;
}

result_t Cipher::paddingMode(int32_t mode)
{
    int32_t ret = mbedtls_cipher_set_padding_mode(&m_ctx, (mbedtls_cipher_padding_t)mode);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t Cipher::process(const mbedtls_operation_t operation, Buffer_base* data,
    obj_ptr<Buffer_base>& retVal)
{
    int32_t ret;

    ret = mbedtls_cipher_setkey(&m_ctx, (unsigned char*)m_key.c_str(), (int32_t)m_key.length() * 8,
        operation);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_cipher_reset(&m_ctx);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    StringBuffer output;
    unsigned char buffer[1024];
    size_t olen, ilen, offset, block_size, data_size;

    Buffer* data_buf = Buffer::Cast(data);
    data_size = data_buf->length();

    block_size = mbedtls_cipher_get_block_size(&m_ctx);
    if (block_size == 1)
        block_size = sizeof(buffer);

    for (offset = 0; offset < data_size; offset += block_size) {
        ilen = ((uint32_t)(data_size - offset) > block_size) ? block_size : (uint32_t)(data_size - offset);

        ret = mbedtls_cipher_update(&m_ctx, data_buf->data() + offset,
            ilen, buffer, &olen);
        if (ret != 0) {
            reset();
            return CHECK_ERROR(_ssl::setError(ret));
        }

        output.append((const char*)buffer, (int32_t)olen);
    }

    ret = mbedtls_cipher_finish(&m_ctx, buffer, &olen);
    reset();

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.append((const char*)buffer, (int32_t)olen);

    exlib::string str = output.str();
    retVal = new Buffer(str.c_str(), str.length());

    return 0;
}

result_t Cipher::encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync()) {
        if (Buffer::Cast(data)->length() > 256)
            return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return process(MBEDTLS_ENCRYPT, data, retVal);
}

result_t Cipher::decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync()) {
        if (Buffer::Cast(data)->length() > 256)
            return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return process(MBEDTLS_DECRYPT, data, retVal);
}

result_t Cipher::setAuthTag(Buffer_base* buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::setAuthTag(exlib::string buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::getAuthTag(obj_ptr<Buffer_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::setAAD(Buffer_base* buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::setAAD(exlib::string buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::setAutoPadding(bool autoPadding, obj_ptr<Cipher_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::update(Buffer_base* data, exlib::string inputEncoding, exlib::string outputEncoding,
    v8::Local<v8::Value>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::update(exlib::string data, exlib::string inputEncoding, exlib::string outputEncoding,
    v8::Local<v8::Value>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Cipher::final(exlib::string outputEncoding, v8::Local<v8::Value>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t crypto_base::getCiphers(v8::Local<v8::Array>& retVal)
{
    v8::Local<v8::Value> v;
    g_ciphers->valueOf(v);

    retVal = v8::Local<v8::Array>::Cast(v);

    return 0;
}

result_t crypto_base::createCipher(exlib::string algorithm, Buffer_base* key,
    v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    obj_ptr<CipherX> ci = new CipherX(CipherX::kCipher);
    result_t hr = ci->init(algorithm, key, options);
    if (hr < 0)
        return hr;

    retVal = ci;

    return 0;
}

result_t crypto_base::createCipheriv(exlib::string algorithm, Buffer_base* key, Buffer_base* iv,
    v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    obj_ptr<CipherX> ci = new CipherX(CipherX::kCipher);
    result_t hr = ci->initiv(algorithm, key, iv, options);
    if (hr < 0)
        return hr;

    retVal = ci;

    return 0;
}

result_t crypto_base::createDecipher(exlib::string algorithm, Buffer_base* key,
    v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    obj_ptr<CipherX> ci = new CipherX(CipherX::kDecipher);
    result_t hr = ci->init(algorithm, key, options);
    if (hr < 0)
        return hr;

    retVal = ci;

    return 0;
}

result_t crypto_base::createDecipheriv(exlib::string algorithm, Buffer_base* key, Buffer_base* iv,
    v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    obj_ptr<CipherX> ci = new CipherX(CipherX::kDecipher);
    result_t hr = ci->initiv(algorithm, key, iv, options);
    if (hr < 0)
        return hr;

    retVal = ci;

    return 0;
}

CipherX::~CipherX()
{
    if (m_ctx)
        EVP_CIPHER_CTX_free(m_ctx);
}

static bool IsSupportedAuthenticatedMode(const EVP_CIPHER* cipher)
{
    switch (EVP_CIPHER_mode(cipher)) {
    case EVP_CIPH_CCM_MODE:
    case EVP_CIPH_GCM_MODE:
    case EVP_CIPH_OCB_MODE:
        return true;
    case EVP_CIPH_STREAM_CIPHER:
        return EVP_CIPHER_nid(cipher) == NID_chacha20_poly1305;
    default:
        return false;
    }
}

static bool IsSupportedAuthenticatedMode(const EVP_CIPHER_CTX* ctx)
{
    const EVP_CIPHER* cipher = EVP_CIPHER_CTX_cipher(ctx);
    return IsSupportedAuthenticatedMode(cipher);
}

static bool IsValidGCMTagLength(unsigned int tag_len)
{
    return tag_len == 4 || tag_len == 8 || (tag_len >= 12 && tag_len <= 16);
}

bool CipherX::IsAuthenticatedMode() const
{
    return IsSupportedAuthenticatedMode(m_ctx);
}

bool CipherX::CheckCCMMessageLength(int message_len)
{
    if (message_len > max_message_size_)
        return false;

    return true;
}

bool CipherX::MaybePassAuthTagToOpenSSL()
{
    if (auth_tag_state_ == kAuthTagKnown) {
        if (!EVP_CIPHER_CTX_ctrl(m_ctx, EVP_CTRL_AEAD_SET_TAG, auth_tag_len_, (unsigned char*)auth_tag_))
            return false;
        auth_tag_state_ = kAuthTagPassedToOpenSSL;
    }
    return true;
}

result_t CipherX::InitAuthenticated(int iv_len, unsigned int auth_tag_len)
{
    if (!EVP_CIPHER_CTX_ctrl(m_ctx, EVP_CTRL_AEAD_SET_IVLEN, iv_len, nullptr))
        return CHECK_ERROR(openssl_error());

    const int mode = EVP_CIPHER_CTX_mode(m_ctx);

    if (mode == EVP_CIPH_GCM_MODE) {
        if (auth_tag_len != kNoAuthTagLength) {
            if (!IsValidGCMTagLength(auth_tag_len))
                return CHECK_ERROR(Runtime::setError("Cipher: Invalid authTagLength"));

            auth_tag_len_ = auth_tag_len;
        }
    } else {
        if (auth_tag_len == kNoAuthTagLength) {
            if (EVP_CIPHER_CTX_nid(m_ctx) == NID_chacha20_poly1305) {
                auth_tag_len = 16;
            } else
                return CHECK_ERROR(Runtime::setError("Cipher: authTagLength is required"));
        }

        if (mode == EVP_CIPH_CCM_MODE && kind_ == kDecipher && EVP_default_properties_is_fips_enabled(nullptr))
            return CHECK_ERROR(Runtime::setError("Cipher: CCM decryption not supported in FIPS mode"));

        if (!EVP_CIPHER_CTX_ctrl(m_ctx, EVP_CTRL_AEAD_SET_TAG, auth_tag_len, nullptr))
            return CHECK_ERROR(Runtime::setError("Cipher: Invalid authTagLength"));

        auth_tag_len_ = auth_tag_len;

        if (mode == EVP_CIPH_CCM_MODE) {
            max_message_size_ = INT_MAX;
            if (iv_len == 12)
                max_message_size_ = 16777215;
            if (iv_len == 13)
                max_message_size_ = 65535;
        }
    }

    return 0;
}

result_t CipherX::init(const exlib::string& name, Buffer_base* key, v8::Local<v8::Object> options)
{
    const EVP_CIPHER* cipher = EVP_get_cipherbyname(name.c_str());
    if (cipher == NULL)
        return CHECK_ERROR(Runtime::setError("Cipher: Unknown cipher"));

    unsigned char key1[EVP_MAX_KEY_LENGTH];
    unsigned char iv1[EVP_MAX_IV_LENGTH];

    Buffer* key_buf = Buffer::Cast(key);
    int key_len = EVP_BytesToKey(cipher, EVP_md5(), nullptr, key_buf->data(),
        key_buf->length(), 1, key1, iv1);

    return CommonInit(cipher, key1, key_len, iv1, EVP_CIPHER_iv_length(cipher), options);
}

result_t CipherX::initiv(const exlib::string& name, Buffer_base* key, Buffer_base* iv, v8::Local<v8::Object> options)
{
    const EVP_CIPHER* cipher = EVP_get_cipherbyname(name.c_str());
    if (cipher == NULL)
        return CHECK_ERROR(Runtime::setError("Cipher: Unknown cipher"));

    result_t hr;
    const int expected_iv_len = EVP_CIPHER_iv_length(cipher);
    const bool is_authenticated_mode = IsSupportedAuthenticatedMode(cipher);

    Buffer* iv_buf = Buffer::Cast(iv);
    const int iv_len = iv_buf ? iv_buf->length() : 0;
    const bool has_iv = iv_len > 0;

    if (!has_iv && expected_iv_len != 0)
        return CHECK_ERROR(Runtime::setError("Cipher: IV is required"));

    if (!is_authenticated_mode && has_iv && iv_len != expected_iv_len)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid iv size"));

    if (EVP_CIPHER_nid(cipher) == NID_chacha20_poly1305) {
        if (!has_iv)
            return CHECK_ERROR(Runtime::setError("Cipher: IV is required"));
        if (iv_len > 12)
            return CHECK_ERROR(Runtime::setError("Cipher: Invalid iv size"));
    }

    Buffer* key_buf = Buffer::Cast(key);
    return CommonInit(cipher, (const unsigned char*)key_buf->data(), key_buf->length(), (const unsigned char*)iv_buf->data(), iv_len, options);
}

result_t CipherX::CommonInit(const EVP_CIPHER* cipher, const unsigned char* key, unsigned int key_len,
    const unsigned char* iv, unsigned int iv_len, v8::Local<v8::Object> options)
{
    result_t hr;

    m_ctx = EVP_CIPHER_CTX_new();
    if (m_ctx == NULL)
        return CHECK_ERROR(Runtime::setError("Cipher: Out of memory"));

    const int mode = EVP_CIPHER_mode(cipher);
    if (mode == EVP_CIPH_WRAP_MODE)
        EVP_CIPHER_CTX_set_flags(m_ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);

    const bool encrypt = (kind_ == kCipher);
    if (EVP_CipherInit_ex(m_ctx, cipher, nullptr, nullptr, nullptr, encrypt) != 1)
        return CHECK_ERROR(openssl_error());

    if (IsSupportedAuthenticatedMode(cipher)) {
        int auth_tag_len = kNoAuthTagLength;

        hr = GetConfigValue(holder(), options, "authTagLength", auth_tag_len, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        hr = InitAuthenticated(iv_len, auth_tag_len);
        if (hr < 0)
            return hr;
    }

    if (!EVP_CIPHER_CTX_set_key_length(m_ctx, key_len))
        return CHECK_ERROR(openssl_error());

    if (EVP_CipherInit_ex(m_ctx, nullptr, nullptr, key, iv, encrypt) != 1)
        return CHECK_ERROR(openssl_error());

    return 0;
}

result_t CipherX::get_name(exlib::string& retVal)
{
    const EVP_CIPHER* cipher = EVP_CIPHER_CTX_cipher(m_ctx);
    retVal = EVP_CIPHER_name(cipher);
    return 0;
}

result_t CipherX::get_keySize(int32_t& retVal)
{
    const EVP_CIPHER* cipher = EVP_CIPHER_CTX_cipher(m_ctx);
    retVal = EVP_CIPHER_key_length(cipher);
    return 0;
}

result_t CipherX::get_ivSize(int32_t& retVal)
{
    const EVP_CIPHER* cipher = EVP_CIPHER_CTX_cipher(m_ctx);
    retVal = EVP_CIPHER_iv_length(cipher);
    return 0;
}

result_t CipherX::get_blockSize(int32_t& retVal)
{
    const EVP_CIPHER* cipher = EVP_CIPHER_CTX_cipher(m_ctx);
    retVal = EVP_CIPHER_block_size(cipher);
    return 0;
}

result_t CipherX::paddingMode(int32_t mode)
{
    if (EVP_CIPHER_CTX_set_padding(m_ctx, mode) != 1)
        return CHECK_ERROR(openssl_error());
    return 0;
}

result_t CipherX::encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}

result_t CipherX::decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return CALL_E_INVALID_CALL;
}

result_t CipherX::setAuthTag(const char* tag, int tag_len)
{
    if (!IsAuthenticatedMode() || kind_ != kDecipher || auth_tag_state_ != kAuthTagUnknown)
        return Runtime::setError("Cipher: Invalid setAuthTag");

    const int mode = EVP_CIPHER_CTX_mode(m_ctx);
    bool is_valid;
    if (mode == EVP_CIPH_GCM_MODE)
        is_valid = (auth_tag_len_ == kNoAuthTagLength || auth_tag_len_ == tag_len) && IsValidGCMTagLength(tag_len);
    else
        is_valid = auth_tag_len_ == tag_len;

    if (!is_valid)
        return Runtime::setError("Cipher: Invalid authTagLength");

    auth_tag_len_ = tag_len;
    auth_tag_state_ = kAuthTagKnown;

    memset(auth_tag_, 0, sizeof(auth_tag_));
    memcpy(auth_tag_, tag, tag_len);

    return 0;
}

result_t CipherX::setAuthTag(Buffer_base* buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal)
{
    Buffer* buf = Buffer::Cast(buffer);

    result_t hr = setAuthTag((const char*)buf->data(), buf->length());
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t CipherX::setAuthTag(exlib::string buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal)
{
    result_t hr = commonDecode(encoding, buffer, buffer);
    if (hr < 0)
        return hr;

    hr = setAuthTag(buffer.c_str(), buffer.length());
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t CipherX::getAuthTag(obj_ptr<Buffer_base>& retVal)
{
    if (auth_tag_state_ != kAuthTagFinalized || kind_ != kCipher || auth_tag_len_ == kNoAuthTagLength)
        return Runtime::setError("Cipher: Invalid authTag");

    retVal = new Buffer(auth_tag_, auth_tag_len_);

    return 0;
}

result_t CipherX::setAAD(const char* data, int data_len, v8::Local<v8::Object> options)
{
    if (auth_tag_state_ == kAuthTagFinalized || !IsAuthenticatedMode())
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid setAAD"));

    int outlen;
    const int mode = EVP_CIPHER_CTX_mode(m_ctx);

    if (mode == EVP_CIPH_CCM_MODE) {
        int plaintext_len = kNoAuthTagLength;
        result_t hr = GetConfigValue(holder(), options, "plaintextLength", plaintext_len, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        if (plaintext_len < 0)
            return CHECK_ERROR(Runtime::setError("Cipher: Invalid plaintextLength"));

        if (!CheckCCMMessageLength(plaintext_len))
            return CHECK_ERROR(Runtime::setError("Cipher: Invalid message length"));

        if (kind_ == kDecipher) {
            if (!MaybePassAuthTagToOpenSSL())
                return CHECK_ERROR(Runtime::setError("Cipher: Invalid authTag"));
        }

        if (!EVP_CipherUpdate(m_ctx, nullptr, &outlen, nullptr, plaintext_len))
            return CHECK_ERROR(openssl_error());
    }

    if (EVP_CipherUpdate(m_ctx, nullptr, &outlen, (const unsigned char*)data, data_len) != 1)
        return CHECK_ERROR(openssl_error());

    return 0;
}

result_t CipherX::setAAD(Buffer_base* buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    Buffer* buf = Buffer::Cast(buffer);
    result_t hr = setAAD((const char*)buf->data(), buf->length(), options);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t CipherX::setAAD(exlib::string buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal)
{
    exlib::string encoding = "utf8";
    result_t hr = GetConfigValue(holder(), options, "encoding", encoding, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = commonDecode(encoding, buffer, buffer);
    if (hr < 0)
        return hr;
    hr = setAAD(buffer.c_str(), buffer.length(), options);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t CipherX::setAutoPadding(bool autoPadding, obj_ptr<Cipher_base>& retVal)
{
    if (auth_tag_state_ == kAuthTagFinalized)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid setAutoPadding"));

    if (EVP_CIPHER_CTX_set_padding(m_ctx, autoPadding) != 1)
        return CHECK_ERROR(openssl_error());

    retVal = this;
    return 0;
}

result_t CipherX::get_result(Buffer* out, exlib::string outputEncoding, v8::Local<v8::Value>& retVal, bool final)
{
    if (outputEncoding == "buffer") {
        if (m_decoder)
            return CHECK_ERROR(Runtime::setError("Cipher: Inconsistent encoding"));

        retVal = out->wrap(holder());
        return 0;
    }

    result_t hr;
    exlib::string new_encoding = normalizeEncoding(outputEncoding);
    if (m_decoder) {
        exlib::string last_encoding;
        m_decoder->get_encoding(last_encoding);
        if (new_encoding != last_encoding)
            return CHECK_ERROR(Runtime::setError("Cipher: Inconsistent encoding"));
    } else {
        bool isValid = false;
        Buffer_base::isEncoding(new_encoding, isValid);
        if (!isValid)
            return CHECK_ERROR(Runtime::setError("CipherX: Unknown encoding: " + outputEncoding));
        m_decoder = new StringDecoder(new_encoding);
    }

    exlib::string str;
    if (final)
        hr = m_decoder->end(out, str);
    else
        hr = m_decoder->write(out, str);

    if (hr < 0)
        return hr;

    retVal = holder()->NewString(str);

    return 0;
}

result_t CipherX::update(const unsigned char* data, size_t len, exlib::string outputEncoding, v8::Local<v8::Value>& retVal)
{
    const int mode = EVP_CIPHER_CTX_mode(m_ctx);
    if (mode == EVP_CIPH_CCM_MODE && !CheckCCMMessageLength(len))
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid message length"));

    if (kind_ == kDecipher && IsAuthenticatedMode())
        if (!MaybePassAuthTagToOpenSSL())
            return CHECK_ERROR(Runtime::setError("Cipher: Invalid authTag"));

    const int block_size = EVP_CIPHER_CTX_block_size(m_ctx);
    int buf_len = len + block_size;

    if (kind_ == kCipher && mode == EVP_CIPH_WRAP_MODE
        && EVP_CipherUpdate(m_ctx, nullptr, &buf_len, reinterpret_cast<const unsigned char*>(data), len) != 1)
        return CHECK_ERROR(openssl_error());

    obj_ptr<Buffer> out = new Buffer(NULL, buf_len);
    if (EVP_CipherUpdate(m_ctx, out->data(), &buf_len, data, len) != 1) {
        if (kind_ != kDecipher || mode != EVP_CIPH_CCM_MODE)
            return Runtime::setError(openssl_error());
        pending_auth_failed_ = true;
    }

    out->resize(buf_len);
    return get_result(out, outputEncoding, retVal, false);
}

result_t CipherX::update(Buffer_base* data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal)
{
    Buffer* buf = Buffer::Cast(data);
    return update(buf->data(), buf->length(), outputEncoding, retVal);
}

result_t CipherX::update(exlib::string data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal)
{
    result_t hr = commonDecode(inputEncoding, data, data);
    if (hr < 0)
        return hr;

    return update((const unsigned char*)data.c_str(), data.length(), outputEncoding, retVal);
}

result_t CipherX::final(exlib::string outputEncoding, v8::Local<v8::Value>& retVal)
{
    const int mode = EVP_CIPHER_CTX_mode(m_ctx);

    if (kind_ == kDecipher && IsSupportedAuthenticatedMode(m_ctx))
        MaybePassAuthTagToOpenSSL();

    bool ok;
    obj_ptr<Buffer> out;
    if (kind_ == kDecipher && mode == EVP_CIPH_CCM_MODE) {
        ok = !pending_auth_failed_;
        out = new Buffer(NULL, 0);
    } else {
        int out_len = EVP_CIPHER_CTX_block_size(m_ctx);
        out = new Buffer(NULL, out_len);
        ok = EVP_CipherFinal_ex(m_ctx, out->data(), &out_len) == 1;
        if (out_len > 0)
            out->resize(out_len);
        else
            out = new Buffer(NULL, 0);

        if (ok && kind_ == kCipher && IsAuthenticatedMode()) {
            if (auth_tag_len_ == kNoAuthTagLength)
                auth_tag_len_ = sizeof(auth_tag_);

            ok = (1 == EVP_CIPHER_CTX_ctrl(m_ctx, EVP_CTRL_AEAD_GET_TAG, auth_tag_len_, (unsigned char*)auth_tag_));
        }
    }

    if (!ok)
        return CHECK_ERROR(Runtime::setError(openssl_error()));

    auth_tag_state_ = kAuthTagFinalized;

    return get_result(out, outputEncoding, retVal, true);
}

}
