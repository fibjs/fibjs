#include "object.h"
#include "date.h"
#include "Blob.h"
#include "Buffer.h"
#include "encoding.h"
#include "v8/include/v8.h"

namespace fibjs {

result_t Blob_base::_new(v8::Local<v8::Array> blobParts, BlobOptions* options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Blob> blob = new Blob();
    retVal = blob;
    return blob->m_impl.initialize(blobParts, options ? options->type.value_or("") : "");
}

result_t Blob_base::_new(Union_Blob_blobData blobData, BlobOptions* options, obj_ptr<Blob_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    v8::Local<v8::Array> blobParts = v8::Array::New(isolate->m_isolate, 1);

    if (std::holds_alternative<obj_ptr<Buffer_base>>(blobData))
        blobParts->Set(isolate->context(), 0, std::get<obj_ptr<Buffer_base>>(blobData)->wrap());
    else
        blobParts->Set(isolate->context(), 0, isolate->NewString(std::get<exlib::string>(blobData)));

    return _new(blobParts, options, retVal, This);
}

result_t File_base::_new(v8::Local<v8::Array> blobParts, exlib::string name,
    FileOptions* options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<File> file = new File();
    file->m_name = name;

    if (options && options->lastModified.has_value()) {
        file->m_lastModified = (double)options->lastModified.value();
    } else {
        date_t d;
        d.now();
        file->m_lastModified = d.date();
    }

    retVal = file;
    return file->m_impl.initialize(blobParts, options ? options->type.value_or("") : "");
}

result_t File_base::_new(Union_File_blobData blobData, exlib::string name, FileOptions* options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    v8::Local<v8::Array> blobParts = v8::Array::New(isolate->m_isolate, 1);

    if (std::holds_alternative<obj_ptr<Buffer_base>>(blobData))
        blobParts->Set(isolate->context(), 0, std::get<obj_ptr<Buffer_base>>(blobData)->wrap());
    else
        blobParts->Set(isolate->context(), 0, isolate->NewString(std::get<exlib::string>(blobData)));

    return _new(blobParts, name, options, retVal, This);
}

result_t File_base::_new(FileOptions* options, obj_ptr<File_base>& retVal, v8::Local<v8::Object> This)
{
    if (!options || !options->data.has_value())
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    exlib::string name = options->name.value_or("");
    Union_File_blobData blobData;

    if (std::holds_alternative<obj_ptr<Buffer_base>>(options->data.value()))
        blobData = std::get<obj_ptr<Buffer_base>>(options->data.value());
    else
        blobData = std::get<exlib::string>(options->data.value());

    return _new(blobData, name, options, retVal, This);
}

result_t BlobImpl::get_type(exlib::string& retVal)
{
    retVal = m_type;
    return result_t();
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
    if (hr.failed())
        return hr;

    retVal.m_buffer = slicedBuffer;
    retVal.m_type = m_type;
    return result_t();
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
    if (hr.failed())
        return hr;

    // Create new blob using same pattern as in _new function
    retVal.m_buffer = slicedBuffer;
    retVal.m_type = contentType.empty() ? m_type : contentType;

    return result_t();
}

result_t BlobImpl::slice(int32_t start, int32_t end, exlib::string contentType, obj_ptr<Blob_base>& retVal)
{
    obj_ptr<Blob> newBlob = new Blob();
    retVal = newBlob;
    return slice(start, end, contentType, newBlob->m_impl);
}

result_t BlobImpl::text(exlib::string& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    return m_buffer->toString("utf8", 0, retVal);
}

result_t BlobImpl::arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

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

    return result_t();
}

result_t BlobImpl::bytes(obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    // slice() copies into a fresh store, which is what the member promises:
    // the result must not alias the Blob's bytes (subarray() would share the
    // backing store and let a write reach the Blob).
    return m_buffer.As<Buffer>()->slice(0, retVal);
}

result_t BlobImpl::initialize(v8::Local<v8::Array> blobParts, exlib::string type)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    // The MIME type is lowercased before it is stored.
    for (size_t i = 0; i < type.length(); i++) {
        char ch = type[i];
        if (ch >= 'A' && ch <= 'Z')
            m_type += (ch + 32);
        else
            m_type += ch;
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
                hr = GetArgumentValue(isolate, part, buffer);
                if (hr.failed_other_than(CALL_E_TYPEMISMATCH)) {
                    return hr;
                }
            }

            if (!buffer) {
                // every other part is its DOM string form: the blob renders the
                // value itself (null becomes "null", 0 becomes "0"), which is
                // what USVString conversion means in the spec
                exlib::string partStr;

                hr = GetDOMStringValue(part, partStr);
                if (hr.failed())
                    return hr;

                hr = Buffer_base::from(partStr, "utf8", buffer);
                if (hr.failed())
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

    return result_t();
}

}
