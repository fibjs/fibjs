/*
 * crypto.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: lion
 */

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

namespace fibjs
{

result_t crypto_base::loadPKey(const char* filename, const char* password,
                               obj_ptr<PKey_base>& retVal)
{
    obj_ptr<PKey_base> key = new PKey();
    result_t hr = key->importFile(filename, password);
    if (hr < 0)
        return hr;

    retVal = key;

    return 0;
}

result_t crypto_base::loadCert(const char* filename, obj_ptr<X509Cert_base>& retVal)
{
    obj_ptr<X509Cert_base> cert = new X509Cert();
    result_t hr = cert->loadFile(filename);
    if (hr < 0)
        return hr;

    retVal = cert;

    return 0;
}

result_t crypto_base::loadCrl(const char* filename, obj_ptr<X509Crl_base>& retVal)
{
    obj_ptr<X509Crl_base> crl = new X509Crl();
    result_t hr = crl->loadFile(filename);
    if (hr < 0)
        return hr;

    retVal = crl;

    return 0;
}

result_t crypto_base::loadReq(const char* filename, obj_ptr<X509Req_base>& retVal)
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
    std::string strBuf;

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
    std::string strBuf;

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
                const char *title, int32_t size)
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

    n = (int32_t)qstrlen(title);
    if (n > 0)
    {
        if ( n > fieldX - 2)
            n = fieldX - 2;
        p = retval + (fieldX - n) / 2;

        *p++ = '[';
        memcpy(p, title, n);
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

result_t crypto_base::randomArt(Buffer_base *data, const char *title,
                                int32_t size, std::string &retVal)
{
    std::string buf;

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

}
