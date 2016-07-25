/*
 * crypto.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "Cipher.h"
#include "Buffer.h"
#include "PKey.h"
#include "X509Cert.h"
#include "X509Crl.h"
#include "X509Req.h"
#include "ssl.h"
#include <time.h>
#include <string.h>
#include <mbedtls/mbedtls/pkcs5.h>

namespace fibjs
{

DECLARE_MODULE(crypto);

result_t crypto_base::loadPKey(exlib::string filename, exlib::string password,
                               obj_ptr<PKey_base>& retVal)
{
    obj_ptr<PKey_base> key = new PKey();
    result_t hr = key->importFile(filename, password);
    if (hr < 0)
        return hr;

    retVal = key;

    return 0;
}

result_t crypto_base::loadCert(exlib::string filename, obj_ptr<X509Cert_base>& retVal)
{
    obj_ptr<X509Cert_base> cert = new X509Cert();
    result_t hr = cert->loadFile(filename);
    if (hr < 0)
        return hr;

    retVal = cert;

    return 0;
}

result_t crypto_base::loadCrl(exlib::string filename, obj_ptr<X509Crl_base>& retVal)
{
    obj_ptr<X509Crl_base> crl = new X509Crl();
    result_t hr = crl->loadFile(filename);
    if (hr < 0)
        return hr;

    retVal = crl;

    return 0;
}

result_t crypto_base::loadReq(exlib::string filename, obj_ptr<X509Req_base>& retVal)
{
    obj_ptr<X509Req_base> req = new X509Req();
    result_t hr = req->loadFile(filename);
    if (hr < 0)
        return hr;

    retVal = req;

    return 0;
}

result_t crypto_base::randomBytes(int32_t size, obj_ptr<Buffer_base> &retVal,
                                  AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    time_t t;
    int32_t i, ret;
    mbedtls_havege_state hs;
    unsigned char buf[1024];
    exlib::string strBuf;

    strBuf.resize(size);

    mbedtls_havege_init(&hs);

    t = time(NULL);

    for (i = 0; i < size; i += sizeof(buf))
    {
        ret = mbedtls_havege_random(&hs, buf, sizeof(buf));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        memcpy(&strBuf[i], buf, size - i > (int32_t)sizeof(buf) ? (int32_t)sizeof(buf) : size - i);
    }

    if (t == time(NULL))
        t--;

    retVal = new Buffer(strBuf);

    return 0;
}

result_t crypto_base::pseudoRandomBytes(int32_t size, obj_ptr<Buffer_base> &retVal,
                                        AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t i, ret;
    mbedtls_entropy_context entropy;
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];
    exlib::string strBuf;

    strBuf.resize(size);

    mbedtls_entropy_init(&entropy);

    for (i = 0; i < size; i += sizeof(buf))
    {
        ret = mbedtls_entropy_func(&entropy, buf, sizeof(buf));
        if (ret != 0)
        {
            mbedtls_entropy_free(&entropy);
            return CHECK_ERROR(_ssl::setError(ret));
        }

        memcpy(&strBuf[i], buf, size - i > (int32_t)sizeof(buf) ? (int32_t)sizeof(buf) : size - i);
    }

    mbedtls_entropy_free(&entropy);
    retVal = new Buffer(strBuf);

    return 0;
}

inline int32_t _max(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

inline int32_t _min(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

char *randomart(const unsigned char *dgst_raw, size_t dgst_raw_len,
                exlib::string title, int32_t size)
{
    const char augmentation_string[] = " .o+=*BOX@%&#/^SE";
    char *retval, *p;
    unsigned char *field;
    int32_t i, b, n;
    int32_t x, y;
    const size_t len = sizeof(augmentation_string) - 2;
    int32_t fieldY = size + 1;
    int32_t fieldX = size * 2 + 1;

    retval = (char *)calloc(1, (fieldX + 3) * (fieldY + 2));
    if (retval == NULL)
        return NULL;

    field = (unsigned char *)calloc(1, fieldX * fieldY);
    x = fieldX / 2;
    y = fieldY / 2;

    for (i = 0; i < (int32_t)dgst_raw_len; i++)
    {
        int32_t input;

        input = dgst_raw[i];
        for (b = 0; b < 4; b++)
        {
            x += (input & 0x1) ? 1 : -1;
            y += (input & 0x2) ? 1 : -1;

            x = _max(x, 0);
            y = _max(y, 0);
            x = _min(x, fieldX - 1);
            y = _min(y, fieldY - 1);

            if (field[x * fieldY + y] < len - 2)
                field[x * fieldY + y]++;
            input = input >> 2;
        }
    }

    field[(fieldX / 2) * fieldY + fieldY / 2] = len - 1;
    field[x * fieldY + y] = len;

    p = retval;
    *p++ = '+';
    for (i = 0; i < fieldX; i++)
        *p++ = '-';
    *p++ = '+';
    *p++ = '\n';

    n = (int32_t)title.length();
    if (n > 0)
    {
        if (n > fieldX - 2)
            n = fieldX - 2;
        p = retval + (fieldX - n) / 2;

        *p++ = '[';
        memcpy(p, title.c_str(), n);
        p += n;
        *p++ = ']';
        p = retval + fieldX + 3;
    }

    for (y = 0; y < fieldY; y++)
    {
        *p++ = '|';
        for (x = 0; x < fieldX; x++)
            *p++ = augmentation_string[_min(field[x * fieldY + y], len)];
        *p++ = '|';
        *p++ = '\n';
    }

    *p++ = '+';
    for (i = 0; i < fieldX; i++)
        *p++ = '-';
    *p++ = '+';

    free(field);

    return retval;
}

result_t crypto_base::randomArt(Buffer_base *data, exlib::string title,
                                int32_t size, exlib::string &retVal)
{
    exlib::string buf;

    data->toString(buf);
    char *str = randomart((const unsigned char *)buf.c_str(), buf.length(),
                          title, size);

    if (str)
    {
        retVal = str;
        free(str);
    }

    return 0;
}

inline int pkcs5_pbkdf1(mbedtls_md_context_t *ctx, const unsigned char *password,
                        size_t plen, const unsigned char *salt, size_t slen,
                        unsigned int iteration_count,
                        uint32_t key_length, unsigned char *output)
{
    int ret, j;
    unsigned int i;
    unsigned char md1[MBEDTLS_MD_MAX_SIZE];
    unsigned char work[MBEDTLS_MD_MAX_SIZE];
    unsigned char md_size = mbedtls_md_get_size(ctx->md_info);
    size_t use_len;
    unsigned char *out_p = output;
    bool bFirst = true;

    if (iteration_count > 0xFFFFFFFF)
        return (MBEDTLS_ERR_PKCS5_BAD_INPUT_DATA);

    while (key_length)
    {
        if ((ret = mbedtls_md_starts(ctx)) != 0)
            return (ret);

        if (!bFirst)
            if ((ret = mbedtls_md_update(ctx, md1, md_size)) != 0)
                return (ret);
        bFirst = false;

        if ((ret = mbedtls_md_update(ctx, password, plen)) != 0)
            return (ret);

        if ((ret = mbedtls_md_update(ctx, salt, slen)) != 0)
            return (ret);

        if ((ret = mbedtls_md_finish(ctx, work)) != 0)
            return (ret);

        memcpy(md1, work, md_size);

        for (i = 1; i < iteration_count; i++)
        {
            if ((ret = mbedtls_md_starts(ctx)) != 0)
                return (ret);

            if ((ret = mbedtls_md_update(ctx, md1, md_size)) != 0)
                return (ret);

            if ((ret = mbedtls_md_finish(ctx, work)) != 0)
                return (ret);

            memcpy(md1, work, md_size);
        }

        use_len = (key_length < md_size) ? key_length : md_size;
        memcpy(out_p, work, use_len);

        key_length -= (uint32_t) use_len;
        out_p += use_len;
    }

    return (0);
}

result_t crypto_base::pbkdf1(int32_t algo, Buffer_base* password,
                             Buffer_base* salt, int32_t iterations,
                             int32_t size, obj_ptr<Buffer_base>& retVal)
{
    if (algo < hash_base::_MD2 || algo > hash_base::_RIPEMD160)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    exlib::string str_pass;
    exlib::string str_salt;
    exlib::string str_key;

    password->toString(str_pass);
    salt->toString(str_salt);
    str_key.resize(size);

    mbedtls_md_context_t ctx;
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type((mbedtls_md_type_t)algo), 1);
    pkcs5_pbkdf1(&ctx, (const unsigned char *)str_pass.c_str(), str_pass.length(),
                 (const unsigned char *)str_salt.c_str(), str_salt.length(),
                 iterations, size, (unsigned char *)&str_key[0]);
    mbedtls_md_free(&ctx);

    retVal = new Buffer(str_key);

    return 0;

}

result_t crypto_base::pbkdf2(int32_t algo, Buffer_base* password,
                             Buffer_base* salt, int32_t iterations,
                             int32_t size, obj_ptr<Buffer_base>& retVal)
{
    if (algo < hash_base::_MD2 || algo > hash_base::_RIPEMD160)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    exlib::string str_pass;
    exlib::string str_salt;
    exlib::string str_key;

    password->toString(str_pass);
    salt->toString(str_salt);
    str_key.resize(size);

    mbedtls_md_context_t ctx;
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type((mbedtls_md_type_t)algo), 1);
    mbedtls_pkcs5_pbkdf2_hmac(&ctx, (const unsigned char *)str_pass.c_str(), str_pass.length(),
                              (const unsigned char *)str_salt.c_str(), str_salt.length(),
                              iterations, size, (unsigned char *)&str_key[0]);
    mbedtls_md_free(&ctx);

    retVal = new Buffer(str_key);

    return 0;
}

}
