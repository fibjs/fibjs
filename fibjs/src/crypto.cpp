/*
 * crypto.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "Cipher.h"
#include "Buffer.h"
#include <polarssl/config.h>
#include <polarssl/havege.h>
#include <polarssl/entropy.h>
#include <time.h>

namespace fibjs
{

result_t crypto_base::randomBytes(int32_t size, obj_ptr<Buffer_base> &retVal,
                                  exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    time_t t;
    int i, ret;
    havege_state hs;
    unsigned char buf[1024];
    std::string strBuf;

    strBuf.resize(size);

    havege_init(&hs);

    t = time(NULL);

    for (i = 0; i < size; i += sizeof(buf))
    {
        ret = havege_random(&hs, buf, sizeof(buf));
        if (ret != 0)
            return Cipher::setError(ret);

        memcpy(&strBuf[i], buf, size - i > sizeof(buf) ? sizeof(buf) : size - i);
    }

    if (t == time(NULL))
        t--;

    retVal = new Buffer(strBuf);

    return 0;
}

result_t crypto_base::pseudoRandomBytes(int32_t size, obj_ptr<Buffer_base> &retVal,
                                        exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    int i, ret;
    entropy_context entropy;
    unsigned char buf[ENTROPY_BLOCK_SIZE];
    std::string strBuf;

    strBuf.resize(size);

    entropy_init(&entropy);

    for (i = 0; i < size; i += sizeof(buf))
    {
        ret = entropy_func(&entropy, buf, sizeof(buf));
        if (ret != 0)
        {
            entropy_free(&entropy);
            return Cipher::setError(ret);
        }

        memcpy(&strBuf[i], buf, size - i > sizeof(buf) ? sizeof(buf) : size - i);
    }

    entropy_free(&entropy);
    retVal = new Buffer(strBuf);

    return 0;
}

}
