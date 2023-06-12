#pragma once

#include <string>
#include <list>

#include "ifs/Buffer.h"

namespace fibjs {

class Buffer : public Buffer_base {
public:
    class store {
    public:
        store(const void* _data, size_t _length)
            : m_length(_length)
        {
            m_store = NewBackingStore(_length);
            if (_data)
                memcpy(data(), _data, _length);
        }

        store(store& s)
            : m_store(s.m_store)
            , m_offset(s.m_offset)
            , m_length(s.m_length)
        {
        }

        store(v8::Local<v8::Uint8Array> ui)
            : m_store(ui->Buffer()->GetBackingStore())
            , m_offset(ui->ByteOffset())
            , m_length(ui->ByteLength())
        {
        }

        store(v8::Local<v8::ArrayBuffer> ab)
            : m_store(ab->GetBackingStore())
            , m_offset(0)
            , m_length(ab->ByteLength())
        {
        }

        store(std::shared_ptr<v8::BackingStore> _store, size_t offset, size_t length)
            : m_store(_store)
            , m_offset(offset)
            , m_length(length)
        {
        }

    public:
        uint8_t* data()
        {
            return (uint8_t*)m_store->Data() + m_offset;
        }

        size_t length()
        {
            return m_length;
        }

    public:
        std::shared_ptr<v8::BackingStore> m_store;
        size_t m_offset = 0;
        size_t m_length = 0;
    };

public:
    Buffer(const void* data = NULL, size_t length = 0)
        : m_store(data, length)
    {
        extMemory(length);
    }

    Buffer(Buffer* buf)
        : m_store(buf->m_store)
    {
    }

    Buffer(v8::Local<v8::Uint8Array> ui)
        : m_store(ui)
    {
    }

    Buffer(v8::Local<v8::ArrayBuffer> ab)
        : m_store(ab)
    {
    }

    Buffer(std::shared_ptr<v8::BackingStore> _store, size_t offset, size_t length)
        : m_store(_store, offset, length)
    {
    }

public:
    uint8_t* data()
    {
        return m_store.data();
    }

    size_t length()
    {
        return m_store.length();
    }

    static Buffer* Cast(Buffer_base* buf)
    {
        return static_cast<Buffer*>(buf);
    }

    static Buffer* getInstance(object_base* o)
    {
        return static_cast<Buffer*>(Buffer_base::getInstance(o));
    }

    static Buffer* getInstance(v8::Local<v8::Value> o);

public:
    // object
    virtual v8::Local<v8::Object> wrap(Isolate* isolate, v8::Local<v8::Object> o = v8::Local<v8::Object>());
    virtual result_t equals(object_base* expected, bool& retVal);
    virtual result_t unbind(obj_ptr<object_base>& retVal);

public:
    // Buffer_base
    virtual result_t compare(Buffer_base* buf, int32_t& retVal);
    virtual result_t get_length(int32_t& retVal);
    virtual result_t write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal);
    virtual result_t write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal);
    virtual result_t write(exlib::string str, exlib::string codec, int32_t& retVal);
    virtual result_t fill(int32_t v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(exlib::string v, int32_t offset, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    virtual result_t fill(exlib::string v, exlib::string codec, obj_ptr<Buffer_base>& retVal);
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
    bool is_safe_codec(exlib::string codec);

public:
    static v8::Local<v8::Value> load_module();

public:
    result_t fill(int32_t v, int32_t offset, int32_t end);
    result_t fill(Buffer_base* v, int32_t offset, int32_t end);
    result_t fill(exlib::string v, int32_t offset, int32_t end, exlib::string codec);
    result_t fill(exlib::string v, int32_t offset, exlib::string codec);
    result_t fill(exlib::string v, exlib::string codec);

private:
    static void proto_compare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void proto_equals(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void proto_indexOf(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void proto_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void proto_fill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void proto_write(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    store m_store;
};

}
