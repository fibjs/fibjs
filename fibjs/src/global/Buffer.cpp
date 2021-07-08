#include "object.h"
#include "Buffer.h"
#include "StringBuffer.h"
#include "encoding_iconv.h"
#include "encoding.h"
#include "utf8.h"
#include <cstring>
#include <string>
#include "Iterator.h"

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

result_t Buffer_base::_new(exlib::string str, exlib::string codec,
    obj_ptr<Buffer_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    return retVal->append(str, codec);
}

result_t Buffer_base::_new(int32_t size, obj_ptr<Buffer_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    retVal->wrap(This);
    return retVal->resize(size);
}

result_t Buffer_base::_new(v8::Local<v8::Array> datas,
    obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Buffer> buf;
    retVal = buf = new Buffer();
    return buf->_append(datas);
}

result_t Buffer_base::_new(v8::Local<v8::ArrayBuffer> datas,
    obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    v8::ArrayBuffer::Contents cnt = datas->GetContents();
    int32_t sz = (int32_t)cnt.ByteLength();
    char* ptr = (char*)cnt.Data();

    retVal = new Buffer(ptr, sz);
    return 0;
}

result_t Buffer_base::_new(v8::Local<v8::TypedArray> datas,
    obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    if (datas->IsUint8Array() || datas->IsInt8Array()) {
        v8::Local<v8::ArrayBufferView> datas1 = datas;
        return _new(datas1, retVal, This);
    }

    obj_ptr<Buffer> buf;
    retVal = buf = new Buffer();
    return buf->_append(datas);
}

result_t Buffer_base::_new(v8::Local<v8::ArrayBufferView> datas,
    obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    exlib::string str;
    str.resize(datas->ByteLength());
    datas->CopyContents(str.c_buffer(), str.length());
    retVal = new Buffer(str);
    return 0;
}

result_t Buffer_base::_new(Buffer_base* buffer,
    obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This)
{
    exlib::string str;
    buffer->toString(str);
    retVal = new Buffer(str);
    return 0;
}

result_t Buffer_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return 0;
}

result_t Buffer_base::from(Buffer_base* buffer, int32_t byteOffset, int32_t length,
    obj_ptr<Buffer_base>& retVal)
{
    exlib::string str;
    buffer->toString(str);
    return from(str, byteOffset, length, retVal);
}

result_t Buffer_base::from(exlib::string str, int32_t byteOffset, int32_t length,
    obj_ptr<Buffer_base>& retVal)
{
    if (byteOffset < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (byteOffset > (int32_t)str.length())
        byteOffset = (int32_t)str.length();

    if (length < 0)
        length = (int32_t)str.length() - byteOffset;

    retVal = new Buffer(str.substr(byteOffset, length));
    return 0;
}

result_t Buffer_base::from(exlib::string str, exlib::string codec,
    obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer();
    return retVal->append(str, codec);
}

result_t Buffer_base::alloc(int32_t size, int32_t fill,
    exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer();
    if (size <= 0) {
        retVal = buf;
        return 0;
    }
    buf->resize(size);
    buf->fill(fill, 0, size, retVal);
    retVal = buf;
    return 0;
}

result_t Buffer_base::alloc(int32_t size, exlib::string fill,
    exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer();
    if (size <= 0) {
        retVal = buf;
        return 0;
    }
    buf->resize(size);
    obj_ptr<Buffer_base> tmp = new Buffer();
    tmp->append(fill, codec);
    buf->fill(tmp, 0, size, retVal);
    return 0;
}

result_t Buffer_base::alloc(int32_t size, Buffer_base* fill,
    exlib::string codec, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer();
    if (size <= 0) {
        retVal = buf;
        return 0;
    }
    buf->resize(size);
    buf->fill(fill, 0, size, retVal);
    return 0;
}

result_t Buffer_base::allocUnsafe(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer();
    if (size <= 0) {
        retVal = buf;
        return 0;
    }
    buf->resize(size);
    retVal = buf;
    return 0;
}

result_t Buffer_base::allocUnsafeSlow(int32_t size, obj_ptr<Buffer_base>& retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer();
    if (size <= 0) {
        retVal = buf;
        return 0;
    }
    buf->resize(size);
    retVal = buf;
    return 0;
}

result_t Buffer_base::byteLength(exlib::string str, exlib::string codec,
    int32_t& retVal)
{
    obj_ptr<Buffer_base> buf = new Buffer();
    buf->append(str, codec);
    return buf->get_length(retVal);
}

result_t Buffer_base::byteLength(v8::Local<v8::ArrayBuffer> str,
    exlib::string codec, int32_t& retVal)
{
    retVal = (int32_t)str->ByteLength();
    return 0;
}

result_t Buffer_base::byteLength(v8::Local<v8::ArrayBufferView> str,
    exlib::string codec, int32_t& retVal)
{
    retVal = (int32_t)str->ByteLength();
    return 0;
}

result_t Buffer_base::byteLength(Buffer_base* str,
    exlib::string codec, int32_t& retVal)
{
    return str->get_length(retVal);
}

result_t Buffer_base::concat(v8::Local<v8::Array> buflist, int32_t cutLength, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = 0;
    int32_t buf_length;
    int32_t offset = 0;
    int32_t total_length = cutLength;
    int32_t sz = buflist->Length();

    if (!sz) {
        retVal = new Buffer();
        return 0;
    }
    if (cutLength < -1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (cutLength == -1)
        total_length = (int32_t)INT32_MAX;

    StringBuffer sb;

    Isolate* isolate = Isolate::current();
   
	for (int32_t i = 0; i < sz; i++) {
        JSValue v = buflist->Get(i);
		obj_ptr<Buffer_base> vdata;
        hr = GetArgumentValue(isolate->m_isolate, v, vdata);
        if (hr < 0)
            return CHECK_ERROR(hr);
        
		exlib::string vstr;
        vdata->toString(vstr);
        buf_length = (int32_t)vstr.length();
        
        offset += buf_length;
        if (offset <= total_length) {
            sb.append(vstr);
        } else {
            sb.append(vstr.c_str(), buf_length - (offset - total_length));
			break;
        }
    }
 
	retVal = new Buffer(sb.str());

    return hr;
}

inline bool is_native_codec(exlib::string codec)
{
    return (codec == "hex") || (codec == "base64")
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
        || (codec == "utf32be") || (codec == "utf-32be");
}

inline bool static_is_safe_codec(exlib::string codec)
{
    return !Isolate::current()->m_safe_buffer || is_native_codec(codec);
}

result_t Buffer_base::isEncoding(exlib::string codec, bool& retVal)
{
    if (!static_is_safe_codec(codec)) {
        retVal = false;
        return 0;
    }

    if ((codec == "utf8") || (codec == "utf-8") || (codec == "hex") || (codec == "base64")) {
        retVal = true;
    } else {
        iconv_base::isEncoding(codec, retVal);
    }
    return 0;
}

bool Buffer::is_safe_codec(exlib::string codec)
{
    return !holder()->m_safe_buffer || is_native_codec(codec);
}

result_t Buffer::_indexed_getter(uint32_t index, int32_t& retVal)
{
    if (index >= m_data.length())
        return CALL_RETURN_NULL;

    retVal = (unsigned char)m_data.c_str()[index];
    return 0;
}

result_t Buffer::_indexed_setter(uint32_t index, int32_t newVal)
{
    if (index >= m_data.length())
        return 0;

    if (newVal < 0)
        newVal = 256 + (newVal % 256);
    else if (newVal > 255)
        newVal = newVal % 256;

    m_data.c_buffer()[index] = newVal;
    return 0;
}

result_t Buffer::symbol_iterator(obj_ptr<Iterator_base>& retVal)
{
    return values(retVal);
}

result_t Buffer::get_length(int32_t& retVal)
{
    retVal = (int32_t)m_data.length();
    return 0;
}

result_t Buffer::resize(int32_t sz)
{
    if (sz < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t sz1 = (int32_t)m_data.length();
    extMemory(sz - sz1);

    m_data.resize(sz);

    Isolate* isolate = get_holder();
    if (!isolate)
        return 0;

    if (sz > sz1 && isolate->m_safe_buffer)
        memset(m_data.c_buffer() + sz1, 0, sz - sz1);

    return 0;
}

result_t Buffer::append(Buffer_base* data)
{
    exlib::string strBuf;
    data->toString(strBuf);

    extMemory((int32_t)strBuf.length());
    m_data.append(strBuf);
    return 0;
}

result_t Buffer::append(exlib::string str, exlib::string codec)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    if ((codec == "utf8") || (codec == "utf-8")) {
        extMemory((int32_t)str.length());
        m_data.append(str);
        return 0;
    }

    obj_ptr<Buffer_base> data;
    result_t hr;

    if ((codec == "hex"))
        hr = hex_base::decode(str, data);
    else if ((codec == "base64"))
        hr = base64_base::decode(str, data);
    else
        hr = iconv_base::encode(codec, str, data);

    if (hr < 0)
        return hr;

    return append(data);
}

result_t Buffer::write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    int32_t max_length = 0;
    int32_t buffer_length = (int32_t)m_data.length();

    if (offset < 0 || length < -1)
        return CHECK_ERROR(CALL_E_INVALIDARG);
    if (buffer_length < offset)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (buffer_length == offset || length == 0) {
        retVal = 0;
        return 0;
    }

    result_t hr;
    exlib::string strBuf;

    if ((codec == "utf8") || (codec == "utf-8"))
        strBuf = str;
    else {
        obj_ptr<Buffer_base> data;

        if ((codec == "hex"))
            hr = hex_base::decode(str, data);
        else if ((codec == "base64"))
            hr = base64_base::decode(str, data);
        else
            hr = iconv_base::encode(codec, str, data);

        if (hr < 0)
            return hr;
        data->toString(strBuf);
    }

    max_length = (int32_t)strBuf.length();
    max_length = MIN(max_length, buffer_length - offset);
    if (length > 0)
        max_length = MIN(max_length, length);

    retVal = max_length;
    memcpy(m_data.c_buffer() + offset, strBuf.c_str(), max_length);
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
    result_t hr = generateEnd((int32_t)m_data.length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    memset(m_data.c_buffer() + offset, v & 255, end - offset);

    retVal = this;
    return 0;
}

result_t Buffer::fill(exlib::string v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = generateEnd((int32_t)m_data.length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    int32_t length = end - offset;
    int32_t str_length = (int32_t)v.length();

    if (str_length == 0) {
        retVal = this;
        return 0;
    }

    char* _data = m_data.c_buffer();
    while (length > 0) {
        memcpy(_data + offset, v.c_str(), MIN(str_length, length));
        length -= str_length;
        offset += str_length;
    }

    retVal = this;
    return 0;
}

result_t Buffer::fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = generateEnd((int32_t)m_data.length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    obj_ptr<Buffer> v_data = static_cast<Buffer*>(v);
    int32_t length = end - offset;
    int32_t v_length = (int32_t)v_data->m_data.length();

    if (v_length == 0) {
        retVal = this;
        return 0;
    }

    char* _data = m_data.c_buffer();
    while (length > 0) {
        memcpy(_data + offset, v_data->m_data.c_str(), MIN(v_length, length));
        length -= v_length;
        offset += v_length;
    }

    retVal = this;
    return 0;
}

result_t Buffer::indexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)m_data.length();
    result_t hr = validOffset(buf_length, offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const char* _data = m_data.c_str();

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
    result_t hr = validOffset((int32_t)m_data.length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    obj_ptr<Buffer> v_data = static_cast<Buffer*>(v);
    exlib::string vstr;
    v_data->toString(vstr);

    const char* find = exlib::qmemmem(m_data.c_str() + offset, m_data.length() - offset,
        vstr.c_str(), vstr.length());

    retVal = find ? (int32_t)(find - m_data.c_str()) : -1;
    return 0;
}

result_t Buffer::indexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)m_data.length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const char* find = exlib::qmemmem(m_data.c_str() + offset, m_data.length() - offset,
        v.c_str(), v.length());

    retVal = find ? (int32_t)(find - m_data.c_str()) : -1;
    return 0;
}

result_t Buffer_base::compare(Buffer_base* buf1, Buffer_base* buf2, int32_t& retVal)
{
    return buf1->compare(buf2, retVal);
}

result_t Buffer::compare(Buffer_base* buf, int32_t& retVal)
{
    obj_ptr<Buffer> cmpdata = static_cast<Buffer*>(buf);
    int32_t pos_length = (int32_t)m_data.length();
    int32_t neg_length = (int32_t)cmpdata->m_data.length();

    retVal = memcmp(m_data.c_buffer(), cmpdata->m_data.c_str(), MIN(pos_length, neg_length));
    if (retVal)
        return 0;

    retVal = pos_length - neg_length;
    return 0;
}

result_t Buffer::copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart,
    int32_t sourceEnd, int32_t& retVal)
{
    if (targetStart < 0 || sourceStart < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (sourceStart > (int32_t)m_data.length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    Buffer* buf = static_cast<Buffer*>(targetBuffer);
    int32_t bufLen;
    buf->get_length(bufLen);

    if (sourceEnd == -1)
        sourceEnd = (int32_t)m_data.length();

    if (targetStart >= bufLen || sourceStart >= sourceEnd) {
        retVal = 0;
        return 0;
    }

    int32_t targetSz = bufLen - targetStart;
    int32_t sourceSz = (int32_t)m_data.length() - sourceStart;
    int32_t sourceLen = sourceEnd - sourceStart;
    int32_t sz = MIN(MIN(sourceLen, targetSz), sourceSz);

    memmove(buf->m_data.c_buffer() + targetStart, m_data.c_str() + sourceStart, sz);

    retVal = sz;

    return 0;
}

result_t Buffer::set(Buffer_base* src, int32_t start, int32_t& retVal)
{
    int32_t len;

    src->get_length(len);
    if (len + start > (int32_t)m_data.length())
        return CHECK_ERROR(Runtime::setError("Buffer: Source is too large."));

    return src->copy(this, start, 0, -1, retVal);
}

result_t Buffer::readNumber(int32_t offset, char* buf, int32_t size,
    int32_t value_size, bool le)
{
    size_t data_len = m_data.length();

    if (offset < 0 || (size_t)offset > data_len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (offset + size > (int32_t)data_len)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    const char* _data = m_data.c_str() + offset;

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
    if (sz <= 0 || sz > 8)                                          \
        return CALL_E_INVALIDARG;                                   \
    t v = 0;                                                        \
    result_t hr = readNumber(offset, (char*)&v, sz, sizeof(v), le); \
    if (hr < 0)                                                     \
        return hr;                                                  \
    retVal = v;                                                     \
    return 0;

#define READ_NUMBER_48(t, sz, le)                                   \
    if (sz <= 0 || sz > 8)                                          \
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

result_t Buffer::readUInt32LE(int32_t offset, int64_t& retVal)
{
    READ_NUMBER(uint32_t, true);
}

result_t Buffer::readUInt32BE(int32_t offset, int64_t& retVal)
{
    READ_NUMBER(uint32_t, false);
}

result_t Buffer::readUIntLE(int32_t offset, int32_t byteLength, int64_t& retVal)
{
    READ_U_NUMBER_48(uint64_t, byteLength, true);
}

result_t Buffer::readUIntBE(int32_t offset, int32_t byteLength, int64_t& retVal)
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

result_t Buffer::readIntLE(int32_t offset, int32_t byteLength, int64_t& retVal)
{
    READ_NUMBER_48(int64_t, byteLength, true);
}

result_t Buffer::readIntBE(int32_t offset, int32_t byteLength, int64_t& retVal)
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
    if (offset < 0 || (size_t)offset > m_data.length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    if (offset + size > (int32_t)m_data.length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    retVal = size + offset;

    char* _data = m_data.c_buffer() + offset;

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
    if (sz <= 0 || sz > 8)                                                   \
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

result_t Buffer::slice(int32_t start, obj_ptr<Buffer_base>& retVal)
{
    return slice(start, (int32_t)m_data.length(), retVal);
}

result_t Buffer::slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    int32_t length = (int32_t)m_data.length();

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

    obj_ptr<Buffer> pNew = new Buffer();
    if (start < end) {
        pNew->m_data.append(m_data.c_str() + start, end - start);
        pNew->extMemory((int32_t)(end - start));
    }
    retVal = pNew;

    return 0;
}

result_t Buffer::join(exlib::string separator, exlib::string& retVal)
{
    StringBuffer sb;
    int32_t length = (int32_t)m_data.length();
    const char* c_str = m_data.c_str();
    char buf[32];

    for (int32_t i = 0; i < length; i++) {
        sprintf(buf, "%d", (unsigned char)c_str[i]);
        if (i > 0)
            sb.append(separator);
        sb.append(buf);
    }

    retVal = sb.str();

    return 0;
}

result_t Buffer::reverse(obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    toString(strBuf);

    char* buf = strBuf.c_buffer();
    int32_t len = (int32_t)strBuf.length();
    for (int32_t i = 0; i < len / 2; i++) {
        char ch = buf[i];
        buf[i] = buf[len - i - 1];
        buf[len - i - 1] = ch;
    }

    retVal = new Buffer(strBuf);

    return 0;
}

result_t Buffer::hex(exlib::string& retVal)
{
    obj_ptr<Buffer_base> data = this;
    return hex_base::encode(data, retVal);
}

result_t Buffer::base64(exlib::string& retVal)
{
    obj_ptr<Buffer_base> data = this;
    return base64_base::encode(data, false, retVal);
}

result_t Buffer::equals(object_base* expected, bool& retVal)
{
    obj_ptr<Buffer_base> buf = Buffer_base::getInstance(expected);
    if (!buf) {
        retVal = false;
        return 0;
    }

    exlib::string str;
    buf->toString(str);

    retVal = (m_data.length() == str.length()) && !memcmp(m_data.c_str(), str.c_str(), str.length());

    return 0;
}

result_t Buffer::keys(obj_ptr<Iterator_base>& retVal)
{
    retVal = new Iterator(this, [&](size_t index, v8::Local<v8::Value>& retVal) {
        if (index < m_data.length())
            retVal = v8::Number::New(holder()->m_isolate, (double)index);
    });
    return 0;
}

result_t Buffer::values(obj_ptr<Iterator_base>& retVal)
{
    retVal = new Iterator(this, [&](size_t index, v8::Local<v8::Value>& retVal) {
        if (index < m_data.length())
            retVal = v8::Number::New(holder()->m_isolate, (unsigned char)m_data[index]);
    });
    return 0;
}

result_t Buffer::entries(obj_ptr<Iterator_base>& retVal)
{
    retVal = new Iterator(this, [&](size_t index, v8::Local<v8::Value>& retVal) {
        if (index < m_data.length()) {
            Isolate* isolate = holder();
            v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate, 2);
            arr1->Set(0, v8::Number::New(isolate->m_isolate, (double)index));
            arr1->Set(1, v8::Number::New(isolate->m_isolate, (unsigned char)m_data[index]));
            retVal = arr1;
        }
    });
    return 0;
}

result_t Buffer::toString(exlib::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t Buffer::toString(exlib::string codec, int32_t offset, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    exlib::string str;
    int32_t length = (int32_t)m_data.length();

    if (offset < 0)
        offset = 0;

    if (offset >= length) {
        retVal = "";
        return 0;
    }

    if (offset > 0) {
        str.append(m_data.c_str() + offset, length - offset);
        return commonEncode(codec, str, retVal);
    } else {
        return commonEncode(codec, m_data, retVal);
    }
}

result_t Buffer::toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal)
{
    if (!is_safe_codec(codec))
        return CHECK_ERROR(Runtime::setError("Buffer: Unknown codec."));

    exlib::string str;
    int32_t length = (int32_t)m_data.length();

    if (offset < 0)
        offset = 0;

    if (end < 0 || offset >= end) {
        retVal = "";
        return 0;
    }

    if (end > length)
        end = length;

    if (offset < end) {
        str.append(m_data.c_str() + offset, end - offset);
        return commonEncode(codec, str, retVal);
    } else {
        return commonEncode(codec, m_data, retVal);
    }
}

result_t Buffer::toArray(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate, (int32_t)m_data.length());
    int32_t i;
    const char* _data = m_data.c_str();

    for (i = 0; i < (int32_t)m_data.length(); i++)
        a->Set(i, v8::Number::New(isolate->m_isolate, (unsigned char)_data[i]));

    retVal = a;

    return 0;
}

result_t Buffer::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate, (int32_t)m_data.length());
    int32_t i;
    const char* _data = m_data.c_str();

    for (i = 0; i < (int32_t)m_data.length(); i++)
        a->Set(i, v8::Number::New(isolate->m_isolate, (unsigned char)_data[i]));

    o->Set(isolate->NewString("type"), isolate->NewString("Buffer"));
    o->Set(isolate->NewString("data"), a);

    retVal = o;

    return 0;
}

result_t Buffer::fromJSON(Isolate* isolate, v8::Local<v8::Value> data, v8::Local<v8::Value>& o)
{
    result_t hr;
    v8::Local<v8::Array> arr;
    exlib::string str;

    hr = GetArgumentValue(isolate->m_isolate, data, arr, true);
    if (hr >= 0) {
        obj_ptr<Buffer> buf = new Buffer();

        hr = buf->_append(arr);
        if (hr < 0)
            return hr;
        o = buf->wrap();

        return 0;
    }

    hr = GetArgumentValue(isolate->m_isolate, data, str, true);
    if (hr >= 0) {
        obj_ptr<Buffer_base> buf;

        hr = base64_base::decode(str, buf);
        if (hr < 0)
            return hr;
        o = buf->wrap();

        return 0;
    }

    return CALL_E_TYPEMISMATCH;
}

result_t Buffer::unbind(obj_ptr<object_base>& retVal)
{
    retVal = new Buffer(m_data);
    return 0;
}
}
