#include "object.h"
#include "Buffer.h"
#include "encoding_iconv.h"
#include "encoding.h"
#include "Int64.h"
#include "utf8.h"
#include <cstring>
#include <string>

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

    if (!sz)
        return 0;
    if (cutLength < -1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    exlib::string str;
    Isolate* isolate = Isolate::current();

    for (int32_t i = 0; i < sz; i++) {
        v8::Local<v8::Value> v = buflist->Get(i);
        obj_ptr<Buffer_base> vdata;

        hr = GetArgumentValue(isolate->m_isolate, v, vdata);
        if (hr < 0)
            return CHECK_ERROR(hr);

        exlib::string vstr;
        vdata->toString(vstr);
        buf_length = (int32_t)vstr.length();

        if (-1 == cutLength)
            total_length = offset + buf_length;

        if (offset + buf_length <= total_length) {
            str.append(vstr.c_str(), buf_length);
            offset += buf_length;
        } else {
            str.append(vstr.c_str(), total_length - offset);
            offset = total_length;
            break;
        }
    }
    obj_ptr<Buffer> bNew = new Buffer(str);
    bNew->extMemory(offset);
    retVal = bNew;
    return hr;
}

result_t Buffer_base::isEncoding(exlib::string codec, bool& retVal)
{
    if ((codec == "utf8") || (codec == "utf-8") || (codec == "hex") || (codec == "base64")) {
        retVal = true;
    } else {
        iconv_base::isEncoding(codec, retVal);
    }
    return 0;
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

    m_data[index] = newVal;
    return 0;
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

    extMemory(sz - (int32_t)m_data.length());
    m_data.resize(sz);

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
    memcpy(&m_data[offset], strBuf.c_str(), max_length);
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

    memset(&m_data[offset], v & 255, end - offset);

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
    while (length > 0) {
        memcpy(&m_data[offset], v.c_str(), MIN(str_length, length));
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

    obj_ptr<Buffer> v_data = dynamic_cast<Buffer*>(v);
    int32_t length = end - offset;
    int32_t v_length = (int32_t)v_data->m_data.length();

    if (v_length == 0) {
        retVal = this;
        return 0;
    }

    while (length > 0) {
        memcpy(&m_data[offset], v_data->m_data.c_str(), MIN(v_length, length));
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

    obj_ptr<Buffer> v_data = dynamic_cast<Buffer*>(v);
    exlib::string vstr;
    v_data->toString(vstr);

    const char* find = exlib::qmemfind(m_data.c_str() + offset, m_data.length() - offset,
        vstr.c_str(), vstr.length());

    retVal = find ? (int32_t)(find - m_data.c_str()) : -1;
    return 0;
}

result_t Buffer::indexOf(exlib::string v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)m_data.length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    const char* find = exlib::qmemfind(m_data.c_str() + offset, m_data.length() - offset,
        v.c_str(), v.length());

    retVal = find ? (int32_t)(find - m_data.c_str()) : -1;
    return 0;
}

result_t Buffer::compare(Buffer_base* buf, int32_t& retVal)
{
    obj_ptr<Buffer> cmpdata = dynamic_cast<Buffer*>(buf);
    int32_t pos_length = (int32_t)m_data.length();
    int32_t neg_length = (int32_t)cmpdata->m_data.length();

    retVal = memcmp(&m_data[0], cmpdata->m_data.c_str(), MIN(pos_length, neg_length));
    if (retVal)
        return 0;

    retVal = pos_length - neg_length;
    return 0;
}

result_t Buffer::copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart,
    int32_t sourceEnd, int32_t& retVal)
{
    if (targetStart < 0 || sourceStart < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

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

    memcpy(&buf->m_data[targetStart], m_data.c_str() + sourceStart, sz);

    retVal = sz;

    return 0;
}

result_t Buffer::readNumber(int32_t offset, char* buf, int32_t size, bool noAssert, bool le)
{
    int32_t sz = size;

    if (offset + sz > (int32_t)m_data.length()) {
        if (!noAssert)
            return CHECK_ERROR(CALL_E_OUTRANGE);

        sz = (int32_t)m_data.length() - offset;
        if (sz <= 0)
            return 0;
    }

    if (size == 1) {
        buf[0] = *(m_data.c_str() + offset);
        return 0;
    }

    if (le)
        memcpy(buf, m_data.c_str() + offset, sz);
    else {
        int32_t i;
        for (i = 0; i < sz; i++)
            buf[size - i - 1] = *(m_data.c_str() + offset + i);
    }

    return 0;
}

#define READ_NUMBER(t, le)                                                \
    t v = 0;                                                              \
    result_t hr = readNumber(offset, (char*)&v, sizeof(v), noAssert, le); \
    if (hr < 0)                                                           \
        return hr;                                                        \
    retVal = v;                                                           \
    return 0;

#define READ_U_NUMBER_48(t, le)                                   \
    t v = 0;                                                      \
    result_t hr = readNumber(offset, (char*)&v, 6, noAssert, le); \
    if (hr < 0)                                                   \
        return hr;                                                \
    retVal = v;                                                   \
    return 0;

#define READ_NUMBER_48(t, le)                                     \
    t v = 0;                                                      \
    result_t hr = readNumber(offset, (char*)&v, 6, noAssert, le); \
    if (hr < 0)                                                   \
        return hr;                                                \
    if (v & 0x800000000000)                                       \
        v = -(v & 0x7fffffffffff);                                \
    retVal = v;                                                   \
    return 0;

result_t Buffer::readUInt8(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(uint8_t, true);
}

result_t Buffer::readUInt16LE(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(uint16_t, true);
}

result_t Buffer::readUInt16BE(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(uint16_t, false);
}

result_t Buffer::readUInt32LE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_NUMBER(uint32_t, true);
}

result_t Buffer::readUInt32BE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_NUMBER(uint32_t, false);
}

result_t Buffer::readUIntLE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_U_NUMBER_48(uint64_t, true);
}

result_t Buffer::readUIntBE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_U_NUMBER_48(uint64_t, false);
}

result_t Buffer::readInt8(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(int8_t, true);
}

result_t Buffer::readInt16LE(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(int16_t, true);
}

result_t Buffer::readInt16BE(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(int16_t, false);
}

result_t Buffer::readInt32LE(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(int32_t, true);
}

result_t Buffer::readInt32BE(int32_t offset, bool noAssert, int32_t& retVal)
{
    READ_NUMBER(int32_t, false);
}

result_t Buffer::readIntLE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_NUMBER_48(int64_t, true);
}

result_t Buffer::readIntBE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_NUMBER_48(int64_t, false);
}

result_t Buffer::readInt64LE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_NUMBER(int64_t, true);
}

result_t Buffer::readInt64BE(int32_t offset, bool noAssert, int64_t& retVal)
{
    READ_NUMBER(int64_t, false);
}

result_t Buffer::readInt64LE(int32_t offset, bool noAssert, obj_ptr<Int64_base>& retVal)
{
    int64_t v;
    result_t hr = readInt64LE(offset, noAssert, v);
    if (hr < 0)
        return hr;

    retVal = new Int64(v);
    return 0;
}

result_t Buffer::readInt64BE(int32_t offset, bool noAssert, obj_ptr<Int64_base>& retVal)
{
    int64_t v;
    result_t hr = readInt64BE(offset, noAssert, v);
    if (hr < 0)
        return hr;

    retVal = new Int64(v);
    return 0;
}

result_t Buffer::readFloatLE(int32_t offset, bool noAssert, double& retVal)
{
    READ_NUMBER(float, true);
}

result_t Buffer::readFloatBE(int32_t offset, bool noAssert, double& retVal)
{
    READ_NUMBER(float, false);
}

result_t Buffer::readDoubleLE(int32_t offset, bool noAssert, double& retVal)
{
    READ_NUMBER(double, true);
}

result_t Buffer::readDoubleBE(int32_t offset, bool noAssert, double& retVal)
{
    READ_NUMBER(double, false);
}

result_t Buffer::writeNumber(int32_t offset, const char* buf, int32_t size, bool noAssert,
    bool le, int32_t& retVal)
{
    int32_t sz = size;

    if (offset + sz > (int32_t)m_data.length()) {
        if (!noAssert)
            return CHECK_ERROR(CALL_E_OUTRANGE);

        sz = (int32_t)m_data.length() - offset;
        if (sz <= 0) {
            retVal = offset;
            return 0;
        }
    }

    retVal = sz + offset;

    if (size == 1) {
        m_data[offset] = buf[0];
        return 0;
    }

    if (le)
        memcpy(&m_data[offset], buf, sz);
    else {
        int32_t i;
        for (i = 0; i < sz; i++)
            m_data[offset + i] = buf[size - i - 1];
    }

    return 0;
}

#define WRITE_NUMBER(t, le)                                                        \
    t v = (t)value;                                                                \
    result_t hr = writeNumber(offset, (char*)&v, sizeof(v), noAssert, le, retVal); \
    if (hr < 0)                                                                    \
        return hr;                                                                 \
    return 0;

#define WRITE_NUMBER_48(t, le)                                             \
    t v = (t)value;                                                        \
    result_t hr = writeNumber(offset, (char*)&v, 6, noAssert, le, retVal); \
    if (hr < 0)                                                            \
        return hr;                                                         \
    return 0;

result_t Buffer::writeUInt8(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(uint8_t, true);
}

result_t Buffer::writeUInt16LE(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(uint16_t, true);
}

result_t Buffer::writeUInt16BE(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(uint16_t, false);
}

result_t Buffer::writeUInt32LE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(uint32_t, true);
}

result_t Buffer::writeUInt32BE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(uint32_t, false);
}

result_t Buffer::writeUIntLE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER_48(uint64_t, true);
}

result_t Buffer::writeUIntBE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER_48(uint64_t, false);
}

result_t Buffer::writeInt8(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int8_t, true);
}

result_t Buffer::writeInt16LE(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int16_t, true);
}

result_t Buffer::writeInt16BE(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int16_t, false);
}

result_t Buffer::writeInt32LE(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int32_t, true);
}

result_t Buffer::writeInt32BE(int32_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int32_t, false);
}

result_t Buffer::writeIntLE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER_48(int64_t, true);
}

result_t Buffer::writeIntBE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER_48(int64_t, false);
}

result_t Buffer::writeInt64LE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int64_t, true);
}

result_t Buffer::writeInt64LE(Int64_base* value, int32_t offset, bool noAssert, int32_t& retVal)
{
    return writeInt64LE(((Int64*)value)->m_num, offset, noAssert, retVal);
}

result_t Buffer::writeInt64BE(int64_t value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(int64_t, false);
}

result_t Buffer::writeInt64BE(Int64_base* value, int32_t offset, bool noAssert, int32_t& retVal)
{
    return writeInt64BE(((Int64*)value)->m_num, offset, noAssert, retVal);
}

result_t Buffer::writeFloatLE(double value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(float, true);
}

result_t Buffer::writeFloatBE(double value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(float, false);
}

result_t Buffer::writeDoubleLE(double value, int32_t offset, bool noAssert, int32_t& retVal)
{
    WRITE_NUMBER(double, true);
}

result_t Buffer::writeDoubleBE(double value, int32_t offset, bool noAssert, int32_t& retVal)
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

result_t Buffer::keys(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, (int32_t)m_data.length());
    int32_t i;

    for (i = 0; i < (int32_t)m_data.length(); i++)
        arr->Set(i, v8::Number::New(isolate->m_isolate, i));

    retVal = GetIteratorReturnValue(isolate->m_isolate, arr);
    return 0;
}

result_t Buffer::values(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, (int32_t)m_data.length());
    int32_t i;
    const char* _data = m_data.c_str();

    for (i = 0; i < (int32_t)m_data.length(); i++)
        arr->Set(i, v8::Number::New(isolate->m_isolate, (unsigned char)_data[i]));

    retVal = GetIteratorReturnValue(isolate->m_isolate, arr);
    return 0;
}

result_t Buffer::entries(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, (int32_t)m_data.length());
    int32_t i;
    const char* _data = m_data.c_str();

    for (i = 0; i < (int32_t)m_data.length(); i++) {
        v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate, 2);
        arr1->Set(0, v8::Number::New(isolate->m_isolate, i));
        arr1->Set(1, v8::Number::New(isolate->m_isolate, (unsigned char)_data[i]));
        arr->Set(i, arr1);
    }

    retVal = GetIteratorReturnValue(isolate->m_isolate, arr);
    return 0;
}

result_t Buffer::toString(exlib::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t Buffer::toString(exlib::string codec, int32_t offset, exlib::string& retVal)
{
    exlib::string str;
    int32_t length = (int32_t)m_data.length();

    if (offset < 0)
        offset = 0;

    if (offset >= length){
        retVal = "";
        return 0;
    }

    if (offset > 0) {
        str.append(m_data.c_str() + offset, length - offset);
        return commonEncode(codec, str, retVal);
    }else{
        return commonEncode(codec, m_data, retVal);
    }
}

result_t Buffer::toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal)
{
    exlib::string str;
    int32_t length = (int32_t)m_data.length();

    if (offset < 0)
        offset = 0;

    if (end < 0 || offset >= end){
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

void Buffer::fromJSON(Isolate* isolate, v8::Local<v8::Value> data, v8::Local<v8::Object>& o)
{
    if (data->IsArray()) {
        obj_ptr<Buffer> buf = new Buffer();
        buf->_append(v8::Local<v8::Array>::Cast(data));
        o = buf->wrap();
    }
}

result_t Buffer::unbind(obj_ptr<object_base>& retVal)
{
    retVal = new Buffer(m_data);
    return 0;
}
}
