/*
 * Cipher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"

namespace fibjs
{

#define SIZE_COUNT 4
#define PROVIDER_COUNT 7
#define MODE_COUNT 8

static const char *s_modes[MODE_COUNT] =
{
    "", "-ECB", "-CBC", "-CFB64", "-CFB128", "-OFB", "-CTR", "-GCM"
};

static struct _cipher_size
{
    const char *name;
    size_t size;
    const cipher_info_t *cis[MODE_COUNT];
} s_sizes[][SIZE_COUNT] =
{
    {
        { "AES-128" },
        { "AES-192" },
        { "AES-256" }
    },
    {
        { "CAMELLIA-128" },
        { "CAMELLIA-192" },
        { "CAMELLIA-256" }
    },
    {
        { "DES" }
    },
    {
        { "DES-EDE" }
    },
    {
        { "DES-EDE3" }
    },
    {
        { "BLOWFISH" }
    },
    {
        { "ARC4-40" },
        { "ARC4-56" },
        { "ARC4-64" },
        { "ARC4-128" }
    }
};

class _cipher_init
{
public:
    _cipher_init()
    {
        int32_t i, j, k;

        for (i = 0; i < PROVIDER_COUNT; i ++)
            for (j = 0; j < SIZE_COUNT; j ++)
                if (s_sizes[i][j].name)
                    for (k = 0; k < MODE_COUNT; k ++)
                    {
                        std::string name = s_sizes[i][j].name;

                        name.append(s_modes[k]);
                        s_sizes[i][j].cis[k] = cipher_info_from_string(name.c_str());
                        if (s_sizes[i][j].cis[k])
                            s_sizes[i][j].size = s_sizes[i][j].cis[k]->key_length;
                    }
    }
} s_cipher_init;


result_t Cipher_base::_new(int32_t provider, int32_t mode, Buffer_base *key,
                           Buffer_base *iv, obj_ptr<Cipher_base> &retVal)
{
    if (provider < crypto_base::_AES || provider > crypto_base::_ARC4)
        return CHECK_ERROR(Runtime::setError("Invalid provider"));
    if (mode < crypto_base::_STREAM || mode > crypto_base::_GCM)
        return CHECK_ERROR(Runtime::setError("Invalid mode"));

    std::string strKey;
    const cipher_info_t *info = NULL;

    key->toString(strKey);
    size_t keylen = strKey.length();

    if (keylen == 0)
        return CHECK_ERROR(Runtime::setError("Invalid key size"));

    if (keylen == 16 && provider == crypto_base::_DES_EDE3)
    {
        strKey.append(strKey.c_str(), 8);
        keylen = 24;
    }

    for (int i = 0; i < SIZE_COUNT; i ++)
        if (s_sizes[provider - crypto_base::_AES][i].size == keylen * 8)
        {
            info = s_sizes[provider - crypto_base::_AES][i].cis[mode - crypto_base::_STREAM];
            if (info == NULL)
                return CHECK_ERROR(Runtime::setError("Invalid mode"));
            break;
        }

    if (info == NULL)
        return CHECK_ERROR(Runtime::setError("Invalid key size"));

    obj_ptr<Cipher> ci = new Cipher(info);

    std::string striv;

    if (iv)
        iv->toString(striv);

    result_t hr = ci->init(strKey, striv);
    if (hr < 0)
        return hr;

    retVal = ci;

    return 0;
}

result_t Cipher_base::_new(int32_t provider, int32_t mode, Buffer_base *key,
                           obj_ptr<Cipher_base> &retVal)
{
    return _new(provider, mode, key, NULL, retVal);
}

result_t Cipher_base::_new(int32_t provider, Buffer_base *key,
                           obj_ptr<Cipher_base> &retVal)
{
    return _new(provider, crypto_base::_STREAM, key, NULL, retVal);
}

Cipher::Cipher(const cipher_info_t *info) : m_info(info)
{
    reset();
}

Cipher::~Cipher()
{
    if (m_key.length())
        memset(&m_key[0], 0, m_key.length());

    if (m_iv.length())
        memset(&m_iv[0], 0, m_iv.length());
}

void Cipher::reset()
{
    memset(&m_ctx, 0, sizeof(m_ctx));
    cipher_init_ctx(&m_ctx, m_info);

    if (m_iv.length())
        cipher_set_iv(&m_ctx, (unsigned char *)m_iv.c_str(), m_iv.length());
}

result_t Cipher::init(std::string &key, std::string &iv)
{
    m_key = key;
    m_iv = iv;

    if (m_iv.length() && cipher_set_iv(&m_ctx, (unsigned char *)m_iv.c_str(),
                                       m_iv.length()))
    {
        m_iv.resize(0);
        return CHECK_ERROR(Runtime::setError("Invalid iv size"));
    }

    return 0;
}

result_t Cipher::get_name(std::string &retVal)
{
    retVal = cipher_get_name(&m_ctx);
    return 0;
}

result_t Cipher::get_keySize(int32_t &retVal)
{
    retVal = cipher_get_key_size(&m_ctx);
    return 0;
}

result_t Cipher::get_ivSize(int32_t &retVal)
{
    retVal = cipher_get_iv_size(&m_ctx);
    return 0;
}

result_t Cipher::get_blockSize(int32_t &retVal)
{
    retVal = cipher_get_block_size(&m_ctx);
    return 0;
}

result_t Cipher::paddingMode(int32_t mode)
{
    int ret = cipher_set_padding_mode(&m_ctx, (cipher_padding_t)mode);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t Cipher::process(const operation_t operation, Buffer_base *data,
                         obj_ptr<Buffer_base> &retVal)
{
    int ret;

    ret = cipher_setkey(&m_ctx, (unsigned char *)m_key.c_str(), (int)m_key.length() * 8,
                        operation);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = cipher_reset(&m_ctx);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    std::string input;
    std::string output;
    unsigned char buffer[1024];
    size_t olen, ilen, offset, block_size, data_size;

    data->toString(input);
    block_size = cipher_get_block_size(&m_ctx);
    data_size = input.length();

    for (offset = 0; offset < data_size; offset += block_size)
    {
        ilen = ((unsigned int)data_size - offset > block_size) ?
               block_size : (unsigned int)(data_size - offset);

        ret = cipher_update(&m_ctx, (unsigned char *)input.c_str() + offset,
                            ilen, buffer, &olen);
        if (ret != 0)
        {
            reset();
            return CHECK_ERROR(_ssl::setError(ret));
        }

        output.append((const char *)buffer, olen);
    }

    ret = cipher_finish(&m_ctx, buffer, &olen);
    reset();

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.append((const char *)buffer, olen);
    retVal = new Buffer(output);

    return 0;
}

result_t Cipher::encrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                         exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process(POLARSSL_ENCRYPT, data, retVal);
}

result_t Cipher::decrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                         exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return process(POLARSSL_DECRYPT, data, retVal);
}

}
