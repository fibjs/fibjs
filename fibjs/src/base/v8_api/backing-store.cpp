/*
 * traceInfo.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: lion
 */

#include "include/v8-array-buffer.h"
#include "include/v8-internal.h"
#include "src/handles/handles.h"

#define private public
#include "v8/src/objects/backing-store.h"
#undef private

#include "v8.h"
#include "exlib/include/qstring.h"
#include "v8/src/api/api-inl.h"
#include "v8_api.h"

#include "v8/src/objects/backing-store.cc"

using namespace v8;

namespace fibjs {

static void custom_deleter(void* data, size_t length, void* deleter_data)
{
}

std::shared_ptr<v8::BackingStore> NewBackingStore(size_t byte_length)
{
    CHECK_LE(byte_length, i::JSArrayBuffer::kMaxByteLength);

    uint8_t* data = new uint8_t[byte_length + sizeof(i::BackingStore)];
    auto result = new ((i::BackingStore*)data) i::BackingStore(data + sizeof(i::BackingStore), byte_length, byte_length, byte_length,
        i::SharedFlag::kNotShared, i::ResizableFlag::kNotResizable, false, false, false, true, false);
    result->type_specific_data_.deleter = { custom_deleter, nullptr };

    return std::shared_ptr<v8::BackingStore>((v8::BackingStore*)result);
}

}