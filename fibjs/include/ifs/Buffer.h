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

namespace fibjs
{

class Int64_base;

class Buffer_base : public object_base
{
	DECLARE_CLASS(Buffer_base);

public:
	// Buffer_base
	static result_t _new(v8::Local<v8::Array> datas, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(const char* str, const char* codec, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(int32_t size, obj_ptr<Buffer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t _indexed_getter(uint32_t index, int32_t& retVal) = 0;
	virtual result_t _indexed_setter(uint32_t index, int32_t newVal) = 0;
	virtual result_t get_length(int32_t& retVal) = 0;
	virtual result_t resize(int32_t sz) = 0;
	virtual result_t write(v8::Local<v8::Array> datas) = 0;
	virtual result_t write(Buffer_base* data) = 0;
	virtual result_t write(const char* str, const char* codec) = 0;
	virtual result_t copy(Buffer_base* targetBuffer, int32_t targetStart, int32_t sourceStart, int32_t sourceEnd, int32_t& retVal) = 0;
	virtual result_t readUInt8(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readUInt16LE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readUInt16BE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readUInt32LE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
	virtual result_t readUInt32BE(int32_t offset, bool noAssert, int64_t& retVal) = 0;
	virtual result_t readInt8(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readInt16LE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readInt16BE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readInt32LE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
	virtual result_t readInt32BE(int32_t offset, bool noAssert, int32_t& retVal) = 0;
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
	virtual result_t writeInt8(int32_t value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeInt16LE(int32_t value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeInt16BE(int32_t value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeInt32LE(int32_t value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeInt32BE(int32_t value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeInt64LE(Int64_base* value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeInt64BE(Int64_base* value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeFloatLE(double value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeFloatBE(double value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeDoubleLE(double value, int32_t offset, bool noAssert) = 0;
	virtual result_t writeDoubleBE(double value, int32_t offset, bool noAssert) = 0;
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t hex(std::string& retVal) = 0;
	virtual result_t base64(std::string& retVal) = 0;
	virtual result_t toString(const char* codec, std::string& retVal) = 0;
	virtual result_t toString(std::string& retVal) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_resize(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copy(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readUInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
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
	static void s_writeInt8(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_slice(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hex(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_base64(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Int64.h"

namespace fibjs
{
	inline ClassInfo& Buffer_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"resize", s_resize, false},
			{"write", s_write, false},
			{"copy", s_copy, false},
			{"readUInt8", s_readUInt8, false},
			{"readUInt16LE", s_readUInt16LE, false},
			{"readUInt16BE", s_readUInt16BE, false},
			{"readUInt32LE", s_readUInt32LE, false},
			{"readUInt32BE", s_readUInt32BE, false},
			{"readInt8", s_readInt8, false},
			{"readInt16LE", s_readInt16LE, false},
			{"readInt16BE", s_readInt16BE, false},
			{"readInt32LE", s_readInt32LE, false},
			{"readInt32BE", s_readInt32BE, false},
			{"readInt64LE", s_readInt64LE, false},
			{"readInt64BE", s_readInt64BE, false},
			{"readFloatLE", s_readFloatLE, false},
			{"readFloatBE", s_readFloatBE, false},
			{"readDoubleLE", s_readDoubleLE, false},
			{"readDoubleBE", s_readDoubleBE, false},
			{"writeUInt8", s_writeUInt8, false},
			{"writeUInt16LE", s_writeUInt16LE, false},
			{"writeUInt16BE", s_writeUInt16BE, false},
			{"writeUInt32LE", s_writeUInt32LE, false},
			{"writeUInt32BE", s_writeUInt32BE, false},
			{"writeInt8", s_writeInt8, false},
			{"writeInt16LE", s_writeInt16LE, false},
			{"writeInt16BE", s_writeInt16BE, false},
			{"writeInt32LE", s_writeInt32LE, false},
			{"writeInt32BE", s_writeInt32BE, false},
			{"writeInt64LE", s_writeInt64LE, false},
			{"writeInt64BE", s_writeInt64BE, false},
			{"writeFloatLE", s_writeFloatLE, false},
			{"writeFloatBE", s_writeFloatBE, false},
			{"writeDoubleLE", s_writeDoubleLE, false},
			{"writeDoubleBE", s_writeDoubleBE, false},
			{"slice", s_slice, false},
			{"hex", s_hex, false},
			{"base64", s_base64, false},
			{"toString", s_toString, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"length", s_get_length, block_set, false}
		};

		static ClassData::ClassIndexed s_indexed = 
		{
			i_IndexedGetter, i_IndexedSetter
		};

		static ClassData s_cd = 
		{ 
			"Buffer", s__new, 
			39, s_method, 0, NULL, 1, s_property, &s_indexed, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Buffer_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Buffer_base);

		hr = pInst->_indexed_getter(index, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Buffer_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->_indexed_setter(index, v0);

		METHOD_VOID();
	}

	inline void Buffer_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Buffer_base);

		hr = pInst->get_length(vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CONSTRUCT_INIT();
		__new(args);
	}

	template<typename T>void Buffer_base::__new(const T& args)
	{
		obj_ptr<Buffer_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = _new(v0, vr, args.This());

		METHOD_OVER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(arg_string, 1, "utf8");

		hr = _new(v0, v1, vr, args.This());

		METHOD_OVER(1, 0);

		OPT_ARG(int32_t, 0, 0);

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void Buffer_base::s_resize(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->resize(v0);

		METHOD_VOID();
	}

	inline void Buffer_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->write(v0);

		METHOD_OVER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->write(v0);

		METHOD_OVER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(arg_string, 1, "utf8");

		hr = pInst->write(v0, v1);

		METHOD_VOID();
	}

	inline void Buffer_base::s_copy(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(4, 1);

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
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readUInt8(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readUInt16LE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readUInt16BE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readUInt32LE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readUInt32BE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt8(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt16LE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt16BE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt32LE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt32BE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Int64_base> vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt64LE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Int64_base> vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readInt64BE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		double vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readFloatLE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		double vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readFloatBE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		double vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readDoubleLE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_readDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		double vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->readDoubleBE(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_writeUInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeUInt8(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeUInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeUInt16LE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeUInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeUInt16BE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeUInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int64_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeUInt32LE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeUInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int64_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeUInt32BE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt8(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt8(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt16LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt16LE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt16BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt16BE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt32LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt32LE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt32BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt32BE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt64LE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(obj_ptr<Int64_base>, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt64LE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeInt64BE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(obj_ptr<Int64_base>, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeInt64BE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeFloatLE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(double, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeFloatLE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeFloatBE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(double, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeFloatBE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeDoubleLE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

		ARG(double, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->writeDoubleLE(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Buffer_base::s_writeDoubleBE(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(3, 2);

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
		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, 0);
		OPT_ARG(int32_t, 1, -1);

		hr = pInst->slice(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_hex(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->hex(vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_base64(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->base64(vr);

		METHOD_RETURN();
	}

	inline void Buffer_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Buffer_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->toString(v0, vr);

		METHOD_OVER(0, 0);

		hr = pInst->toString(vr);

		METHOD_RETURN();
	}

}

#endif

