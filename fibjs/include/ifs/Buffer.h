/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Iterator_base;

class Buffer_base : public object_base {
    DECLARE_CLASS(Buffer_base);

public:
    // Buffer_base
    static result_t _new(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::ArrayBuffer> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::TypedArray> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::ArrayBufferView> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Buffer_base* buffer, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(int32_t size, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t from(Buffer_base* buffer, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal);
    static result_t from(exlib::string str, int32_t byteOffset, int32_t length, obj_ptr<Buffer_base>& retVal);
    static result_t from(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    static result_t concat(v8::Local<v8::Array> buflist, int32_t cutLength, obj_ptr<Buffer_base>& retVal);
    static result_t alloc(int32_t size, int32_t fill, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    static result_t alloc(int32_t size, exlib::string fill, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    static result_t alloc(int32_t size, Buffer_base* fill, exlib::string codec, obj_ptr<Buffer_base>& retVal);
    static result_t allocUnsafe(int32_t size, obj_ptr<Buffer_base>& retVal);
    static result_t allocUnsafeSlow(int32_t size, obj_ptr<Buffer_base>& retVal);
    static result_t byteLength(exlib::string str, exlib::string codec, int32_t& retVal);
    static result_t byteLength(v8::Local<v8::ArrayBuffer> str, exlib::string codec, int32_t& retVal);
    static result_t byteLength(v8::Local<v8::ArrayBufferView> str, exlib::string codec, int32_t& retVal);
    static result_t byteLength(Buffer_base* str, exlib::string codec, int32_t& retVal);
    static result_t compare(Buffer_base* buf1, Buffer_base* buf2, int32_t& retVal);
    virtual result_t compare(Buffer_base* buf, int32_t& retVal) = 0;
    static result_t isEncoding(exlib::string codec, bool& retVal);
    virtual result_t _indexed_getter(uint32_t index, int32_t& retVal) = 0;
    virtual result_t _indexed_setter(uint32_t index, int32_t newVal) = 0;
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t get_byteOffset(int32_t& retVal) = 0;
    virtual result_t get_buffer(v8::Local<v8::ArrayBuffer>& retVal) = 0;
    virtual result_t resize(int32_t sz) = 0;
    virtual result_t append(Buffer_base* data) = 0;
    virtual result_t append(exlib::string str, exlib::string codec) = 0;
    virtual result_t write(exlib::string str, int32_t offset, int32_t length, exlib::string codec, int32_t& retVal) = 0;
    virtual result_t write(exlib::string str, int32_t offset, exlib::string codec, int32_t& retVal) = 0;
    virtual result_t write(exlib::string str, exlib::string codec, int32_t& retVal) = 0;
    virtual result_t fill(int32_t v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t fill(Buffer_base* v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t fill(exlib::string v, int32_t offset, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t indexOf(int32_t v, int32_t offset, int32_t& retVal) = 0;
    virtual result_t indexOf(Buffer_base* v, int32_t offset, int32_t& retVal) = 0;
    virtual result_t indexOf(exlib::string v, int32_t offset, int32_t& retVal) = 0;
    virtual result_t copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal) = 0;
    virtual result_t set(Buffer_base* src, int32_t start, int32_t& retVal) = 0;
    virtual result_t readUInt8(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readUInt16LE(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readUInt16BE(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readUInt32LE(int32_t offset, double& retVal) = 0;
    virtual result_t readUInt32BE(int32_t offset, double& retVal) = 0;
    virtual result_t readUIntLE(int32_t offset, int32_t byteLength, double& retVal) = 0;
    virtual result_t readUIntBE(int32_t offset, int32_t byteLength, double& retVal) = 0;
    virtual result_t readInt64LE(int32_t offset, int64_t& retVal) = 0;
    virtual result_t readInt64BE(int32_t offset, int64_t& retVal) = 0;
    virtual result_t readInt8(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readInt16LE(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readInt16BE(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readInt32LE(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readInt32BE(int32_t offset, int32_t& retVal) = 0;
    virtual result_t readIntLE(int32_t offset, int32_t byteLength, double& retVal) = 0;
    virtual result_t readIntBE(int32_t offset, int32_t byteLength, double& retVal) = 0;
    virtual result_t writeInt64LE(int64_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeInt64BE(int64_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t readFloatLE(int32_t offset, double& retVal) = 0;
    virtual result_t readFloatBE(int32_t offset, double& retVal) = 0;
    virtual result_t readDoubleLE(int32_t offset, double& retVal) = 0;
    virtual result_t readDoubleBE(int32_t offset, double& retVal) = 0;
    virtual result_t writeUInt8(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeUInt16LE(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeUInt16BE(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeUInt32LE(int64_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeUInt32BE(int64_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeUIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal) = 0;
    virtual result_t writeUIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal) = 0;
    virtual result_t writeInt8(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeInt16LE(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeInt16BE(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeInt32LE(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeInt32BE(int32_t value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeIntLE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal) = 0;
    virtual result_t writeIntBE(int64_t value, int32_t offset, int32_t byteLength, int32_t& retVal) = 0;
    virtual result_t writeFloatLE(double value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeFloatBE(double value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeDoubleLE(double value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t writeDoubleBE(double value, int32_t offset, int32_t& retVal) = 0;
    virtual result_t slice(int32_t start, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t join(exlib::string separator, exlib::string& retVal) = 0;
    virtual result_t reverse(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t equals(object_base* expected, bool& retVal) = 0;
    virtual result_t hex(exlib::string& retVal) = 0;
    virtual result_t base32(exlib::string& retVal) = 0;
    virtual result_t base58(exlib::string& retVal) = 0;
    virtual result_t base64(exlib::string& retVal) = 0;
    virtual result_t keys(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t values(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t entries(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t forEach(v8::Local<v8::Function> callback, v8::Local<v8::Value> thisArg) = 0;
    virtual result_t toArray(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal) = 0;
    virtual result_t toString(exlib::string codec, int32_t offset, exlib::string& retVal) = 0;
    virtual result_t toString(exlib::string& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_from(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_concat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_alloc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_allocUnsafe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_allocUnsafeSlow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_byteLength(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_compare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_compare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isEncoding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_symbol_iterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_byteOffset(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_buffer(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_resize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_indexOf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readIntLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readIntBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeIntLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeIntBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_equals(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hex(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_base32(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_base58(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_base64(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_values(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_entries(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Iterator.h"

namespace fibjs {
inline ClassInfo& Buffer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isBuffer", s_static_isBuffer, true, false },
        { "from", s_static_from, true, false },
        { "concat", s_static_concat, true, false },
        { "alloc", s_static_alloc, true, false },
        { "allocUnsafe", s_static_allocUnsafe, true, false },
        { "allocUnsafeSlow", s_static_allocUnsafeSlow, true, false },
        { "byteLength", s_static_byteLength, true, false },
        { "compare", s_compare, false, false },
        { "compare", s_static_compare, true, false },
        { "isEncoding", s_static_isEncoding, true, false },
        { "@iterator", s_symbol_iterator, false, false },
        { "resize", s_resize, false, false },
        { "append", s_append, false, false },
        { "write", s_write, false, false },
        { "fill", s_fill, false, false },
        { "indexOf", s_indexOf, false, false },
        { "copy", s_copy, false, false },
        { "set", s_set, false, false },
        { "readUInt8", s_readUInt8, false, false },
        { "readUInt16LE", s_readUInt16LE, false, false },
        { "readUInt16BE", s_readUInt16BE, false, false },
        { "readUInt32LE", s_readUInt32LE, false, false },
        { "readUInt32BE", s_readUInt32BE, false, false },
        { "readUIntLE", s_readUIntLE, false, false },
        { "readUIntBE", s_readUIntBE, false, false },
        { "readInt64LE", s_readInt64LE, false, false },
        { "readInt64BE", s_readInt64BE, false, false },
        { "readInt8", s_readInt8, false, false },
        { "readInt16LE", s_readInt16LE, false, false },
        { "readInt16BE", s_readInt16BE, false, false },
        { "readInt32LE", s_readInt32LE, false, false },
        { "readInt32BE", s_readInt32BE, false, false },
        { "readIntLE", s_readIntLE, false, false },
        { "readIntBE", s_readIntBE, false, false },
        { "writeInt64LE", s_writeInt64LE, false, false },
        { "writeInt64BE", s_writeInt64BE, false, false },
        { "readFloatLE", s_readFloatLE, false, false },
        { "readFloatBE", s_readFloatBE, false, false },
        { "readDoubleLE", s_readDoubleLE, false, false },
        { "readDoubleBE", s_readDoubleBE, false, false },
        { "writeUInt8", s_writeUInt8, false, false },
        { "writeUInt16LE", s_writeUInt16LE, false, false },
        { "writeUInt16BE", s_writeUInt16BE, false, false },
        { "writeUInt32LE", s_writeUInt32LE, false, false },
        { "writeUInt32BE", s_writeUInt32BE, false, false },
        { "writeUIntLE", s_writeUIntLE, false, false },
        { "writeUIntBE", s_writeUIntBE, false, false },
        { "writeInt8", s_writeInt8, false, false },
        { "writeInt16LE", s_writeInt16LE, false, false },
        { "writeInt16BE", s_writeInt16BE, false, false },
        { "writeInt32LE", s_writeInt32LE, false, false },
        { "writeInt32BE", s_writeInt32BE, false, false },
        { "writeIntLE", s_writeIntLE, false, false },
        { "writeIntBE", s_writeIntBE, false, false },
        { "writeFloatLE", s_writeFloatLE, false, false },
        { "writeFloatBE", s_writeFloatBE, false, false },
        { "writeDoubleLE", s_writeDoubleLE, false, false },
        { "writeDoubleBE", s_writeDoubleBE, false, false },
        { "slice", s_slice, false, false },
        { "join", s_join, false, false },
        { "reverse", s_reverse, false, false },
        { "equals", s_equals, false, false },
        { "hex", s_hex, false, false },
        { "base32", s_base32, false, false },
        { "base58", s_base58, false, false },
        { "base64", s_base64, false, false },
        { "keys", s_keys, false, false },
        { "values", s_values, false, false },
        { "entries", s_entries, false, false },
        { "forEach", s_forEach, false, false },
        { "toArray", s_toArray, false, false },
        { "toString", s_toString, false, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Buffer", Buffer_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "length", s_get_length, block_set, false },
        { "byteOffset", s_get_byteOffset, block_set, false },
        { "buffer", s_get_buffer, block_set, false }
    };

    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData s_cd = {
        "Buffer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, &s_indexed, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Buffer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Buffer_base::__new(const T& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("new Buffer()");
    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::ArrayBuffer>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::TypedArray>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::ArrayBufferView>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    STRICT_ARG(obj_ptr<Buffer_base>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Buffer_base::s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Buffer.isBuffer");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBuffer(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_from(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.from");
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = from(v0, v1, v2, vr);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = from(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = from(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_concat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.concat");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Array>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = concat(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_alloc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.alloc");
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(exlib::string, 2, "utf8");

    hr = alloc(v0, v1, v2, vr);

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(exlib::string, 2, "utf8");

    hr = alloc(v0, v1, v2, vr);

    METHOD_OVER(3, 2);

    ARG(int32_t, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(exlib::string, 2, "utf8");

    hr = alloc(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_allocUnsafe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.allocUnsafe");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = allocUnsafe(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_allocUnsafeSlow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.allocUnsafeSlow");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = allocUnsafeSlow(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_byteLength(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.byteLength");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = byteLength(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::ArrayBuffer>, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = byteLength(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::ArrayBufferView>, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = byteLength(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = byteLength(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.compare");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = compare(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.compare");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->compare(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_static_isEncoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Buffer.isEncoding");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = isEncoding(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer[]");
    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return;

    METHOD_RETURN();
}

inline void Buffer_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Buffer[]");
    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    PROPERTY_VAL(int32_t);
    hr = pInst->_indexed_setter(index, v0);

    METHOD_VOID();
}

inline void Buffer_base::s_symbol_iterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_NAME("Buffer.@iterator");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->symbol_iterator(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_get_length(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.length");
    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_get_byteOffset(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.byteOffset");
    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    hr = pInst->get_byteOffset(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_get_buffer(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::ArrayBuffer> vr;

    METHOD_NAME("Buffer.buffer");
    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    hr = pInst->get_buffer(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_resize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Buffer.resize");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->resize(v0);

    METHOD_VOID();
}

inline void Buffer_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Buffer.append");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->append(v0);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = pInst->append(v0, v1);

    METHOD_VOID();
}

inline void Buffer_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.write");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);
    OPT_ARG(exlib::string, 3, "utf8");

    hr = pInst->write(v0, v1, v2, v3, vr);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(exlib::string, 2, "utf8");

    hr = pInst->write(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = pInst->write(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_fill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.fill");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = pInst->fill(v0, v1, v2, vr);

    METHOD_OVER(3, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = pInst->fill(v0, v1, v2, vr);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = pInst->fill(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_indexOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.indexOf");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->indexOf(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->indexOf(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->indexOf(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.copy");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, -1);

    hr = pInst->copy(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.set");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);

    hr = pInst->set(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readUInt8");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readUInt8(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readUInt16LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readUInt16LE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readUInt16BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readUInt16BE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readUInt32LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readUInt32LE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readUInt32BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readUInt32BE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readUIntLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(int32_t, 1, 6);

    hr = pInst->readUIntLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readUIntBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(int32_t, 1, 6);

    hr = pInst->readUIntBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("Buffer.readInt64LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt64LE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("Buffer.readInt64BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt64BE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readInt8");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt8(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readInt16LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt16LE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readInt16BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt16BE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readInt32LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt32LE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.readInt32BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readInt32BE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readIntLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(int32_t, 1, 6);

    hr = pInst->readIntLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readIntBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(int32_t, 1, 6);

    hr = pInst->readIntBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt64LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt64LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt64BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt64BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readFloatLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readFloatLE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readFloatBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readFloatBE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readDoubleLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readDoubleLE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_NAME("Buffer.readDoubleBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->readDoubleBE(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUInt8");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeUInt8(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUInt16LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeUInt16LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUInt16BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeUInt16BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUInt32LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeUInt32LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUInt32BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeUInt32BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUIntLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 6);

    hr = pInst->writeUIntLE(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeUIntBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 6);

    hr = pInst->writeUIntBE(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt8");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt8(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt16LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt16LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt16BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt16BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt32LE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt32LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeInt32BE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->writeInt32BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeIntLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 6);

    hr = pInst->writeIntLE(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeIntBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 6);

    hr = pInst->writeIntBE(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeFloatLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(double, 0);
    ARG(int32_t, 1);

    hr = pInst->writeFloatLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeFloatBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(double, 0);
    ARG(int32_t, 1);

    hr = pInst->writeFloatBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeDoubleLE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(double, 0);
    ARG(int32_t, 1);

    hr = pInst->writeDoubleLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Buffer.writeDoubleBE");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(double, 0);
    ARG(int32_t, 1);

    hr = pInst->writeDoubleBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_slice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.slice");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->slice(v0, vr);

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = pInst->slice(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Buffer.join");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, ",");

    hr = pInst->join(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_reverse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Buffer.reverse");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->reverse(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_equals(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Buffer.equals");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<object_base>, 0);

    hr = pInst->equals(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_hex(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Buffer.hex");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->hex(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_base32(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Buffer.base32");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->base32(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_base58(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Buffer.base58");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->base58(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_base64(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Buffer.base64");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->base64(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_NAME("Buffer.keys");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->keys(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_NAME("Buffer.values");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->values(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_entries(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_NAME("Buffer.entries");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->entries(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Buffer.forEach");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));

    hr = pInst->forEach(v0, v1);

    METHOD_VOID();
}

inline void Buffer_base::s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_NAME("Buffer.toArray");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->toArray(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Buffer.toString");
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    ARG(int32_t, 2);

    hr = pInst->toString(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->toString(v0, v1, vr);

    METHOD_OVER(0, 0);

    hr = pInst->toString(vr);

    METHOD_RETURN();
}
}
