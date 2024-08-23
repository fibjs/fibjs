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
    std::shared_ptr<v8::BackingStore> buffer = data->Buffer()->GetBackingStore();
    uint8_t* ptr = static_cast<uint8_t*>(buffer->Data());
    size_t len = buffer->ByteLength();

    if (len > 65536)
        return CALL_E_OUTRANGE;

    if (len > 0 && RAND_bytes(ptr, len) != 1)
        return openssl_error();

    retVal = data;

    return 0;
}

result_t webcrypto_base::randomUUID(exlib::string& retVal)
{
    unsigned char buf[16];

    if (RAND_bytes(buf, sizeof(buf)) != 1)
        return openssl_error();

    buf[6] = (buf[6] & 0x0f) | 0x40;

    retVal.resize(36);
    char* sbuf = retVal.data();
    static char hex[] = "0123456789abcdef";

    for (int32_t i = 0; i < 16; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            *sbuf++ = '-';
        *sbuf++ = hex[buf[i] >> 4];
        *sbuf++ = hex[buf[i] & 0x0f];
    }

    return 0;
}

}
