/*
 * random.cpp
 *
 *  Created on: Aug 23, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/webcrypto.h"
#include "crypto_util.h"

namespace fibjs {

result_t webcrypto_base::getRandomValues(v8::Local<v8::TypedArray> data, v8::Local<v8::TypedArray>& retVal)
{
    // WebCrypto limits the *view* to 65536 bytes, not the whole backing store:
    // a small Uint8Array over a large WebAssembly.Memory must still work.
    size_t len = data->ByteLength();

    if (len > 65536)
        return result_t::out_of_range();

    if (len > 0) {
        std::shared_ptr<v8::BackingStore> buffer = data->Buffer()->GetBackingStore();
        size_t offset = data->ByteOffset();

        if (!buffer || offset > buffer->ByteLength() || len > buffer->ByteLength() - offset)
            return result_t::out_of_range();

        // Only fill the bytes covered by the view; the backing store may be
        // much larger (e.g. wasm linear memory).
        if (RAND_bytes(static_cast<uint8_t*>(buffer->Data()) + offset, (int)len) != 1)
            return openssl_error();
    }

    retVal = data;

    return result_t();
}

result_t webcrypto_base::randomUUID(exlib::string& retVal)
{
    unsigned char buf[16];

    if (RAND_bytes(buf, sizeof(buf)) != 1)
        return openssl_error();

    // Set version to 4
    buf[6] = (buf[6] & 0x0f) | 0x40;
    // Set variant bits to 10
    buf[8] = (buf[8] & 0x3f) | 0x80;

    retVal.resize(36);
    char* sbuf = retVal.data();
    static char hex[] = "0123456789abcdef";

    for (int32_t i = 0; i < 16; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            *sbuf++ = '-';
        *sbuf++ = hex[buf[i] >> 4];
        *sbuf++ = hex[buf[i] & 0x0f];
    }

    return result_t();
}

}
