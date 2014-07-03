#include "Buffer.h"
#include "ifs/encoding.h"

namespace fibjs
{

result_t Buffer_base::_new(const char *str, const char *codec,
                           obj_ptr<Buffer_base> &retVal)
{
    retVal = new Buffer();
    return retVal->write(str, codec);
}

result_t Buffer_base::_new(Buffer_base *data, obj_ptr<Buffer_base> &retVal)
{
    retVal = new Buffer();
    return retVal->write(data);
}

result_t Buffer_base::_new(int32_t size, obj_ptr<Buffer_base> &retVal)
{
    retVal = new Buffer();
    return retVal->resize(size);
}

result_t Buffer_base::_new(v8::Local<v8::Array> datas,
                           obj_ptr<Buffer_base> &retVal)
{
    retVal = new Buffer();
    return retVal->write(datas);
}

result_t Buffer::_indexed_getter(uint32_t index, int32_t &retVal)
{
    if (index >= m_data.length())
        return CALL_E_BADINDEX;

    retVal = (unsigned char) m_data[index];
    return 0;
}

result_t Buffer::_indexed_setter(uint32_t index, int32_t newVal)
{
    if (index >= m_data.length())
        return CALL_E_BADINDEX;

    if (newVal < 0 || newVal > 255)
        return CALL_E_OUTRANGE;

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
        return CALL_E_INVALIDARG;

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
                return hr;

            if (num < 0 || num > 256)
                return CALL_E_OUTRANGE;

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

result_t Buffer::slice(int32_t start, int32_t end, obj_ptr<Buffer_base> &retVal)
{
    if (start < 0)
        return CALL_E_INVALIDARG;

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
