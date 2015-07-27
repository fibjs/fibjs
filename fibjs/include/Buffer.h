#include <string>
#include <list>

#include "ifs/Buffer.h"

#ifndef _fj_BUFFER_H
#define _fj_BUFFER_H

namespace fibjs
{

class Buffer: public Buffer_base
{
public:
    Buffer()
    {
    }

    Buffer(const std::string &strData) :
        m_data(strData)
    {
        extMemory((int32_t)m_data.length());
    }

    Buffer(const void *pData, size_t n) :
        m_data((const char *)pData, n)
    {
        extMemory((int32_t)m_data.length());
    }

public:
    // Buffer_base
    virtual result_t _indexed_getter(uint32_t index, int32_t &retVal);
    virtual result_t _indexed_setter(uint32_t index, int32_t newVal);
    virtual result_t get_length(int32_t &retVal);
    virtual result_t resize(int32_t sz);
    virtual result_t write(v8::Local<v8::Array> datas);
    virtual result_t write(Buffer_base *data);
    virtual result_t write(const char *str, const char *codec);
    virtual result_t copy(Buffer_base *targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal);
    virtual result_t readUInt8(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readUInt16LE(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readUInt16BE(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readUInt32LE(int32_t offset, bool noAssert, int64_t &retVal);
    virtual result_t readUInt32BE(int32_t offset, bool noAssert, int64_t &retVal);
    virtual result_t readInt8(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readInt16LE(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readInt16BE(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readInt32LE(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readInt32BE(int32_t offset, bool noAssert, int32_t &retVal);
    virtual result_t readInt64LE(int32_t offset, bool noAssert, obj_ptr<Int64_base> &retVal);
    virtual result_t readInt64BE(int32_t offset, bool noAssert, obj_ptr<Int64_base> &retVal);
    virtual result_t readFloatLE(int32_t offset, bool noAssert, double &retVal);
    virtual result_t readFloatBE(int32_t offset, bool noAssert, double &retVal);
    virtual result_t readDoubleLE(int32_t offset, bool noAssert, double &retVal);
    virtual result_t readDoubleBE(int32_t offset, bool noAssert, double &retVal);
    virtual result_t writeUInt8(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt16LE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt16BE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt32LE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeUInt32BE(int64_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt8(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt16LE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt16BE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt32LE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt32BE(int32_t value, int32_t offset, bool noAssert);
    virtual result_t writeInt64LE(Int64_base *value, int32_t offset, bool noAssert);
    virtual result_t writeInt64BE(Int64_base *value, int32_t offset, bool noAssert);
    virtual result_t writeFloatLE(double value, int32_t offset, bool noAssert);
    virtual result_t writeFloatBE(double value, int32_t offset, bool noAssert);
    virtual result_t writeDoubleLE(double value, int32_t offset, bool noAssert);
    virtual result_t writeDoubleBE(double value, int32_t offset, bool noAssert);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base> &retVal);
    virtual result_t hex(std::string &retVal);
    virtual result_t base64(std::string &retVal);
    virtual result_t toString(const char *codec, std::string &retVal);
    virtual result_t toString(std::string &retVal);

    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal);

private:
    result_t readNumber(int32_t offset, char *buf, int32_t size, bool noAssert, bool le);
    result_t writeNumber(int32_t offset, const char *buf, int32_t size, bool noAssert, bool le);

    result_t readInt64LE(int32_t offset, bool noAssert, int64_t &retVal);
    result_t readInt64BE(int32_t offset, bool noAssert, int64_t &retVal);

    result_t writeInt64LE(int64_t value, int32_t offset, bool noAssert);
    result_t writeInt64BE(int64_t value, int32_t offset, bool noAssert);

private:
    std::string m_data;
};

}

#endif // BUFFER_H
