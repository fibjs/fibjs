/*
 * util.cpp
 *
 *  Created on: Aug 28, 2024
 *      Author: lion
 */

#include "object.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include "Buffer.h"

namespace fibjs {

result_t crypto_base::timingSafeEqual(Buffer_base* a, Buffer_base* b, bool& retVal)
{
    Buffer* _a = (Buffer*)a;
    Buffer* _b = (Buffer*)b;

    if (_a->length() != _b->length())
        return Runtime::setError("Buffer lengths must be equal");

    retVal = CRYPTO_memcmp(_a->data(), _b->data(), _a->length()) == 0;

    return 0;
}

}
