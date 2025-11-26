#include "object.h"
#include "date.h"
#include "Blob.h"
#include "Buffer.h"
#include "encoding.h"
#include "v8/include/v8.h"

namespace fibjs {

result_t Blob_base::_new(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Blob> blob = new Blob();
    retVal = blob;
    return blob->m_impl.initialize(blobParts, options);
}

result_t Blob_base::_new(Buffer_base* blobData, v8::Local<v8::Object> options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    v8::Local<v8::Array> blobParts = v8::Array::New(isolate->m_isolate, 1);
    blobParts->Set(isolate->context(), 0, blobData->wrap());

    return _new(blobParts, options, retVal, This);
}

result_t File_base::_new(v8::Local<v8::Array> blobParts, exlib::string name,
    v8::Local<v8::Object> options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<File> file = new File();
    file->m_name = name;

    result_t hr = GetConfigValue(options, "lastModified", file->m_lastModified, true);
    if (hr == CALL_E_PARAMNOTOPTIONAL) {
        date_t d;
        d.now();
        file->m_lastModified = d.date();
    } else if (hr < 0) {
        return hr;
    }

    retVal = file;
    return file->m_impl.initialize(blobParts, options);
}

result_t File_base::_new(Buffer_base* blobData, exlib::string name, v8::Local<v8::Object> options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    v8::Local<v8::Array> blobParts = v8::Array::New(isolate->m_isolate, 1);
    blobParts->Set(isolate->context(), 0, blobData->wrap());

    return _new(blobParts, name, options, retVal, This);
}

result_t File_base::_new(v8::Local<v8::Object> options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This)
{
    result_t hr;
    obj_ptr<Buffer_base> blobData;
    exlib::string name;

    hr = GetConfigValue(options, "data", blobData);
    if (hr < 0) {
        return hr;
    }

    hr = GetConfigValue(options, "name", name, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL) {
        return hr;
    }

    return _new(blobData, name, options, retVal, This);
}

result_t BlobImpl::get_type(exlib::string& retVal)
{
    retVal = m_type;
    return 0;
}

result_t BlobImpl::get_size(int32_t& retVal)
{
    return m_buffer->get_length(retVal);
}

result_t BlobImpl::clone(BlobImpl& retVal)
{
    int32_t bufferSize;
    m_buffer->get_length(bufferSize);

    obj_ptr<Buffer_base> slicedBuffer;
    result_t hr = m_buffer.As<Buffer>()->subarray(0, bufferSize, slicedBuffer);
    if (hr < 0)
        return hr;

    retVal.m_buffer = slicedBuffer;
    retVal.m_type = m_type;
    return 0;
}

result_t BlobImpl::slice(int32_t start, int32_t end, exlib::string contentType, BlobImpl& retVal)
{
    // Handle default end value (-1 means slice to end)
    if (end == -1) {
        int32_t bufferSize;
        m_buffer->get_length(bufferSize);
        end = bufferSize;
    }

    // Use Buffer's subarray which handles all edge cases
    obj_ptr<Buffer_base> slicedBuffer;
    result_t hr = m_buffer.As<Buffer>()->subarray(start, end, slicedBuffer);
    if (hr < 0)
        return hr;

    // Create new blob using same pattern as in _new function
    retVal.m_buffer = slicedBuffer;
    retVal.m_type = contentType.empty() ? m_type : contentType;

    return 0;
}

result_t BlobImpl::slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal)
{
    obj_ptr<Blob> newBlob = new Blob();
    retVal = newBlob;
    return slice(start, end, contentType, newBlob->m_impl);
}

result_t BlobImpl::text(exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return m_buffer->toString("utf8", 0, retVal);
}

result_t BlobImpl::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    Buffer* buf = m_buffer.As<Buffer>();
    int32_t bufSize = buf->length();
    const uint8_t* data = buf->data();

    // Create new backing store and copy data
    std::shared_ptr<v8::BackingStore> store = NewBackingStore(bufSize);
    if (bufSize > 0 && store->Data() && data) {
        memcpy(store->Data(), data, bufSize);
    }

    // Create ArrayBuffer with the backing store
    retVal = std::move(store);

    return 0;
}

result_t BlobImpl::initialize(v8::Local<v8::Array> blobParts, v8::Local<v8::Object> options)
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
                    char ch = type[i];
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
            Blob* blobPart = static_cast<Blob*>(Blob_base::getInstance(part));
            if (blobPart) {
                // If part is a Blob, use its buffer directly
                if (!blobPart->m_impl.m_buffer) {
                    continue;
                }

                buffer = blobPart->m_impl.m_buffer;
            }

            if (!buffer) {
                hr = GetArgumentValue(isolate, part, buffer, false, "utf8");
                if (hr < 0 && hr != CALL_E_TYPEMISMATCH) {
                    return hr;
                }
            }

            if (!buffer) {
                v8::Local<v8::String> partStr = part->ToString(isolate->context()).ToLocalChecked();
                if (partStr.IsEmpty()) {
                    return CALL_E_TYPEMISMATCH;
                }
                v8::String::Utf8Value str(isolate->m_isolate, partStr);
                hr = Buffer_base::from(*str, "utf8", buffer);
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

    return 0;
}

}
