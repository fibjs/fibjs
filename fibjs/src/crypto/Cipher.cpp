/*
 * Cipher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "StringBuffer.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"
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
    { { "CAMELLIA-128", {} },
        { "CAMELLIA-192", {} },
        { "CAMELLIA-256", {} } },
    { { "DES", {} } },
    { { "DES-EDE", {} } },
    { { "DES-EDE3", {} } },
    { { "BLOWFISH", {} } },
    { { "ARC4-40", {} },
        { "ARC4-56", {} },
        { "ARC4-64", {} },
        { "ARC4-128", {} } },
    { { "ARIA-128", {} },
        { "ARIA-192", {} },
        { "ARIA-256", {} } },
    { { "CHACHA20", {} } },
    { { "SM4", {} } }
};

#define PROVIDER_COUNT ARRAYSIZE(s_sizes)

class cipher_initer {
public:
    cipher_initer()
    {
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
    Buffer_base* iv, obj_ptr<Cipher_base>& retVal,
    v8::Local<v8::Object> This)
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
        memset(m_key.c_buffer(), 0, m_key.length());

    if (m_iv.length())
        memset(m_iv.c_buffer(), 0, m_iv.length());

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

    exlib::string input;
    StringBuffer output;
    unsigned char buffer[1024];
    size_t olen, ilen, offset, block_size, data_size;

    data->toString(input);
    data_size = input.length();

    block_size = mbedtls_cipher_get_block_size(&m_ctx);
    if (block_size == 1)
        block_size = sizeof(buffer);

    for (offset = 0; offset < data_size; offset += block_size) {
        ilen = ((uint32_t)(data_size - offset) > block_size) ? block_size : (uint32_t)(data_size - offset);

        ret = mbedtls_cipher_update(&m_ctx, (unsigned char*)input.c_str() + offset,
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
    retVal = new Buffer(output.str());

    return 0;
}

result_t Cipher::encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync()) {
        exlib::string input;
        data->toString(input);
        if (input.length() > 256)
            return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return process(MBEDTLS_ENCRYPT, data, retVal);
}

result_t Cipher::decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync()) {
        exlib::string input;
        data->toString(input);
        if (input.length() > 256)
            return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return process(MBEDTLS_DECRYPT, data, retVal);
}
}
