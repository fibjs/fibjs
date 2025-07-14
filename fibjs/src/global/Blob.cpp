#include "object.h"
#include "Blob.h"
#include "Buffer.h"
#include "encoding.h"
#include "v8/include/v8.h"

namespace fibjs {

result_t Blob_base::_new(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Blob> blob = new Blob();
    return blob->initialize(blobParts, options, retVal);
}

result_t Blob::get_type(exlib::string& retVal)
{
    retVal = m_type;
    return 0;
}

result_t Blob::get_size(int32_t& retVal)
{
    if (!m_buffer) {
        retVal = 0;
        return 0;
    }
    return m_buffer->get_length(retVal);
}

result_t Blob::slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal)
{
    // Handle default end value (-1 means slice to end)
    if (end == -1 && m_buffer) {
        int32_t bufferSize;
        m_buffer->get_length(bufferSize);
        end = bufferSize;
    }

    // Use Buffer's subarray which handles all edge cases
    obj_ptr<Buffer_base> slicedBuffer;
    if (m_buffer) {
        result_t hr = m_buffer.As<Buffer>()->subarray(start, end, slicedBuffer);
        if (hr < 0)
            return hr;
    } else {
        // Empty buffer for empty blob
        Buffer_base::allocUnsafe(0, slicedBuffer);
    }

    // Create new blob using same pattern as in _new function
    obj_ptr<Blob> newBlob = new Blob();
    newBlob->m_buffer = slicedBuffer;
    newBlob->m_type = contentType.empty() ? m_type : contentType;
    retVal = newBlob;

    return 0;
}

result_t Blob::text(exlib::string& retVal, AsyncEvent* ac)
{
    if (!m_buffer) {
        retVal = "";
        return 0;
    }

    return m_buffer->toString("utf8", 0, retVal);
}

result_t Blob::arrayBuffer(v8::Local<v8::ArrayBuffer>& retVal, AsyncEvent* ac)
{
    Isolate* isolate = holder();

    if (!m_buffer) {
        retVal = v8::ArrayBuffer::New(isolate->m_isolate, 0);
        return 0;
    }

    Buffer* buf = m_buffer.As<Buffer>();
    int32_t bufSize = buf->length();
    const uint8_t* data = buf->data();

    // Create new backing store and copy data
    std::unique_ptr<v8::BackingStore> store = v8::ArrayBuffer::NewBackingStore(isolate->m_isolate, bufSize);
    if (bufSize > 0 && store->Data() && data) {
        memcpy(store->Data(), data, bufSize);
    }

    // Create ArrayBuffer with the backing store
    retVal = v8::ArrayBuffer::New(isolate->m_isolate, std::move(store));

    return 0;
}

result_t Blob::initialize(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // Parse options for type
    if (!options.IsEmpty()) {
        v8::Local<v8::String> typeKey = isolate->NewString("type");
        if (options->Has(context, typeKey).FromMaybe(false)) {
            v8::Local<v8::Value> typeValue = options->Get(context, typeKey).FromMaybe(v8::Local<v8::Value>());
            if (!typeValue.IsEmpty() && typeValue->IsString()) {
                v8::String::Utf8Value type_str(isolate->m_isolate, typeValue);
                exlib::string type(*type_str);
                // Convert to lowercase
                exlib::string lowerType;
                for (size_t i = 0; i < type.length(); i++) {
                    char ch = type.c_str()[i];
                    if (ch >= 'A' && ch <= 'Z') {
                        lowerType += (ch + 32);
                    } else {
                        lowerType += ch;
                    }
                }
                m_type = lowerType;
            }
        }
    }

    // Process blob parts
    std::vector<obj_ptr<Buffer_base>> buffers;
    size_t totalSize = 0;
    result_t hr;

    if (!blobParts.IsEmpty()) {
        int32_t length = blobParts->Length();
        for (int32_t i = 0; i < length; i++) {
            v8::Local<v8::Value> part = blobParts->Get(context, i).FromMaybe(v8::Local<v8::Value>());

            if (part.IsEmpty()) {
                continue;
            }

            obj_ptr<Buffer_base> buffer;
            Blob* blobPart = static_cast<Blob*>(Blob_base::getInstance(part.As<v8::Object>()));
            if (blobPart) {
                // If part is a Blob, use its buffer directly
                if (!blobPart->m_buffer) {
                    continue;
                }

                buffer = blobPart->m_buffer;
            }

            if (!buffer) {
                hr = GetArgumentValue(isolate, part, buffer, false, "utf8");
                if (hr < 0 && hr != CALL_E_TYPEMISMATCH) {
                    return hr;
                }
            }

            if (!buffer) {
                v8::String::Utf8Value str(isolate->m_isolate, part->ToString(context).ToLocalChecked());
                hr = Buffer_base::from(*str, "utf8", buffer);
            }

            if (hr < 0) {
                return hr;
            }

            int32_t bufferSize;
            buffer->get_length(bufferSize);
            totalSize += bufferSize;
            buffers.push_back(buffer);
        }
    }

    // Concatenate all buffers
    if (buffers.empty()) {
        Buffer_base::allocUnsafe(0, m_buffer);
    } else if (buffers.size() == 1) {
        m_buffer = buffers[0];
    } else {
        // Create concatenated buffer
        Buffer_base::allocUnsafe(totalSize, m_buffer);
        Buffer* result = m_buffer.As<Buffer>();
        size_t offset = 0;

        for (auto& buf : buffers) {
            Buffer* srcBuf = buf.As<Buffer>();
            memcpy(result->data() + offset, srcBuf->data(), srcBuf->length());
            offset += srcBuf->length();
        }
    }

    retVal = this;
    return 0;
}

}
