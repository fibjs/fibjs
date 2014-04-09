/*
 * Cipher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "Cipher.h"
#include "polarssl/error.h"

namespace fibjs
{

#define SIZE_COUNT 3
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
    if (provider < _AES || provider > _ARC4)
        return Runtime::setError("Invalid provider");
    if (mode < _STREAM || mode > _GCM)
        return Runtime::setError("Invalid mode");

    std::string strKey;
    const cipher_info_t *ci = NULL;

    key->toString(strKey);
    size_t keylen = strKey.length();

    if (keylen == 0)
        return Runtime::setError("Invalid key size");

    for (int i = 0; i < 3; i ++)
        if (s_sizes[provider - _AES][i].size == keylen * 8)
        {
            ci = s_sizes[provider - _AES][i].cis[mode - _STREAM];
            if (ci == NULL)
                return Runtime::setError("Invalid mode");
            break;
        }

    if (ci == NULL)
        return Runtime::setError("Invalid key size");

    retVal = new Cipher(ci);
    return 0;
}

result_t Cipher_base::_new(int32_t provider, Buffer_base *key,
                           Buffer_base *iv, obj_ptr<Cipher_base> &retVal)
{
    return _new(provider, _STREAM, key, iv, retVal);
}

result_t Cipher_base::_new(int32_t provider, int32_t mode, Buffer_base *key,
                           obj_ptr<Cipher_base> &retVal)
{
    return _new(provider, mode, key, NULL, retVal);
}

result_t Cipher_base::_new(int32_t provider, Buffer_base *key,
                           obj_ptr<Cipher_base> &retVal)
{
    return _new(provider, _STREAM, key, NULL, retVal);
}

Cipher::Cipher(const cipher_info_t *ci) : m_ci(ci)
{

}

result_t Cipher::get_name(std::string &retVal)
{
    retVal = m_ci->name;
    return 0;
}

result_t Cipher::cripto(object_base *v, exlib::AsyncEvent *ac)
{
    return 0;
}

result_t Cipher::setError(int ret)
{
    char msg[128];

    polarssl_strerror(ret, msg, sizeof(msg));
    return Runtime::setError(msg);
}

}
