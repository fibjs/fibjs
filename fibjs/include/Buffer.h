#pragma once

#include <string>
#include <list>

#include "ifs/Buffer.h"

namespace fibjs {

class Buffer : public Buffer_base {
public:
    Buffer(const void* data = NULL, size_t length = 0)
    {
        init(data, length);
    }

    Buffer(Buffer* buf)
        : m_store(buf->m_store)
    {
    }

public:
    uint8_t* data()
    {
        return (uint8_t*)m_store->Data();
    }

    size_t length()
    {
        return m_store->ByteLength();
    }

    static Buffer* Cast(Buffer_base* buf)
    {
        return static_cast<Buffer*>(buf);
    }

    static Buffer* getInstance(object_base* expected)
    {
        return dynamic_cast<Buffer*>(expected);
    }

public:
    // object
    virtual v8::Local<v8::Object> wrap(v8::Local<v8::Object> o = v8::Local<v8::Object>());
    virtual result_t equals(object_base* expected, bool& retVal);
    virtual result_t unbind(obj_ptr<object_base>& retVal);

public:
    // Buffer_base
    virtual result_t compare(Buffer_base* buf, int32_t& retVal);
    virtual result_t write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal);
    virtual result_t write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal);
    virtual result_t write(exlib::string str, exlib::string codec, int32_t& retVal);
    virtual result_t fill(int32_t v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    virtual result_t copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal);
    virtual result_t set(Buffer_base* src, int32_t start, int32_t& retVal);
    virtual result_t readUInt8(int32_t offset, int32_t& retVal);
    virtual result_t readUInt16LE(int32_t offset, int32_t& retVal);
    virtual result_t readUInt16BE(int32_t offset, int32_t& retVal);
    virtual result_t readUInt32LE(int32_t offset, double& retVal);
    virtual result_t readUInt32BE(int32_t offset, double& retVal);
    virtual result_t readUIntLE(int32_t offset, int32_t byteLength, double& retVal);
    virtual result_t readUIntBE(int32_t offset, int32_t byteLength, double& retVal);
    virtual result_t readInt64LE(int32_t offset, int64_t& retVal);
    virtual result_t readInt64BE(int32_t offset, int64_t& retVal);
    virtual result_t readInt8(int32_t offset, int32_t& retVal);
    virtual result_t readInt16LE(int32_t offset, int32_t& retVal);
    virtual result_t readInt16BE(int32_t offset, int32_t& retVal);
    virtual result_t readInt32LE(int32_t offset, int32_t& retVal);
    virtual result_t readInt32BE(int32_t offset, int32_t& retVal);
    virtual result_t readIntLE(int32_t offset, int32_t byteLength, double& retVal);
    virtual result_t readIntBE(int32_t offset, int32_t byteLength, double& retVal);
    virtual result_t readFloatLE(int32_t offset, double& retVal);
    virtual result_t readFloatBE(int32_t offset, double& retVal);
    virtual result_t readDoubleLE(int32_t offset, double& retVal);
    virtual result_t readDoubleBE(int32_t offset, double& retVal);
    virtual result_t writeUInt8(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeUInt16LE(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeUInt16BE(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeUInt32LE(int64_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeUInt32BE(int64_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeUIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal);
    virtual result_t writeUIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal);
    virtual result_t writeInt8(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeInt16LE(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeInt16BE(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeInt32LE(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeInt32BE(int32_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeInt64LE(int64_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeInt64BE(int64_t value, int32_t offset, int32_t& retVal);
    virtual result_t writeIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal);
    virtual result_t writeIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal);
    virtual result_t writeFloatLE(double value, int32_t offset, int32_t& retVal);
    virtual result_t writeFloatBE(double value, int32_t offset, int32_t& retVal);
    virtual result_t writeDoubleLE(double value, int32_t offset, int32_t& retVal);
    virtual result_t writeDoubleBE(double value, int32_t offset, int32_t& retVal);
    virtual result_t indexOf(int32_t v, int32_t offset, int32_t& retVal);
    virtual result_t indexOf(Buffer_base* v, int32_t offset, int32_t& retVal);
    virtual result_t indexOf(exlib::string v, int32_t offset, int32_t& retVal);
    virtual result_t slice(int32_t start, obj_ptr<Buffer_base>& retVal);
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal);
    virtual result_t toString(exlib::string codec, int32_t offset, exlib::string& retVal);
    virtual result_t toString(exlib::string& retVal);
    virtual result_t toArray(v8::Local<v8::Array>& retVal);
    virtual result_t hex(exlib::string& retVal);
    virtual result_t base32(exlib::string& retVal);
    virtual result_t base58(exlib::string& retVal);
    virtual result_t base64(exlib::string& retVal);

public:
    result_t fill(const uint8_t* buf, size_t sz, int32_t offset, int32_t end);
    result_t readNumber(int32_t offset, char* buf, int32_t size, int32_t value_size, bool le);
    result_t writeNumber(int32_t offset, const char* buf, int32_t size, int32_t value_size, bool le, int32_t& retVal);

public:
    void init(const void* data, size_t length);
    bool is_safe_codec(exlib::string codec);

private:
    std::shared_ptr<v8::BackingStore> m_store;
    v8::Global<v8::ArrayBuffer> m_buf;
};

}
