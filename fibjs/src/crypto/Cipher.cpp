/*
 * Cipher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"
#include <string.h>

namespace fibjs {

#define SIZE_COUNT 4
#define PROVIDER_COUNT 7
#define MODE_COUNT 10

static const char* s_modes[MODE_COUNT] = {
    "", "-ECB", "-CBC", "-CFB64", "-CFB128", "-OFB", "-CTR", "-GCM", "", "-CCM"
};

static struct _cipher_size {
    const char* name;
    size_t size;
    const mbedtls_cipher_info_t* cis[MODE_COUNT];
} s_sizes[][SIZE_COUNT] = {
    { { "AES-128", 0, {} },
        { "AES-192", 0, {} },
        { "AES-256", 0, {} } },
    { { "CAMELLIA-128", 0, {} },
        { "CAMELLIA-192", 0, {} },
        { "CAMELLIA-256", 0, {} } },
    { { "DES", 0, {} } },
    { { "DES-EDE", 0, {} } },
    { { "DES-EDE3", 0, {} } },
    { { "BLOWFISH", 0, {} } },
    { { "ARC4-40", 0, {} },
        { "ARC4-56", 0, {} },
        { "ARC4-64", 0, {} },
        { "ARC4-128", 0, {} } }
};

void init_cipher()
{
    int32_t i, j, k;

    for (i = 0; i < PROVIDER_COUNT; i++)
        for (j = 0; j < SIZE_COUNT; j++)
            if (s_sizes[i][j].name)
                for (k = 1; k < MODE_COUNT; k++) {
                    exlib::string name = s_sizes[i][j].name;

                    name.append(s_modes[k]);
                    s_sizes[i][j].cis[k] = mbedtls_cipher_info_from_string(name.c_str());
                    if (s_sizes[i][j].cis[k])
                        s_sizes[i][j].size = s_sizes[i][j].cis[k]->key_bitlen;
                }
}

result_t Cipher_base::_new(int32_t provider, int32_t mode, Buffer_base* key,
    Buffer_base* iv, obj_ptr<Cipher_base>& retVal,
    v8::Local<v8::Object> This)
{
    if (provider < crypto_base::_AES || provider > crypto_base::_ARC4)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid provider"));
    if (mode < crypto_base::_ECB || mode > crypto_base::_CCM)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid mode"));

    exlib::string strKey;
    const mbedtls_cipher_info_t* info = NULL;

    key->toString(strKey);
    size_t keylen = strKey.length();

    if (keylen == 0)
        return CHECK_ERROR(Runtime::setError("Cipher: Invalid key size"));

    if (keylen == 16 && provider == crypto_base::_DES_EDE3) {
        strKey.append(strKey.c_str(), 8);
        keylen = 24;
    }

    for (int32_t i = 0; i < SIZE_COUNT; i++)
        if (s_sizes[provider - crypto_base::_AES][i].size == keylen * 8) {
            info = s_sizes[provider - crypto_base::_AES][i].cis[mode];
            if (info == NULL)
                return CHECK_ERROR(Runtime::setError("Cipher: Invalid mode"));
            break;
        }

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
    return _new(provider, crypto_base::_STREAM, key, NULL, retVal);
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
        memset(&m_key[0], 0, m_key.length());

    if (m_iv.length())
        memset(&m_iv[0], 0, m_iv.length());

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

    if (m_iv.length() && mbedtls_cipher_set_iv(&m_ctx, (unsigned char*)m_iv.c_str(),
                             m_iv.length())) {
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
    exlib::string output;
    unsigned char buffer[1024];
    size_t olen, ilen, offset, block_size, data_size;

    data->toString(input);
    block_size = mbedtls_cipher_get_block_size(&m_ctx);
    data_size = input.length();

    for (offset = 0; offset < data_size; offset += block_size) {
        ilen = ((uint32_t)data_size - offset > block_size) ? block_size : (uint32_t)(data_size - offset);

        ret = mbedtls_cipher_update(&m_ctx, (unsigned char*)input.c_str() + offset,
            ilen, buffer, &olen);
        if (ret != 0) {
            reset();
            return CHECK_ERROR(_ssl::setError(ret));
        }

        output.append((const char*)buffer, olen);
    }

    ret = mbedtls_cipher_finish(&m_ctx, buffer, &olen);
    reset();

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.append((const char*)buffer, olen);
    retVal = new Buffer(output);

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
