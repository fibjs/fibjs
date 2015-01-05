#include "Buffer.h"
#include "ifs/encoding.h"
#include "Int64.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace fibjs
{

result_t Buffer_base::_new(const char *str, const char *codec,
                           obj_ptr<Buffer_base> &retVal,
                           v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    return retVal->write(str, codec);
}

result_t Buffer_base::_new(int32_t size, obj_ptr<Buffer_base> &retVal,
                           v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    return retVal->resize(size);
}

result_t Buffer_base::_new(v8::Local<v8::Array> datas,
                           obj_ptr<Buffer_base> &retVal,
                           v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    return retVal->write(datas);
}

result_t Buffer::_indexed_getter(uint32_t index, int32_t &retVal)
{
    if (index >= m_data.length())
        return CHECK_ERROR(CALL_E_BADINDEX);

    retVal = (unsigned char) m_data[index];
    return 0;
}

result_t Buffer::_indexed_setter(uint32_t index, int32_t newVal)
{
    if (index >= m_data.length())
        return CHECK_ERROR(CALL_E_BADINDEX);

    if (newVal < 0 || newVal > 255)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_data[index] = newVal;
    return 0;
}

result_t Buffer::get_length(int32_t &retVal)
{
    retVal = (int32_t) m_data.length();
    return 0;
}

result_t Buffer::resize(int32_t sz)
{
    if (sz < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    extMemory(sz - (int) m_data.length());
    m_data.resize(sz);

    return 0;
}

result_t Buffer::write(v8::Local<v8::Array> datas)
{
    int32_t sz = datas->Length();

    if (sz)
    {
        int32_t i;
        result_t hr;
        std::string str;

        str.resize(sz);
        for (i = 0; i < sz; i ++)
        {
            v8::Local<v8::Value> v = datas->Get(i);
            int32_t num;

            hr = GetArgumentValue(v, num);
            if (hr < 0)
                return CHECK_ERROR(hr);

            if (num < 0 || num > 256)
                return CHECK_ERROR(CALL_E_OUTRANGE);

            str[i] = num;
        }

        extMemory((int) sz);
        m_data.append(str);
    }

    return 0;
}

result_t Buffer::write(Buffer_base *data)
{
    std::string strBuf;
    data->toString(strBuf);

    extMemory((int) strBuf.length());
    m_data.append(strBuf);
    return 0;
}

result_t Buffer::write(const char *str, const char *codec)
{
    if (!qstricmp(codec, "utf8") || !qstricmp(codec, "utf-8"))
    {
        size_t sz = qstrlen(str);

        extMemory((int) sz);
        m_data.append(str, sz);
        return 0;
    }

    obj_ptr<Buffer_base> data;
    result_t hr;

    if (!qstrcmp(codec, "hex"))
        hr = encoding_base::hexDecode(str, data);
    else if (!qstrcmp(codec, "base64"))
        hr = encoding_base::base64Decode(str, data);
    else
        hr = encoding_base::iconvEncode(codec, str, data);

    if (hr < 0)
        return hr;

    return write(data);
}

result_t Buffer::copy(Buffer_base *targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal)
{
    if (targetStart < 0 || sourceStart < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if(sourceStart > m_data.length())
    {
        return CHECK_ERROR(CALL_E_OUTRANGE);
    }

    Buffer* buf = static_cast<Buffer*>(targetBuffer);
    int32_t bufLen;
    buf->get_length(bufLen);

    if(sourceEnd == -1)
    {
        sourceEnd = m_data.length();
    }

    if(targetStart >= bufLen || sourceStart >= sourceEnd)
    {
        retVal = 0;
        return 0;
    }

    int32_t targetSz = bufLen - targetStart;
    int32_t sourceSz = m_data.length() - sourceStart;
    int32_t sourceLen = sourceEnd - sourceStart;
    int32_t sz = MIN(MIN(sourceLen, targetSz), sourceSz);

    memcpy(&buf->m_data[targetStart], m_data.c_str() + sourceStart, sz);

    retVal = sz;

    return 0;
}

result_t Buffer::readNumber(int32_t offset, char *buf, int32_t size, bool noAssert, bool le)
{
    int32_t sz = size;

    if (offset + sz > (int32_t) m_data.length())
    {
        if (!noAssert)
            return CHECK_ERROR(CALL_E_OUTRANGE);

        sz = (int32_t) m_data.length() - offset;
        if (sz <= 0)
            return 0;
    }

    if (size == 1)
    {
        buf[0] = *(m_data.c_str() + offset);
        return 0;
    }

    if (le)
        memcpy(buf, m_data.c_str() + offset, sz);
    else
    {
        int32_t i;
        for (i = 0; i < sz; i ++)
            buf[size - i - 1] = *(m_data.c_str() + offset + i);
    }

    return 0;
}

#define READ_NUMBER(t, le) \
    t v = 0; \
    result_t hr = readNumber(offset, (char *)&v, sizeof(v), noAssert, le); \
    if (hr < 0)return hr; \
    retVal = v; \
    return 0;

result_t Buffer::readUInt8(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(uint8_t, true);
}

result_t Buffer::readUInt16LE(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(uint16_t, true);
}

result_t Buffer::readUInt16BE(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(uint16_t, false);
}

result_t Buffer::readUInt32LE(int32_t offset, bool noAssert, int64_t &retVal)
{
    READ_NUMBER(uint32_t, true);
}

result_t Buffer::readUInt32BE(int32_t offset, bool noAssert, int64_t &retVal)
{
    READ_NUMBER(uint32_t, false);
}

result_t Buffer::readInt8(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(int8_t, true);
}

result_t Buffer::readInt16LE(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(int16_t, true);
}

result_t Buffer::readInt16BE(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(int16_t, false);
}

result_t Buffer::readInt32LE(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(int32_t, true);
}

result_t Buffer::readInt32BE(int32_t offset, bool noAssert, int32_t &retVal)
{
    READ_NUMBER(int32_t, false);
}

result_t Buffer::readInt64LE(int32_t offset, bool noAssert, int64_t &retVal)
{
    READ_NUMBER(int64_t, true);
}

result_t Buffer::readInt64BE(int32_t offset, bool noAssert, int64_t &retVal)
{
    READ_NUMBER(int64_t, false);
}

result_t Buffer::readInt64LE(int32_t offset, bool noAssert, obj_ptr<Int64_base> &retVal)
{
    int64_t v;
    result_t hr = readInt64LE(offset, noAssert, v);
    if (hr < 0)
        return hr;

    retVal = new Int64(v);
    return 0;
}

result_t Buffer::readInt64BE(int32_t offset, bool noAssert, obj_ptr<Int64_base> &retVal)
{
    int64_t v;
    result_t hr = readInt64BE(offset, noAssert, v);
    if (hr < 0)
        return hr;

    retVal = new Int64(v);
    return 0;
}

result_t Buffer::readFloatLE(int32_t offset, bool noAssert, double &retVal)
{
    READ_NUMBER(float, true);
}

result_t Buffer::readFloatBE(int32_t offset, bool noAssert, double &retVal)
{
    READ_NUMBER(float, false);
}

result_t Buffer::readDoubleLE(int32_t offset, bool noAssert, double &retVal)
{
    READ_NUMBER(double, true);
}

result_t Buffer::readDoubleBE(int32_t offset, bool noAssert, double &retVal)
{
    READ_NUMBER(double, false);
}

result_t Buffer::writeNumber(int32_t offset, const char *buf, int32_t size, bool noAssert, bool le)
{
    int32_t sz = size;

    if (offset + sz > (int32_t) m_data.length())
    {
        if (!noAssert)
            return CHECK_ERROR(CALL_E_OUTRANGE);

        sz = (int32_t) m_data.length() - offset;
        if (sz <= 0)
            return 0;
    }

    if (size == 1)
    {
        m_data[offset] = buf[0];
        return 0;
    }

    if (le)
        memcpy(&m_data[offset], buf, sz);
    else
    {
        int32_t i;
        for (i = 0; i < sz; i ++)
            m_data[offset + i] = buf[size - i - 1];
    }

    return 0;
}

#define WRITE_NUMBER(t, le) \
    t v = (t)value; \
    result_t hr = writeNumber(offset, (char *)&v, sizeof(v), noAssert, le); \
    if (hr < 0)return hr; \
    return 0;


result_t Buffer::writeUInt8(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(uint8_t, true);
}

result_t Buffer::writeUInt16LE(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(uint16_t, true);
}

result_t Buffer::writeUInt16BE(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(uint16_t, false);
}

result_t Buffer::writeUInt32LE(int64_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(uint32_t, true);
}

result_t Buffer::writeUInt32BE(int64_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(uint32_t, false);
}

result_t Buffer::writeInt8(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int8_t, true);
}

result_t Buffer::writeInt16LE(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int16_t, true);
}

result_t Buffer::writeInt16BE(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int16_t, false);
}

result_t Buffer::writeInt32LE(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int32_t, true);
}

result_t Buffer::writeInt32BE(int32_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int32_t, false);
}

result_t Buffer::writeInt64LE(int64_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int64_t, true);
}

result_t Buffer::writeInt64LE(Int64_base *value, int32_t offset, bool noAssert)
{
    return writeInt64LE(((Int64 *)value)->m_num, offset, noAssert);
}

result_t Buffer::writeInt64BE(int64_t value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(int64_t, false);
}

result_t Buffer::writeInt64BE(Int64_base *value, int32_t offset, bool noAssert)
{
    return writeInt64BE(((Int64 *)value)->m_num, offset, noAssert);
}

result_t Buffer::writeFloatLE(double value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(float, true);
}

result_t Buffer::writeFloatBE(double value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(float, false);
}

result_t Buffer::writeDoubleLE(double value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(double, true);
}

result_t Buffer::writeDoubleBE(double value, int32_t offset, bool noAssert)
{
    WRITE_NUMBER(double, false);
}

result_t Buffer::slice(int32_t start, int32_t end, obj_ptr<Buffer_base> &retVal)
{
    if (start < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (end < 0)
        end = (int32_t) m_data.length();

    obj_ptr<Buffer> pNew = new Buffer();

    pNew->m_data.append(m_data, start, end - start);
    pNew->extMemory((int) (end - start));

    retVal = pNew;

    return 0;
}

result_t Buffer::toString(std::string &retVal)
{
    retVal = m_data;
    return 0;
}

result_t Buffer::hex(std::string &retVal)
{
    obj_ptr<Buffer_base> data = this;
    return encoding_base::hexEncode(data, retVal);
}

result_t Buffer::base64(std::string &retVal)
{
    obj_ptr<Buffer_base> data = this;
    return encoding_base::base64Encode(data, retVal);
}

result_t Buffer::toString(const char *codec, std::string &retVal)
{
    if (!qstricmp(codec, "utf8") || !qstricmp(codec, "utf-8"))
    {
        retVal = m_data;
        return 0;
    }

    if (!qstrcmp(codec, "hex"))
        return hex(retVal);

    if (!qstrcmp(codec, "base64"))
        return base64(retVal);

    obj_ptr<Buffer_base> data = this;
    return encoding_base::iconvDecode(codec, data, retVal);
}

result_t Buffer::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    v8::Local<v8::Array> a = v8::Array::New(isolate, (int) m_data.length());
    int i;

    for (i = 0; i < (int) m_data.length(); i++)
        a->Set(i, v8::Number::New(isolate, (unsigned char) m_data[i]));

    retVal = a;

    return 0;
}

}
