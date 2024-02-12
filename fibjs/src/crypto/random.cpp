/*
 * random.cpp
 *
 *  Created on: Feb 11, 2024
 *      Author: lion
 */

#include "object.h"
#include "crypto.h"
#include "ifs/crypto.h"
#include "Buffer.h"
#include <crypto/rand.h>

namespace fibjs {

result_t randomBytes(uint8_t* buf, int32_t size)
{
    if (RAND_bytes_ex(nullptr, buf, size, 0) < 0)
        return openssl_error();

    return 0;
}

result_t crypto_base::randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (size < 1)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer> buf_rand = new Buffer(NULL, size);
    uint8_t* buf = buf_rand->data();

    result_t hr = fibjs::randomBytes(buf, size);
    if (hr < 0)
        return hr;

    retVal = buf_rand;
    return 0;
}

result_t crypto_base::randomFill(Buffer_base* buffer, int32_t offset, int32_t size,
    obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    int32_t len = Buffer::Cast(buffer)->length();

    if (offset < 0 || offset > len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (size < 0)
        size = len - offset;
    else if (size + offset > len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (size == 0) {
        retVal = buffer;
        return 0;
    }

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> rand;
    randomBytes(size, rand, ac);

    return buffer->fill(rand, offset, offset + size, retVal);
}

}
