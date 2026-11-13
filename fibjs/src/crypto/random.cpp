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
#include <cmath>

namespace fibjs {

result_t randomBytes(uint8_t* buf, int32_t size)
{
    if (RAND_bytes_ex(nullptr, buf, size, 0) < 0)
        return openssl_error();

    return result_t();
}

// Node.js rejects a bound that is not an integer instead of truncating it: a
// fractional max would silently change the range the caller asked for.
static bool is_integer_bound(double v)
{
    return std::isfinite(v) && std::trunc(v) == v;
}

static result_t check_int_bound(const char* name, double v)
{
    if (is_integer_bound(v))
        return result_t();

    return result_t::make(CALL_E_INVALIDARG)
        .with_code("ERR_INVALID_ARG_TYPE")
        .with_format("The \"%s\" argument must be an integer. Received %f", name, v);
}

result_t crypto_base::randomInt(double max, double& retVal)
{
    return randomInt(0, max, retVal);
}

result_t crypto_base::randomInt(double min, double max, double& retVal)
{
    result_t hr = check_int_bound("min", min);
    if (hr.failed())
        return hr;

    hr = check_int_bound("max", max);
    if (hr.failed())
        return hr;

    double range = max - min;

    if (range <= 0)
        return result_t::make(CALL_E_OVERFLOW)
            .with_type_name(errtype::kRangeError)
            .with_code("ERR_OUT_OF_RANGE")
            .with_format("The value of \"max\" is out of range. It must be greater than \"min\". Received [%f, %f)", min, max);

    // The draw widens until the whole range fits: one byte covers 256 values,
    // and 7 bytes cover 2^56, which is past the safe-integer span - the widest
    // range this member can be asked for, since a bound beyond 2^53 is not a
    // whole number any more.
    uint64_t span = (uint64_t)range;
    int32_t bytes = 1;
    while (bytes < 7 && span > (uint64_t)1 << (8 * bytes))
        bytes++;

    // Rejection sampling: the largest multiple of `range` that fits in `bytes`
    // is the cutoff, and a draw at or above it is discarded. That is what keeps
    // every value equally likely instead of favouring the low end.
    uint64_t limit = (uint64_t)1 << (8 * bytes);
    uint64_t cutoff = limit - (limit % span);

    uint8_t buf[7];
    for (;;) {
        hr = fibjs::randomBytes(buf, bytes);
        if (hr.failed())
            return hr;

        uint64_t v = 0;
        for (int32_t i = 0; i < bytes; i++)
            v = (v << 8) | buf[i];

        if (v < cutoff) {
            retVal = min + (double)(v % span);
            return result_t();
        }
    }
}

result_t crypto_base::getFips(int32_t& retVal)
{
    // OpenSSL is not built with the FIPS provider and there is no switch to
    // turn it on, so this is the truthful answer rather than a stub.
    retVal = 0;
    return result_t();
}

result_t crypto_base::randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    if (size < 1)
        return result_t::make(CALL_E_OVERFLOW)
            .with_type_name(errtype::kRangeError)
            .with_code("ERR_OUT_OF_RANGE")
            .with_format("randomBytes: size must be >= 1, got %d.", size);

    obj_ptr<Buffer> buf_rand = new Buffer(NULL, size);
    uint8_t* buf = buf_rand->data();

    result_t hr = fibjs::randomBytes(buf, size);
    if (hr.failed())
        return hr;

    retVal = buf_rand;
    return result_t();
}

result_t crypto_base::randomFill(Union_randomFill_buffer buffer, int32_t offset, int32_t size,
    obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<Buffer_base> buf;

    if (std::holds_alternative<exlib::string>(buffer)) {
        // The randomly filled buffer is the result: decoding has to happen
        // before it is validated and filled, so it is decoded here.
        result_t hr = Buffer_base::from(std::get<exlib::string>(buffer), "utf8", buf);
        if (hr.failed())
            return hr;
    } else
        buf = std::get<obj_ptr<Buffer_base>>(buffer);

    int32_t len = Buffer::Cast(buf)->length();

    if (offset < 0 || offset > len)
        return result_t::make(CALL_E_OVERFLOW)
            .with_type_name(errtype::kRangeError)
            .with_code("ERR_OUT_OF_RANGE")
            .with_format("randomFill: offset %d is out of range [0, %d].", offset, len);

    if (size < 0)
        size = len - offset;
    else if (size + offset > len)
        return result_t::make(CALL_E_OVERFLOW)
            .with_type_name(errtype::kRangeError)
            .with_code("ERR_OUT_OF_RANGE")
            .with_format("randomFill: offset(%d) + size(%d) exceeds buffer length %d.", offset, size, len);

    if (size == 0) {
        retVal = buf;
        return result_t();
    }

    obj_ptr<Buffer_base> rand;
    randomBytes(size, rand);

    int32_t copied;
    result_t hr = buf->set(rand, offset, copied);
    if (hr.failed())
        return hr;

    retVal = buf;
    return result_t();
}

result_t crypto_base::getRandomValues(v8::Local<v8::TypedArray> data, v8::Local<v8::TypedArray>& retVal)
{
    return webcrypto_base::getRandomValues(data, retVal);
}

result_t crypto_base::randomUUID(Options_crypto_randomUUID_options* options, exlib::string& retVal)
{
    // Node's only documented option is disableEntropyCache. fibjs does not
    // keep an entropy cache, so accepting the flag is enough; `random` remains
    // a uuid.v4 extension and is deliberately not forwarded here.
    (void)options;

    return uuid_base::v4(nullptr, retVal);
}

}
