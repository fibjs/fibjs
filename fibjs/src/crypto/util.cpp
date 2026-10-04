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

result_t crypto_base::timingSafeEqual(Union_timingSafeEqual_a a, Union_timingSafeEqual_b b, bool& retVal)
{
    // both arguments are used as their bytes; a string is encoded as utf8
    obj_ptr<Buffer_base> bufA;

    if (std::holds_alternative<exlib::string>(a)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(a), "utf8", bufA);
        if (hr < 0)
            return hr;
    } else
        bufA = std::get<obj_ptr<Buffer_base>>(a);

    obj_ptr<Buffer_base> bufB;

    if (std::holds_alternative<exlib::string>(b)) {
        result_t hr = Buffer_base::from(std::get<exlib::string>(b), "utf8", bufB);
        if (hr < 0)
            return hr;
    } else
        bufB = std::get<obj_ptr<Buffer_base>>(b);

    Buffer* _a = (Buffer*)bufA.get();
    Buffer* _b = (Buffer*)bufB.get();

    if (_a->length() != _b->length())
        return Runtime::setError("Buffer lengths must be equal, got %d and %d.", (int)_a->length(), (int)_b->length());

    retVal = CRYPTO_memcmp(_a->data(), _b->data(), _a->length()) == 0;

    return 0;
}


}
