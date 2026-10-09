/*
 * random.cpp
 *
 *  Created on: Feb 11, 2024
 *      Author: lion
 */

#include "object.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include "ifs/uuid.h"
#include "Buffer.h"
#include <crypto/rand.h>

namespace fibjs {

result_t randomBytes(uint8_t* buf, int32_t size)
{
    if (RAND_bytes_ex(nullptr, buf, size, 0) < 0)
        return openssl_error();

    return 0;
}

result_t crypto_base::randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    if (size < 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_OUTRANGE, "randomBytes: size must be >= 1, got %d.", size));

    obj_ptr<Buffer> buf_rand = new Buffer(NULL, size);
    uint8_t* buf = buf_rand->data();

    result_t hr = fibjs::randomBytes(buf, size);
    if (hr < 0)
        return hr;

    retVal = buf_rand;
    return 0;
}

result_t crypto_base::randomFill(Union_randomFill_buffer buffer, int32_t offset, int32_t size,
    obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<exlib::string>(buffer)) {
        // The randomly filled buffer is the result: decoding has to happen
        // before it is validated and filled, so it is decoded here.
        result_t hr = Buffer_base::from(std::get<exlib::string>(buffer), "utf8", buf);
        if (hr < 0)
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(buffer);

    int32_t len = Buffer::Cast(buf)->length();

    if (offset < 0 || offset > len)
        return CHECK_ERROR(Runtime::setError(CALL_E_OUTRANGE, "randomFill: offset %d is out of range [0, %d].", offset, len));

    if (size < 0)
        size = len - offset;
    else if (size + offset > len)
        return CHECK_ERROR(Runtime::setError(CALL_E_OUTRANGE, "randomFill: offset(%d) + size(%d) exceeds buffer length %d.", offset, size, len));

    if (size == 0) {
        retVal = buf;
        return 0;
    }

    obj_ptr<Buffer_base> rand;
    randomBytes(size, rand);

    int32_t copied;
    result_t hr = buf->set(rand, offset, copied);
    if (hr < 0)
        return hr;

    retVal = buf;
    return 0;
}

result_t crypto_base::getRandomValues(v8::Local<v8::TypedArray> data, v8::Local<v8::TypedArray>& retVal)
{
    return webcrypto_base::getRandomValues(data, retVal);
}

result_t crypto_base::randomUUID(v8::Local<v8::Object> options, exlib::string& retVal)
{
    // Generate UUID v4 using uuid module
    return uuid_base::v4(options, retVal);
}

}
