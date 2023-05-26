#include "object.h"
#include "Buffer.h"
#include "encoding.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace fibjs {

DECLARE_MODULE_EX(buffer, Buffer);

inline result_t generateEnd(const int32_t buffer_length, const int32_t offset, int32_t& end)
{
    if (end < 0)
        end = buffer_length + end + 1;

    if (offset < 0 || end < 0 || offset > end)
        return CALL_E_INVALIDARG;

    if (end > buffer_length)
        return CALL_E_OUTRANGE;

    return 0;
}

inline result_t validOffset(const int32_t buffer_length, const int32_t offset)
{
    if (offset < 0)
        return CALL_E_INVALIDARG;

    if (offset > buffer_length)
        return CALL_E_OUTRANGE;

    return 0;
}

result_t Buffer_base::_new(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(datas, retVal);
}

result_t Buffer_base::_new(v8::Local<v8::ArrayBuffer> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(datas, byteOffset, length, retVal);
}

result_t Buffer_base::_new(v8::Local<v8::Uint8Array> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(datas, byteOffset, length, retVal);
}

result_t Buffer_base::_new(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return from(str, codec, retVal);
}

result_t Buffer_base::_new(int32_t size, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    return allocUnsafe(size, retVal);
}

result_t Buffer_base::alloc(int32_t size, int32_t fill, obj_ptr<Buffer_base>& retVal)
{
    if (size < 0)
        size = 0;

    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    buf->fill(fill, 0, size, retVal);
    retVal = buf;
    return 0;
}

result_t Buffer_base::alloc(int32_t size, exlib::string fill, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    return buf->fill(fill, 0, size, codec, retVal);
}

result_t Buffer_base::alloc(int32_t size, Buffer_base* fill, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, size);
    return buf->fill(fill, 0, size, retVal);
}

result_t Buffer_base::allocUnsafe(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(NULL, size);
    return 0;
}

result_t Buffer_base::allocUnsafeSlow(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer(NULL, size);
    return 0;
}

result_t Buffer_base::from(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buf = new Buffer(NULL, datas->Length());
    retVal = buf;

    int32_t sz = (int32_t)buf->length();
    v8::Local<v8::Context> context = datas->GetCreationContextChecked();
    Isolate* isolate = Isolate::current(context);

    if (sz) {
        int32_t i;
        result_t hr;
        uint8_t* _str = buf->data();

        for (i = 0; i < sz; i++) {
            JSValue v = datas->Get(context, i);
            int32_t num;

            hr = GetArgumentValue(isolate, v, num);
            if (hr < 0)
                return CHECK_ERROR(hr);

            _str[i] = (uint8_t)num;
        }
    }

    return 0;
}

result_t Buffer_base::from(Buffer_base* buffer, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> buffer1 = Buffer::Cast(buffer);

    if (byteOffset < 0)
        byteOffset = 0;

    if (length < 0)
        length = buffer1->length() - byteOffset;

    if (length < 0 || length > buffer1->length() - byteOffset)
        length = 0;

    retVal = new Buffer(buffer1->data() + byteOffset, length);

    return 0;
}

result_t Buffer_base::from(v8::Local<v8::ArrayBuffer> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    std::shared_ptr<v8::BackingStore> cnt = datas->GetBackingStore();

    if (byteOffset < 0)
        byteOffset = 0;

    if (length < 0)
        length = cnt->ByteLength() - byteOffset;

    if (length < 0 || length > cnt->ByteLength() - byteOffset)
        length = 0;

    retVal = new Buffer((uint8_t*)cnt->Data() + byteOffset, length);

    return 0;
}

result_t Buffer_base::from(v8::Local<v8::Uint8Array> datas, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal)
{
    if (byteOffset < 0)
        return CALL_E_INVALIDARG;

    if (length < 0)
        length = datas->ByteLength() - byteOffset;
    else if (length > datas->ByteLength() - byteOffset)
        return CALL_E_INVALIDARG;

    return from(datas->Buffer(), byteOffset + datas->ByteOffset(), length, retVal);
}

result_t Buffer_base::from(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    result_t hr = commonDecode(codec, str, strBuf);
    if (hr < 0)
        return hr;

    retVal = new Buffer(strBuf.c_str(), strBuf.length());
    return 0;
}

result_t Buffer_base::concat(v8::Local<v8::Array> buflist, int32_t cutLength, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current(buflist);
    v8::Local<v8::Context> context = isolate->context();

    std::vector<v8::Local<v8::Uint8Array>> bufs;
    int32_t length = buflist->Length();
    size_t sz = 0;

    bufs.resize(length);

    for (int32_t i = 0; i < length; i++) {
        v8::Local<v8::Value> val = buflist->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        if (val.IsEmpty() || !val->IsUint8Array())
            return CALL_E_INVALIDARG;
        bufs[i] = v8::Local<v8::Uint8Array>::Cast(val);
        sz += bufs[i]->ByteLength();
    }

    if (cutLength >= 0 && sz > cutLength)
        sz = cutLength;

    obj_ptr<Buffer> buf = new Buffer(NULL, sz);
    size_t pos = 0;

    for (int32_t i = 0; i < length; i++) {
        int32_t len = bufs[i]->ByteLength();
        if (pos + len > sz)
            len = sz - pos;
        if (len <= 0)
            break;
        bufs[i]->CopyContents(buf->data() + pos, len);
        pos += len;
    }

    retVal = buf;

    return 0;
}

void Buffer::init(const void* data, size_t length)
{
    m_store = NewBackingStore(length);
    if (data)
        memcpy(Buffer::data(), data, Buffer::length());

    extMemory(length);
}

v8::Local<v8::Object> Buffer::wrap(Isolate* isolate, v8::Local<v8::Object> This)
{
    if (!hasJSHandle()) {
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::ArrayBuffer> buf;

        if (m_buf.IsEmpty()) {
            buf = v8::ArrayBuffer::New(isolate->m_isolate, m_store);
            m_buf.Reset(isolate->m_isolate, buf);
        } else
            buf = m_buf.Get(isolate->m_isolate);

        v8::Local<v8::Object> proto;
        v8::Local<v8::Uint8Array> ui;
        if (isolate->m_buffer_prototype.IsEmpty()) {
            if (This.IsEmpty())
                This = Classinfo().CreateInstance(isolate);

            ui = v8::Uint8Array::New(buf, 0, m_store->ByteLength());

            proto = This->GetPrototype().As<v8::Object>();
            proto->SetPrototype(context, ui->GetPrototype()).IsJust();

            set_instance_type(proto, Buffer_base::class_info().getInstanceType());

            isolate->m_buffer_prototype.Reset(isolate->m_isolate, proto);
        } else {
            proto = isolate->m_buffer_prototype.Get(isolate->m_isolate);
            ui = v8::Uint8Array::New(buf, 0, m_store->ByteLength());
        }

        ui->SetPrototype(context, proto).IsJust();
        ui->SetAlignedPointerInInternalField(0, this);

        return object_base::wrap(isolate, ui);
    }

    return object_base::wrap(isolate);
}

inline bool is_native_codec(exlib::string codec)
{
    return (codec == "hex")
        || (codec == "base32") || (codec == "base58")
        || (codec == "base64") || (codec == "base64url")

        || (codec == "utf8") || (codec == "utf-8")

        || (codec == "ucs2") || (codec == "ucs-2")
        || (codec == "utf16") || (codec == "utf-16")

        || (codec == "ucs2le") || (codec == "ucs-2le")
        || (codec == "utf16le") || (codec == "utf-16le")

        || (codec == "ucs2be") || (codec == "ucs-2be")
        || (codec == "utf16be") || (codec == "utf-16be")

        || (codec == "ucs4") || (codec == "ucs-4")
        || (codec == "utf32") || (codec == "utf-32")

        || (codec == "ucs4le") || (codec == "ucs-4le")
        || (codec == "utf32le") || (codec == "utf-32le")

        || (codec == "ucs4be") || (codec == "ucs-4be")
        || (codec == "utf32be") || (codec == "utf-32be")

        || (codec == "binary") || (codec == "latin1");
}

inline bool static_is_safe_codec(exlib::string codec)
{
    Isolate* isolate = NULL;
    Runtime* rt = Runtime::current();
    if (rt)
        isolate = rt->safe_isolate();

    return (isolate && !isolate->m_safe_buffer) || is_native_codec(codec);
}

result_t Buffer_base::isEncoding(exlib::string codec, bool& retVal)
{
    if (!static_is_safe_codec(codec)) {
        retVal = false;
        return 0;
    }

    if ((codec == "utf8") || (codec == "utf-8") || (codec == "hex")
        || (codec == "base32") || (codec == "base58")
        || (codec == "base64") || (codec == "base64url")) {
        retVal = true;
    } else {
        iconv_base::isEncoding(codec, retVal);
    }
    return 0;
}

result_t Buffer_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return 0;
}

bool Buffer::is_safe_codec(exlib::string codec)
{
    Isolate* isolate = get_holder();
    if (!isolate)
        return static_is_safe_codec(codec);

    return !isolate->m_safe_buffer || is_native_codec(codec);
}

result_t Buffer_base::byteLength(exlib::string str, exlib::string codec, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, codec, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer_base::byteLength(v8::Local<v8::ArrayBuffer> str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, 0, -1, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer_base::byteLength(v8::Local<v8::Uint8Array> str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, 0, -1, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer_base::byteLength(Buffer_base* str, int32_t& retVal)
{
    obj_ptr<Buffer_base> buf;

    result_t hr = from(str, 0, -1, buf);
    if (hr < 0)
        return hr;

    retVal = (int32_t)Buffer::Cast(buf)->length();

    return 0;
}

result_t Buffer::get_length(int32_t& retVal)
{
    retVal = length();
    return 0;
}

result_t Buffer_base::compare(Buffer_base* buf1, Buffer_base* buf2, int32_t& retVal)
{
    return buf1->compare(buf2, retVal);
}

result_t Buffer::write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t max_length = 0;
    int32_t buffer_length = (int32_t)Buffer::length();

    if (offset < 0 || length < -1)
        return CHECK_ERROR(CALL_E_INVALIDARG);
    if (buffer_length < offset)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (buffer_length == offset || length == 0) {
        retVal = 0;
        return 0;
    }

    exlib::string strBuf;
    result_t hr = commonDecode(codec, str, strBuf);
    if (hr < 0)
        return hr;

    max_length = (int32_t)strBuf.length();
    max_length = MIN(max_length, buffer_length - offset);
    if (length > 0)
        max_length = MIN(max_length, length);

    memcpy(data() + offset, strBuf.c_str(), max_length);
    retVal = max_length;

    return 0;
}

result_t Buffer::write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal)
{
    return write(str, offset, -1, codec, retVal);
}

result_t Buffer::write(exlib::string str, exlib::string codec, int32_t& retVal)
{
    return write(str, 0, -1, codec, retVal);
}

result_t Buffer::fill(int32_t v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = generateEnd((int32_t)length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    memset(data() + offset, v & 255, end - offset);

    retVal = this;
    return 0;
}

result_t Buffer::fill(const uint8_t* buf, size_t sz, int32_t offset, int32_t end)
{
    if (sz == 0)
        return 0;

    result_t hr = generateEnd((int32_t)length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    int32_t length = end - offset;

    uint8_t* _data = data();
    while (length > 0) {
        memcpy(_data + offset, buf, MIN(sz, length));
        length -= sz;
        offset += sz;
    }
    return 0;
}

result_t Buffer::fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer> v_data = Buffer::Cast(v);
    result_t hr = fill(v_data->data(), v_data->length(), offset, end);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t Buffer::fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    exlib::string strBuf;
    result_t hr = commonDecode(codec, v, strBuf);
    if (hr < 0)
        return hr;

    hr = fill((const uint8_t*)strBuf.c_str(), strBuf.length(), offset, end);
    if (hr < 0)
        return hr;

    retVal = this;
    return 0;
}

result_t Buffer::copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart,
    int32_t sourceEnd, int32_t& retVal)
{
    if (targetStart < 0 || sourceStart < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (sourceStart > (int32_t)length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    Buffer* buf = Buffer::Cast(targetBuffer);
    int32_t bufLen = buf->length();

    if (sourceEnd == -1)
        sourceEnd = (int32_t)length();

    if (targetStart >= bufLen || sourceStart >= sourceEnd) {
        retVal = 0;
        return 0;
    }

    int32_t targetSz = bufLen - targetStart;
    int32_t sourceSz = (int32_t)length() - sourceStart;
    int32_t sourceLen = sourceEnd - sourceStart;
    int32_t sz = MIN(MIN(sourceLen, targetSz), sourceSz);

    memmove(buf->data() + targetStart, data() + sourceStart, sz);

    retVal = sz;

    return 0;
}

result_t Buffer::set(Buffer_base* src, int32_t start, int32_t& retVal)
{
    Buffer* buf = Buffer::Cast(src);
    int32_t len = buf->length();
    if (len + start > (int32_t)length())
        return CHECK_ERROR(Runtime::setError("Buffer: Source is too large."));

    return src->copy(this, start, 0, -1, retVal);
}

result_t Buffer::readNumber(int32_t offset, char* buf, int32_t size,
    int32_t value_size, bool le)
{
    size_t data_len = length();

    if (offset < 0 || (size_t)offset > data_len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (offset + size > (int32_t)data_len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    const char* _data = (const char*)data() + offset;

    if (size == 1) {
        buf[0] = *_data;
        return 0;
    }

    if (is_big_endian()) {
        if (!le)
            memcpy(buf + value_size - size, _data, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                buf[value_size - i - 1] = _data[i];
        }
    } else {
        if (le)
            memcpy(buf, _data, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                buf[size - i - 1] = _data[i];
        }
    }

    return 0;
}

#define READ_NUMBER(t, le)                                                 \
    t v = 0;                                                               \
    result_t hr = readNumber(offset, (char*)&v, sizeof(v), sizeof(v), le); \
    if (hr < 0)                                                            \
        return hr;                                                         \
    retVal = v;                                                            \
    return 0;

#define READ_U_NUMBER_48(t, sz, le)                                 \
    if (sz <= 0 || sz > 6)                                          \
        return CALL_E_INVALIDARG;                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr < 0)                                                     \
        return hr;                                                  \
    retVal = v;                                                     \
    return 0;

#define READ_NUMBER_48(t, sz, le)                                   \
    if (sz <= 0 || sz > 6)                                          \
        return CALL_E_INVALIDARG;                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr < 0)                                                     \
        return hr;                                                  \
    if ((sz < 8) && (v & (((t)1) << (8 * sz - 1))))                 \
        v |= ((t)-1) << (8 * sz);                                   \
    retVal = v;                                                     \
    return 0;

result_t Buffer::readUInt8(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(uint8_t, true);
}

result_t Buffer::readUInt16LE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(uint16_t, true);
}

result_t Buffer::readUInt16BE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(uint16_t, false);
}

result_t Buffer::readUInt32LE(int32_t offset, double& retVal)
{
    READ_NUMBER(uint32_t, true);
}

result_t Buffer::readUInt32BE(int32_t offset, double& retVal)
{
    READ_NUMBER(uint32_t, false);
}

result_t Buffer::readUIntLE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_U_NUMBER_48(uint64_t, byteLength, true);
}

result_t Buffer::readUIntBE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_U_NUMBER_48(uint64_t, byteLength, false);
}

result_t Buffer::readInt8(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int8_t, true);
}

result_t Buffer::readInt16LE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int16_t, true);
}

result_t Buffer::readInt16BE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int16_t, false);
}

result_t Buffer::readInt32LE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int32_t, true);
}

result_t Buffer::readInt32BE(int32_t offset, int32_t& retVal)
{
    READ_NUMBER(int32_t, false);
}

result_t Buffer::readIntLE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_NUMBER_48(int64_t, byteLength, true);
}

result_t Buffer::readIntBE(int32_t offset, int32_t byteLength, double& retVal)
{
    READ_NUMBER_48(int64_t, byteLength, false);
}

result_t Buffer::readInt64LE(int32_t offset, int64_t& retVal)
{
    READ_NUMBER(int64_t, true);
}

result_t Buffer::readInt64BE(int32_t offset, int64_t& retVal)
{
    READ_NUMBER(int64_t, false);
}

result_t Buffer::readFloatLE(int32_t offset, double& retVal)
{
    READ_NUMBER(float, true);
}

result_t Buffer::readFloatBE(int32_t offset, double& retVal)
{
    READ_NUMBER(float, false);
}

result_t Buffer::readDoubleLE(int32_t offset, double& retVal)
{
    READ_NUMBER(double, true);
}

result_t Buffer::readDoubleBE(int32_t offset, double& retVal)
{
    READ_NUMBER(double, false);
}

result_t Buffer::writeNumber(int32_t offset, const char* buf, int32_t size,
    int32_t value_size, bool le, int32_t& retVal)
{
    if (offset < 0 || (size_t)offset > length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (offset + size > (int32_t)length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    retVal = size + offset;

    char* _data = (char*)data() + offset;

    if (size == 1) {
        _data[0] = buf[0];
        return 0;
    }

    if (is_big_endian()) {
        if (!le)
            memcpy(_data, buf + value_size - size, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                _data[i] = buf[value_size - i - 1];
        }
    } else {
        if (le)
            memcpy(_data, buf, size);
        else {
            int32_t i;
            for (i = 0; i < size; i++)
                _data[i] = buf[size - i - 1];
        }
    }

    return 0;
}

#define WRITE_NUMBER(t, le)                                                         \
    t v = (t)value;                                                                 \
    result_t hr = writeNumber(offset, (char*)&v, sizeof(v), sizeof(v), le, retVal); \
    if (hr < 0)                                                                     \
        return hr;                                                                  \
    return 0;

#define WRITE_NUMBER_48(t, sz, le)                                           \
    if (sz <= 0 || sz > 6)                                                   \
        return CALL_E_INVALIDARG;                                            \
    t v = (t)value;                                                          \
    result_t hr = writeNumber(offset, (char*)&v, sz, sizeof(v), le, retVal); \
    if (hr < 0)                                                              \
        return hr;                                                           \
    return 0;

result_t Buffer::writeUInt8(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint8_t, true);
}

result_t Buffer::writeUInt16LE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint16_t, true);
}

result_t Buffer::writeUInt16BE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint16_t, false);
}

result_t Buffer::writeUInt32LE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint32_t, true);
}

result_t Buffer::writeUInt32BE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(uint32_t, false);
}

result_t Buffer::writeUIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(uint64_t, byteLength, true);
}

result_t Buffer::writeUIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(uint64_t, byteLength, false);
}

result_t Buffer::writeInt8(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int8_t, true);
}

result_t Buffer::writeInt16LE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int16_t, true);
}

result_t Buffer::writeInt16BE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int16_t, false);
}

result_t Buffer::writeInt32LE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int32_t, true);
}

result_t Buffer::writeInt32BE(int32_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int32_t, false);
}

result_t Buffer::writeIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(int64_t, byteLength, true);
}

result_t Buffer::writeIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal)
{
    WRITE_NUMBER_48(int64_t, byteLength, false);
}

result_t Buffer::writeInt64LE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int64_t, true);
}

result_t Buffer::writeInt64BE(int64_t value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(int64_t, false);
}

result_t Buffer::writeFloatLE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(float, true);
}

result_t Buffer::writeFloatBE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(float, false);
}

result_t Buffer::writeDoubleLE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(double, true);
}

result_t Buffer::writeDoubleBE(double value, int32_t offset, int32_t& retVal)
{
    WRITE_NUMBER(double, false);
}

result_t Buffer::indexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)length();
    result_t hr = validOffset(buf_length, offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const uint8_t* _data = data();

    while (offset < buf_length) {
        if (_data[offset] == (v & 255)) {
            retVal = offset;
            return 0;
        }
        offset++;
    }

    retVal = -1;
    return 0;
}

result_t Buffer::indexOf(Buffer_base* v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    obj_ptr<Buffer> v_data = Buffer::Cast(v);
    exlib::string vstr;
    v_data->toString(vstr);

    const uint8_t* find = exlib::qmemmem(data() + offset, length() - offset,
        (const uint8_t*)vstr.c_str(), vstr.length());

    retVal = find ? (int32_t)(find - data()) : -1;
    return 0;
}

result_t Buffer::indexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const uint8_t* find = exlib::qmemmem(data() + offset, length() - offset,
        (const uint8_t*)v.c_str(), v.length());

    retVal = find ? (int32_t)(find - data()) : -1;
    return 0;
}

result_t Buffer::slice(int32_t start, obj_ptr<Buffer_base>& retVal)
{
    return slice(start, (int32_t)length(), retVal);
}

result_t Buffer::slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    int32_t length = (int32_t)Buffer::length();

    if (start < 0)
        start = length + start;

    if (end < 0)
        end = length + end;

    if (start < 0)
        start = 0;

    if (end > length)
        end = length;

    if (start > end)
        start = end;

    if (start < end)
        retVal = new Buffer(data() + start, end - start);
    else
        retVal = new Buffer(NULL, 0);

    return 0;
}

result_t Buffer::equals(object_base* expected, bool& retVal)
{
    obj_ptr<Buffer> buf = Buffer::getInstance(expected);
    if (!buf) {
        retVal = false;
        return 0;
    }

    if (length() != buf->length()) {
        retVal = false;
        return 0;
    }

    retVal = !memcmp(data(), buf->data(), length());
    return 0;
}

result_t Buffer::compare(Buffer_base* buf, int32_t& retVal)
{
    obj_ptr<Buffer> cmpdata = Buffer::Cast(buf);
    int32_t pos_length = (int32_t)length();
    int32_t neg_length = (int32_t)cmpdata->length();

    retVal = memcmp(data(), cmpdata->data(), MIN(pos_length, neg_length));
    if (retVal)
        return 0;

    retVal = pos_length - neg_length;
    return 0;
}

result_t Buffer::toString(exlib::string& retVal)
{
    retVal.assign((const char*)data(), length());
    return 0;
}

result_t Buffer::toString(exlib::string codec, int32_t offset, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t sz = (int32_t)length();

    if (offset < 0)
        offset = 0;

    if (offset >= sz) {
        retVal = "";
        return 0;
    }

    if (offset > 0) {
        return commonEncode(codec, (const char*)data() + offset, sz - offset, retVal);
    } else {
        return commonEncode(codec, (const char*)data(), sz, retVal);
    }
}

result_t Buffer::toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t sz = (int32_t)length();

    if (offset < 0)
        offset = 0;

    if (end < 0 || offset >= end) {
        retVal = "";
        return 0;
    }

    if (end > sz)
        end = sz;

    if (offset < end) {
        return commonEncode(codec, (const char*)data() + offset, end - offset, retVal);
    } else {
        return commonEncode(codec, (const char*)data(), sz, retVal);
    }
}

result_t Buffer::toArray(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate, (int32_t)length());
    int32_t i;
    const uint8_t* _data = data();

    for (i = 0; i < (int32_t)length(); i++)
        a->Set(context, i, v8::Number::New(isolate->m_isolate, _data[i])).IsJust();

    retVal = a;

    return 0;
}

result_t Buffer::hex(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return hex_base::encode(buf, retVal);
}

result_t Buffer::base32(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return base32_base::encode(buf, retVal);
}

result_t Buffer::base58(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return base58_base::encode(buf, retVal);
}

result_t Buffer::base64(exlib::string& retVal)
{
    obj_ptr<Buffer_base> buf = this;
    return base64_base::encode(buf, false, retVal);
}

result_t Buffer::unbind(obj_ptr<object_base>& retVal)
{
    retVal = new Buffer(this);
    return 0;
}

}
