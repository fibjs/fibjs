#include "Buffer.h"
#include "ifs/encoding.h"
#include "Int64.h"
#include <cstring>
#include <string>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace fibjs
{

inline result_t generateEnd(const int32_t buffer_length, const int32_t offset, int32_t &end)
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

result_t Buffer_base::_new(const char *str, const char *codec,
                           obj_ptr<Buffer_base> &retVal,
                           v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    return retVal->append(str, codec);
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
    return retVal->append(datas);
}

result_t Buffer_base::_new(Buffer_base* buffer,
                           obj_ptr<Buffer_base> &retVal,
                           v8::Local<v8::Object> This)
{
    retVal = new Buffer();
    return retVal->append(buffer);
}


result_t Buffer_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return 0;
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

    std::string str;
    Isolate* isolate = Isolate::current();

    for (int32_t i = 0; i < sz; i ++)
    {
        v8::Local<v8::Value> v = buflist->Get(i);
        obj_ptr<Buffer_base> vdata;

        hr = GetArgumentValue(isolate->m_isolate, v, vdata);
        if (hr < 0)
            return CHECK_ERROR(hr);

        std::string vstr;
        vdata -> toString(vstr);
        buf_length = (int32_t) vstr.length();

        if (-1 == cutLength)
            total_length = offset + buf_length;

        if (offset + buf_length <=  total_length) {
            str.append(vstr.c_str(), buf_length);
            offset += buf_length;
        }
        else
        {
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

    extMemory(sz - (int32_t) m_data.length());
    m_data.resize(sz);

    return 0;
}

result_t Buffer::append(v8::Local<v8::Array> datas)
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

        extMemory((int32_t) sz);
        m_data.append(str);
    }

    return 0;
}

result_t Buffer::append(Buffer_base *data)
{
    std::string strBuf;
    data->toString(strBuf);

    extMemory((int32_t) strBuf.length());
    m_data.append(strBuf);
    return 0;
}

result_t Buffer::append(const char *str, const char *codec)
{
    if (!qstricmp(codec, "utf8") || !qstricmp(codec, "utf-8"))
    {
        size_t sz = qstrlen(str);

        extMemory((int32_t) sz);
        m_data.append(str, sz);
        return 0;
    }

    obj_ptr<Buffer_base> data;
    result_t hr;

    if (!qstrcmp(codec, "hex"))
        hr = hex_base::decode(str, data);
    else if (!qstrcmp(codec, "base64"))
        hr = base64_base::decode(str, data);
    else
        hr = iconv_base::encode(codec, str, data);

    if (hr < 0)
        return hr;

    return append(data);
}

result_t Buffer::write(const char* str, int32_t offset, int32_t length, const char* codec, int32_t& retVal)
{
    int32_t max_length = 0;
    int32_t buffer_length = (int32_t)m_data.length();

    if (offset < 0 || length < -1)
        return CHECK_ERROR(CALL_E_INVALIDARG);
    if (buffer_length < offset)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    max_length = (int32_t)qstrlen(str);
    max_length = MIN(max_length,  buffer_length - offset);
    if (0 == length)
        return 0;
    else if (0 < length)
        max_length = MIN(max_length,  length);

    if (max_length < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    retVal = max_length;
    if (!qstricmp(codec, "utf8") || !qstricmp(codec, "utf-8"))
    {
        m_data.replace(offset, max_length, str, max_length);
        return 0;
    }

    result_t hr;
    obj_ptr<Buffer_base> data;
    std::string strBuf;

    if (!qstrcmp(codec, "hex"))
        hr = hex_base::decode(str, data);
    else if (!qstrcmp(codec, "base64"))
        hr = base64_base::decode(str, data);
    else
        hr = iconv_base::encode(codec, str, data);

    if (hr < 0)
        return hr;
    data->toString(strBuf);
    m_data.replace(offset, max_length, strBuf.c_str(), max_length);

    return hr;
}

result_t Buffer::fill(int32_t v, int32_t offset, int32_t end)
{
    result_t hr = generateEnd((int32_t)m_data.length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    memset(&m_data[offset], v & 255, end - offset);
    return 0;
}

result_t Buffer::fill(const char* v, int32_t offset, int32_t end)
{
    result_t hr = generateEnd((int32_t)m_data.length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    int32_t length = end - offset;
    int32_t str_length = (int32_t) strlen(v);

    if (str_length == 0)
        return 0;
    while (length > 0)
    {
        m_data.replace(offset, MIN(str_length, length), v, MIN(str_length, length));
        length -= str_length;
        offset += str_length;
    }
    return 0;
}

result_t Buffer::fill(Buffer_base* v, int32_t offset, int32_t end)
{
    result_t hr = generateEnd((int32_t)m_data.length(), offset, end);
    if (hr < 0)
        return CHECK_ERROR(hr);

    obj_ptr<Buffer> v_data = dynamic_cast<Buffer *>(v);
    int32_t length = end - offset;
    int32_t v_length = (int32_t)v_data->m_data.length();

    if (v_length == 0)
        return 0;
    while (length > 0)
    {
        memcpy(&m_data[offset], &v_data->m_data[0], MIN(v_length, length));
        length -= v_length;
        offset += v_length;
    }
    return 0;
}

result_t Buffer::indexOf(int32_t v, int32_t offset, int32_t& retVal)
{
    int32_t buf_length = (int32_t)m_data.length();
    result_t hr = validOffset(buf_length, offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    while (offset < buf_length)
    {
        if (m_data[offset] == (v & 255))
        {
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

    obj_ptr<Buffer> v_data = dynamic_cast<Buffer *>(v);
    std::string vstr;
    v_data->toString(vstr);

    retVal = (int32_t) m_data.find (vstr, offset);
    return 0;
}

result_t Buffer::indexOf(const char* v, int32_t offset, int32_t& retVal)
{
    result_t hr = validOffset((int32_t)m_data.length(), offset);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = (int32_t) m_data.find (v, offset);
    return 0;
}

result_t Buffer::equals(Buffer_base * buf, bool & retVal)
{
    int32_t cmp;
    this->compare(buf, cmp);
    retVal = !cmp;
    return 0;
}

result_t Buffer::compare(Buffer_base * buf, int32_t& retVal)
{
    obj_ptr<Buffer> cmpdata = dynamic_cast<Buffer *>(buf);
    int32_t pos_length = (int32_t)m_data.length();
    int32_t neg_length = (int32_t)cmpdata->m_data.length();

    retVal =  memcmp(&m_data[0], &cmpdata->m_data[0], MIN(pos_length, neg_length));
    if (retVal)
        return 0;

    retVal = pos_length - neg_length;
    return 0;
}

result_t Buffer::copy(Buffer_base * targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t &retVal)
{
    if (targetStart < 0 || sourceStart < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (sourceStart > (int32_t)m_data.length())
        return CHECK_ERROR(CALL_E_OUTRANGE);

    Buffer *buf = static_cast<Buffer *>(targetBuffer);
    int32_t bufLen;
    buf->get_length(bufLen);

    if (sourceEnd == -1)
        sourceEnd = (int32_t)m_data.length();

    if (targetStart >= bufLen || sourceStart >= sourceEnd)
    {
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
    int32_t length = (int32_t) m_data.length();
    if (end < 0)
        end = length + end + 1;

    if (start < 0 || end < 0 || start > end)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (end > length)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    obj_ptr<Buffer> pNew = new Buffer();
    if (start < end)
    {
        pNew->m_data.append(m_data, start, end - start);
        pNew->extMemory((int32_t) (end - start));
    }
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
    return hex_base::encode(data, retVal);
}

result_t Buffer::base64(std::string &retVal)
{
    obj_ptr<Buffer_base> data = this;
    return base64_base::encode(data, retVal);
}

result_t Buffer::toString(const char* codec, int32_t offset, int32_t end, std::string &retVal)
{
    result_t hr;
    std::string str;
    int32_t str_length;

    if (!qstricmp(codec, "utf8") || !qstricmp(codec, "utf-8"))
    {
        str = m_data;
        hr = 0;
    }
    else
    {
        if (!qstrcmp(codec, "hex"))
            hr = hex_base::encode(this, str);
        else if (!qstrcmp(codec, "base64"))
            hr = base64_base::encode(this, str);
        else
            hr = iconv_base::decode(codec, this, str);
    }

    if (hr < 0)
        return hr;

    str_length = (int32_t)str.length();
    if (end < 0)
        end = str_length + end + 1;

    if (offset < 0 || end < 0 || offset > end)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (end > str_length)
        end = str_length;

    retVal = str.substr(offset, end - offset);
    return hr;
}

result_t Buffer::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Array> a = v8::Array::New(isolate->m_isolate, (int32_t) m_data.length());
    int32_t i;

    for (i = 0; i < (int32_t) m_data.length(); i++)
        a->Set(i, v8::Number::New(isolate->m_isolate, (unsigned char) m_data[i]));

    retVal = a;

    return 0;
}

}
