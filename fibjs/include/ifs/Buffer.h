/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Buffer_base_H_
#define _Buffer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Int64_base;

class Buffer_base : public object_base {
    DECLARE_CLASS(Buffer_base);

public:
    // Buffer_base
    static result_t _new(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::ArrayBuffer> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::TypedArray> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Buffer_base* buffer, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string str, exlib::string codec, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(int32_t size, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t from(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal);
    static result_t from(v8::Local<v8::ArrayBuffer> datas, obj_ptr<Buffer_base>& retVal);
    static result_t from(v8::Local<v8::TypedArray> datas, obj_ptr<Buffer_base>& retVal);
    static result_t from(Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
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
    virtual result_t _indexed_getter(uint32_t index, int32_t& retVal) = 0;
    virtual result_t _indexed_setter(uint32_t index, int32_t newVal) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t resize(int32_t sz) = 0;
    virtual result_t append(v8::Local<v8::Array> datas) = 0;
    virtual result_t append(v8::Local<v8::TypedArray> datas) = 0;
    virtual result_t append(v8::Local<v8::ArrayBuffer> datas) = 0;
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
    virtual result_t compare(Buffer_base* buf, int32_t& retVal) = 0;
    virtual result_t copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal) = 0;
    virtual result_t readUInt8(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readUInt16LE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readUInt16BE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readUInt32LE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
    virtual result_t readUInt32BE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
    virtual result_t readUIntLE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
    virtual result_t readUIntBE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
    virtual result_t readInt8(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readInt16LE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readInt16BE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readInt32LE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readInt32BE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
    virtual result_t readIntLE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
    virtual result_t readIntBE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
    virtual result_t readInt64LE(int32_t offset, bool noAssert, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t readInt64BE(int32_t offset, bool noAssert, obj_ptr<Int64_base>& retVal) = 0;
    virtual result_t readFloatLE(int32_t offset, bool noAssert, double& retVal) = 0;
    virtual result_t readFloatBE(int32_t offset, bool noAssert, double& retVal) = 0;
    virtual result_t readDoubleLE(int32_t offset, bool noAssert, double& retVal) = 0;
    virtual result_t readDoubleBE(int32_t offset, bool noAssert, double& retVal) = 0;
    virtual result_t writeUInt8(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeUInt16LE(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeUInt16BE(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeUInt32LE(int64_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeUInt32BE(int64_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeUIntLE(int64_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeUIntBE(int64_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt8(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt16LE(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt16BE(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt32LE(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt32BE(int32_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeIntLE(int64_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeIntBE(int64_t value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt64LE(Int64_base* value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeInt64BE(Int64_base* value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeFloatLE(double value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeFloatBE(double value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeDoubleLE(double value, int32_t offset, bool noAssert) = 0;
    virtual result_t writeDoubleBE(double value, int32_t offset, bool noAssert) = 0;
    virtual result_t slice(int32_t start, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t hex(exlib::string& retVal) = 0;
    virtual result_t base64(exlib::string& retVal) = 0;
    virtual result_t keys(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t values(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t toArray(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t toString(exlib::string codec, int32_t offset, int32_t end, exlib::string& retVal) = 0;
    virtual result_t toString(exlib::string& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_from(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_concat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_alloc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_allocUnsafe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_allocUnsafeSlow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_byteLength(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_resize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_fill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_indexOf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_compare(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readIntLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readIntBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_writeInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hex(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_base64(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_values(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "Int64.h"

namespace fibjs {
inline ClassInfo& Buffer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isBuffer", s_isBuffer, true },
        { "from", s_from, true },
        { "concat", s_concat, true },
        { "alloc", s_alloc, true },
        { "allocUnsafe", s_allocUnsafe, true },
        { "allocUnsafeSlow", s_allocUnsafeSlow, true },
        { "byteLength", s_byteLength, true },
        { "resize", s_resize, false },
        { "append", s_append, false },
        { "write", s_write, false },
        { "fill", s_fill, false },
        { "indexOf", s_indexOf, false },
        { "compare", s_compare, false },
        { "copy", s_copy, false },
        { "readUInt8", s_readUInt8, false },
        { "readUInt16LE", s_readUInt16LE, false },
        { "readUInt16BE", s_readUInt16BE, false },
        { "readUInt32LE", s_readUInt32LE, false },
        { "readUInt32BE", s_readUInt32BE, false },
        { "readUIntLE", s_readUIntLE, false },
        { "readUIntBE", s_readUIntBE, false },
        { "readInt8", s_readInt8, false },
        { "readInt16LE", s_readInt16LE, false },
        { "readInt16BE", s_readInt16BE, false },
        { "readInt32LE", s_readInt32LE, false },
        { "readInt32BE", s_readInt32BE, false },
        { "readIntLE", s_readIntLE, false },
        { "readIntBE", s_readIntBE, false },
        { "readInt64LE", s_readInt64LE, false },
        { "readInt64BE", s_readInt64BE, false },
        { "readFloatLE", s_readFloatLE, false },
        { "readFloatBE", s_readFloatBE, false },
        { "readDoubleLE", s_readDoubleLE, false },
        { "readDoubleBE", s_readDoubleBE, false },
        { "writeUInt8", s_writeUInt8, false },
        { "writeUInt16LE", s_writeUInt16LE, false },
        { "writeUInt16BE", s_writeUInt16BE, false },
        { "writeUInt32LE", s_writeUInt32LE, false },
        { "writeUInt32BE", s_writeUInt32BE, false },
        { "writeUIntLE", s_writeUIntLE, false },
        { "writeUIntBE", s_writeUIntBE, false },
        { "writeInt8", s_writeInt8, false },
        { "writeInt16LE", s_writeInt16LE, false },
        { "writeInt16BE", s_writeInt16BE, false },
        { "writeInt32LE", s_writeInt32LE, false },
        { "writeInt32BE", s_writeInt32BE, false },
        { "writeIntLE", s_writeIntLE, false },
        { "writeIntBE", s_writeIntBE, false },
        { "writeInt64LE", s_writeInt64LE, false },
        { "writeInt64BE", s_writeInt64BE, false },
        { "writeFloatLE", s_writeFloatLE, false },
        { "writeFloatBE", s_writeFloatBE, false },
        { "writeDoubleLE", s_writeDoubleLE, false },
        { "writeDoubleBE", s_writeDoubleBE, false },
        { "slice", s_slice, false },
        { "hex", s_hex, false },
        { "base64", s_base64, false },
        { "keys", s_keys, false },
        { "values", s_values, false },
        { "toArray", s_toArray, false },
        { "toString", s_toString, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "length", s_get_length, block_set, false }
    };

    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData s_cd = {
        "Buffer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, &s_indexed, NULL,
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

inline void Buffer_base::s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBuffer(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_from(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = from(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::ArrayBuffer>, 0);

    hr = from(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::TypedArray>, 0);

    hr = from(v0, vr);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = from(v0, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = from(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_concat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Array>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = concat(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_alloc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

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

inline void Buffer_base::s_allocUnsafe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = allocUnsafe(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_allocUnsafeSlow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = allocUnsafeSlow(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_byteLength(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

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

inline void Buffer_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);

    METHOD_RETURN();
}

inline void Buffer_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    PROPERTY_VAL(int32_t);
    hr = pInst->_indexed_setter(index, v0);

    METHOD_VOID();
}

inline void Buffer_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    PROPERTY_ENTER();

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_resize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->resize(v0);

    METHOD_VOID();
}

inline void Buffer_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->append(v0);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::TypedArray>, 0);

    hr = pInst->append(v0);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::ArrayBuffer>, 0);

    hr = pInst->append(v0);

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

inline void Buffer_base::s_compare(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->compare(v0, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

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

inline void Buffer_base::s_readUInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUInt8(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUInt16LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUInt16BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUInt32LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUInt32BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUIntLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readUIntBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt8(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt16LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt16BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt32LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt32BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readIntLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readIntBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Int64_base> vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt64LE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Int64_base> vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readInt64BE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readFloatLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readFloatBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readDoubleLE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_readDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(bool, 1, false);

    hr = pInst->readDoubleBE(v0, v1, vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_writeUInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUInt8(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUInt16LE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUInt16BE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUInt32LE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUInt32BE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeUIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUIntLE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeUIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeUIntBE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt8(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt16LE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt16BE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt32LE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt32BE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeIntLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeIntLE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeIntBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeIntBE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(obj_ptr<Int64_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt64LE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(obj_ptr<Int64_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeInt64BE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(double, 0);
    ARG(int32_t, 1);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeFloatLE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(double, 0);
    ARG(int32_t, 1);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeFloatBE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(double, 0);
    ARG(int32_t, 1);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeDoubleLE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_writeDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(double, 0);
    ARG(int32_t, 1);
    OPT_ARG(bool, 2, false);

    hr = pInst->writeDoubleBE(v0, v1, v2);

    METHOD_VOID();
}

inline void Buffer_base::s_slice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

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

inline void Buffer_base::s_hex(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->hex(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_base64(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->base64(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->keys(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->values(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->toArray(vr);

    METHOD_RETURN();
}

inline void Buffer_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Buffer_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    hr = pInst->toString(v0, v1, v2, vr);

    METHOD_OVER(0, 0);

    hr = pInst->toString(vr);

    METHOD_RETURN();
}
}

#endif
