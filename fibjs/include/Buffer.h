#include <string>
#include <list>

#include "ifs/Buffer.h"

#ifndef _fj_BUFFER_H
#define _fj_BUFFER_H

namespace fibjs {

class Buffer : public Buffer_base {
public:
    Buffer()
    {
    }

    Buffer(const exlib::string& strData)
        : m_data(strData)
    {
        extMemory((int32_t)m_data.length());
    }

    Buffer(Buffer_base* data)
    {
        exlib::string str;

        data->toString(str);
        m_data = str;
        extMemory((int32_t)m_data.length());
    }

    Buffer(const void* pData, size_t n)
        : m_data((const char*)pData, n)
    {
        extMemory((int32_t)m_data.length());
    }

public:
    // object_base
    virtual result_t equals(object_base* expected, bool& retVal);
    virtual result_t unbind(obj_ptr<object_base>& retVal);

public:
    // Buffer_base
    virtual result_t _indexed_getter(uint32_t index, int32_t& retVal);
    virtual result_t _indexed_setter(uint32_t index, int32_t newVal);
    virtual result_t get_length(int32_t& retVal);
    virtual result_t resize(int32_t sz);
    virtual result_t append(v8::Local<v8::Array> datas);
    virtual result_t append(v8::Local<v8::TypedArray> datas);
    virtual result_t append(v8::Local<v8::ArrayBuffer> datas);
    virtual result_t append(Buffer_base* data);
    virtual result_t append(exlib::string str, exlib::string codec);
    virtual result_t write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal);
    virtual result_t write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal);
    virtual result_t write(exlib::string str, exlib::string codec, int32_t& retVal);
    virtual result_t fill(int32_t v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(exlib::string v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t indexOf(int32_t v, int32_t offset, int32_t& retVal);
    virtual result_t indexOf(exlib::string v, int32_t offset, int32_t& retVal);
    virtual result_t indexOf(Buffer_base* v, int32_t offset, int32_t& retVal);
    virtual result_t compare(Buffer_base* buf, int32_t& retVal);
    virtual result_t copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal);
    virtual result_t readUInt8(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readUInt16LE(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readUInt16BE(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readUInt32LE(int32_t offset, bool noAssert, int64_t& retVal);
    virtual result_t readUInt32BE(int32_t offset, bool noAssert, int64_t& retVal);
    virtual result_t readUIntLE(int32_t offset, bool noAssert, int64_t& retVal);
    virtual result_t readUIntBE(int32_t offset, bool noAssert, int64_t& retVal);
    virtual result_t readInt8(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readInt16LE(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readInt16BE(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readInt32LE(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readInt32BE(int32_t offset, bool noAssert, int32_t& retVal);
    virtual result_t readIntLE(int32_t offset, bool noAssert, int64_t& retVal);
    virtual result_t readIntBE(int32_t offset, bool noAssert, int64_t& retVal);
    virtual result_t readInt64LE(int32_t offset, bool noAssert, obj_ptr<Int64_base>& retVal);
    virtual result_t readInt64BE(int32_t offset, bool noAssert, obj_ptr<Int64_base>& retVal);
    virtual result_t readFloatLE(int32_t offset, bool noAssert, double& retVal);
    virtual result_t readFloatBE(int32_t offset, bool noAssert, double& retVal);
    virtual result_t readDoubleLE(int32_t offset, bool noAssert, double& retVal);
    virtual result_t readDoubleBE(int32_t offset, bool noAssert, double& retVal);
    virtual result_t writeUInt8(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt16LE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt16BE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt32LE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt32BE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeUIntLE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeUIntBE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt8(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt16LE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt16BE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt32LE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt32BE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeIntLE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeIntBE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt64LE(Int64_base* value, int32_t offset, bool noAssert);
    virtual result_t writeInt64BE(Int64_base* value, int32_t offset, bool noAssert);
    virtual result_t writeFloatLE(double value, int32_t offset, bool noAssert);
    virtual result_t writeFloatBE(double value, int32_t offset, bool noAssert);
    virtual result_t writeDoubleLE(double value, int32_t offset, bool noAssert);
    virtual result_t writeDoubleBE(double value, int32_t offset, bool noAssert);
    virtual result_t slice(int32_t start, obj_ptr<Buffer_base>& retVal);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t hex(exlib::string& retVal);
    virtual result_t base64(exlib::string& retVal);
    virtual result_t toArray(v8::Local<v8::Array>& retVal);
    virtual result_t toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal);
    virtual result_t toString(exlib::string& retVal);

    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    static void fromJSON(Isolate* isolate, v8::Local<v8::Value> data, v8::Local<v8::Object>& o);

    char* data()
    {
        return &m_data[0];
    }

public:
    result_t readNumber(int32_t offset, char* buf, int32_t size, bool noAssert, bool le);
    result_t writeNumber(int32_t offset, const char* buf, int32_t size, bool noAssert, bool le);

    result_t readInt64LE(int32_t offset, bool noAssert, int64_t& retVal);
    result_t readInt64BE(int32_t offset, bool noAssert, int64_t& retVal);

    result_t writeInt64LE(int64_t value, int32_t offset, bool noAssert);
    result_t writeInt64BE(int64_t value, int32_t offset, bool noAssert);

private:
    template <typename T>
    result_t _append(T datas)
    {
        int32_t sz = (int32_t)datas->Length();

        if (sz) {
            int32_t i;
            result_t hr;
            exlib::string str;

            str.resize(sz);
            for (i = 0; i < sz; i++) {
                v8::Local<v8::Value> v = datas->Get(i);
                int32_t num;

                hr = GetArgumentValue(v, num);
                if (hr < 0)
                    return CHECK_ERROR(hr);

                str[i] = (char)num;
            }

            extMemory((int32_t)sz);
            m_data.append(str);
        }

        return 0;
    }

private:
    exlib::string m_data;
};
}

#endif // BUFFER_H
