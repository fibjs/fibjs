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

// The BackingStore is placement constructed at the beginning of the block that
// NewBackingStore() handed out, so returning the block means destroying the
// embedded object first (it unregisters itself from v8) and then handing the
// whole block back to the same allocator that produced it.
static void free_backing_store_block(v8::BackingStore* store)
{
    store->~BackingStore();
    free(store);
}

static std::shared_ptr<v8::BackingStore> make_backing_store(void* block, size_t byte_length)
{
    uint8_t* data = (uint8_t*)block;
    auto result = new ((i::BackingStore*)data) i::BackingStore(nullptr, data + sizeof(i::BackingStore), byte_length, byte_length, byte_length,
        i::SharedFlag::kNotShared, i::ResizableFlag::kNotResizable, false, false, false, true, false);
    result->type_specific_data_.deleter = { custom_deleter, nullptr };

    return std::shared_ptr<v8::BackingStore>((v8::BackingStore*)result, free_backing_store_block);
}

std::shared_ptr<v8::BackingStore> NewBackingStore(size_t byte_length)
{
    return NewBackingStore(byte_length, false);
}

std::shared_ptr<v8::BackingStore> NewBackingStore(size_t byte_length, bool zero)
{
    CHECK_LE(byte_length, i::JSArrayBuffer::kMaxByteLength);

    // One block holds the BackingStore object followed by its payload, so it is
    // allocated (and later released) as a single unit.  calloc() for the zero
    // filled case: the kernel hands out fresh pages already zeroed, which avoids
    // an eager memset of the whole payload.
    void* block = zero
        ? calloc(1, byte_length + sizeof(i::BackingStore))
        : malloc(byte_length + sizeof(i::BackingStore));
    if (block == nullptr)
        throw std::bad_alloc();

    return make_backing_store(block, byte_length);
}

}