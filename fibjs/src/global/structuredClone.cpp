/*
 * structuredClone.cpp
 *
 *  Created on: Dec 21, 2025
 *      Author: fibjs team
 */

#include "object.h"
#include "ifs/global.h"
#include "Runtime.h"
#include <vector>

namespace fibjs {

class StructuredCloneSerializer : public v8::ValueSerializer::Delegate {
public:
    StructuredCloneSerializer(v8::Isolate* isolate,
        const std::vector<v8::Local<v8::ArrayBuffer>>& transferList)
        : m_isolate(isolate)
        , m_transferList(transferList)
    {
    }

    void ThrowDataCloneError(v8::Local<v8::String> message) override
    {
        m_isolate->ThrowException(v8::Exception::Error(message));
    }

    v8::Maybe<uint32_t> GetSharedArrayBufferId(
        v8::Isolate* isolate,
        v8::Local<v8::SharedArrayBuffer> shared_array_buffer) override
    {
        for (size_t i = 0; i < m_sharedArrayBuffers.size(); i++) {
            if (m_sharedArrayBuffers[i] == shared_array_buffer) {
                return v8::Just(static_cast<uint32_t>(i));
            }
        }

        m_sharedArrayBuffers.push_back(shared_array_buffer);
        return v8::Just(static_cast<uint32_t>(m_sharedArrayBuffers.size() - 1));
    }

    std::vector<v8::Local<v8::SharedArrayBuffer>>& sharedArrayBuffers()
    {
        return m_sharedArrayBuffers;
    }

    std::vector<std::shared_ptr<v8::BackingStore>>& arrayBufferBackingStores()
    {
        return m_arrayBufferBackingStores;
    }

    bool PrepareTransfer(Isolate* fibjs_isolate)
    {
        for (auto& ab : m_transferList) {
            if (ab->WasDetached()) {
                m_isolate->ThrowException(v8::Exception::TypeError(
                    fibjs_isolate->NewString("ArrayBuffer has already been detached")));
                return false;
            }
            m_arrayBufferBackingStores.push_back(ab->GetBackingStore());
        }
        return true;
    }

    void DetachArrayBuffers()
    {
        for (auto& ab : m_transferList) {
            ab->Detach(v8::Local<v8::Value>()).Check();
        }
    }

private:
    v8::Isolate* m_isolate;
    const std::vector<v8::Local<v8::ArrayBuffer>>& m_transferList;
    std::vector<v8::Local<v8::SharedArrayBuffer>> m_sharedArrayBuffers;
    std::vector<std::shared_ptr<v8::BackingStore>> m_arrayBufferBackingStores;
};

class StructuredCloneDeserializer : public v8::ValueDeserializer::Delegate {
public:
    StructuredCloneDeserializer(v8::Isolate* isolate,
        std::vector<v8::Local<v8::SharedArrayBuffer>>& sharedArrayBuffers)
        : m_isolate(isolate)
        , m_sharedArrayBuffers(sharedArrayBuffers)
    {
    }

    v8::MaybeLocal<v8::SharedArrayBuffer> GetSharedArrayBufferFromId(
        v8::Isolate* isolate, uint32_t clone_id) override
    {
        if (clone_id < m_sharedArrayBuffers.size()) {
            return m_sharedArrayBuffers[clone_id];
        }
        return v8::MaybeLocal<v8::SharedArrayBuffer>();
    }

private:
    v8::Isolate* m_isolate;
    std::vector<v8::Local<v8::SharedArrayBuffer>>& m_sharedArrayBuffers;
};

result_t global_base::structuredClone(v8::Local<v8::Value> value,
    v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    Isolate* _isolate = Isolate::current();
    v8::Isolate* isolate = _isolate->m_isolate;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // Parse transfer list from options
    std::vector<v8::Local<v8::ArrayBuffer>> transferList;

    if (!options.IsEmpty() && !options->IsNullOrUndefined()) {
        v8::Local<v8::Value> transferValue;
        v8::Local<v8::String> transferKey = _isolate->NewString("transfer");

        if (options->Get(context, transferKey).ToLocal(&transferValue)) {
            if (transferValue->IsArray()) {
                v8::Local<v8::Array> transferArray = v8::Local<v8::Array>::Cast(transferValue);
                uint32_t length = transferArray->Length();

                for (uint32_t i = 0; i < length; i++) {
                    v8::Local<v8::Value> item;
                    if (transferArray->Get(context, i).ToLocal(&item)) {
                        if (item->IsArrayBuffer()) {
                            transferList.push_back(v8::Local<v8::ArrayBuffer>::Cast(item));
                        } else {
                            return CHECK_ERROR(Runtime::setError("Transfer list must contain only ArrayBuffer objects"));
                        }
                    }
                }
            } else if (!transferValue->IsUndefined()) {
                return CHECK_ERROR(Runtime::setError("options.transfer must be an array"));
            }
        }
    }

    // Create serializer
    StructuredCloneSerializer serializerDelegate(isolate, transferList);
    v8::ValueSerializer serializer(isolate, &serializerDelegate);

    // Prepare transfer
    if (!serializerDelegate.PrepareTransfer(_isolate)) {
        return CALL_E_JAVASCRIPT;
    }

    // Register transferred ArrayBuffers
    for (size_t i = 0; i < transferList.size(); i++) {
        serializer.TransferArrayBuffer(static_cast<uint32_t>(i), transferList[i]);
    }

    // Write header
    serializer.WriteHeader();

    // Write value
    v8::Maybe<bool> writeResult = serializer.WriteValue(context, value);
    if (writeResult.IsNothing() || !writeResult.FromJust()) {
        return CALL_E_JAVASCRIPT;
    }

    // Detach transferred ArrayBuffers after successful serialization
    serializerDelegate.DetachArrayBuffers();

    // Get serialized data
    std::pair<uint8_t*, size_t> buffer = serializer.Release();

    // Create deserializer
    StructuredCloneDeserializer deserializerDelegate(isolate,
        serializerDelegate.sharedArrayBuffers());
    v8::ValueDeserializer deserializer(isolate, buffer.first, buffer.second,
        &deserializerDelegate);

    // Transfer ArrayBuffers to the new context
    auto& backingStores = serializerDelegate.arrayBufferBackingStores();
    for (size_t i = 0; i < backingStores.size(); i++) {
        v8::Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(isolate, backingStores[i]);
        deserializer.TransferArrayBuffer(static_cast<uint32_t>(i), ab);
    }

    // Read header
    v8::Maybe<bool> headerResult = deserializer.ReadHeader(context);
    if (headerResult.IsNothing() || !headerResult.FromJust()) {
        free(buffer.first);
        return CALL_E_JAVASCRIPT;
    }

    // Read value
    v8::MaybeLocal<v8::Value> result = deserializer.ReadValue(context);
    free(buffer.first);

    if (result.IsEmpty()) {
        return CALL_E_JAVASCRIPT;
    }

    retVal = result.ToLocalChecked();
    return 0;
}

}
