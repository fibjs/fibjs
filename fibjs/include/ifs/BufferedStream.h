/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _BufferedStream_base_H_
#define _BufferedStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs
{

class Stream_base;
class Buffer_base;

class BufferedStream_base : public Stream_base
{
public:
	// BufferedStream_base
	static result_t _new(Stream_base* stm, obj_ptr<BufferedStream_base>& retVal);
	virtual result_t readText(int32_t size, std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readLine(int32_t maxlen, std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readLines(int32_t maxlines, v8::Local<v8::Array>& retVal) = 0;
	virtual result_t readUntil(const char* mk, int32_t maxlen, std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readPacket(int32_t limit, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t writeText(const char* txt, exlib::AsyncEvent* ac) = 0;
	virtual result_t writeLine(const char* txt, exlib::AsyncEvent* ac) = 0;
	virtual result_t writePacket(Buffer_base* data, exlib::AsyncEvent* ac) = 0;
	virtual result_t get_stream(obj_ptr<Stream_base>& retVal) = 0;
	virtual result_t get_EOL(std::string& retVal) = 0;
	virtual result_t set_EOL(const char* newVal) = 0;

	DECLARE_CLASSINFO(BufferedStream_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readText(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readLine(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readLines(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readUntil(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readPacket(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeText(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeLine(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writePacket(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_stream(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_EOL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_EOL(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);

public:
	ASYNC_MEMBERVALUE2(BufferedStream_base, readText, int32_t, std::string);
	ASYNC_MEMBERVALUE2(BufferedStream_base, readLine, int32_t, std::string);
	ASYNC_MEMBERVALUE3(BufferedStream_base, readUntil, const char*, int32_t, std::string);
	ASYNC_MEMBERVALUE2(BufferedStream_base, readPacket, int32_t, obj_ptr<Buffer_base>);
	ASYNC_MEMBER1(BufferedStream_base, writeText, const char*);
	ASYNC_MEMBER1(BufferedStream_base, writeLine, const char*);
	ASYNC_MEMBER1(BufferedStream_base, writePacket, Buffer_base*);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& BufferedStream_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"readText", s_readText},
			{"readLine", s_readLine},
			{"readLines", s_readLines},
			{"readUntil", s_readUntil},
			{"readPacket", s_readPacket},
			{"writeText", s_writeText},
			{"writeLine", s_writeLine},
			{"writePacket", s_writePacket}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"stream", s_get_stream, block_set},
			{"EOL", s_get_EOL, s_set_EOL}
		};

		static ClassData s_cd = 
		{ 
			"BufferedStream", s__new, 
			8, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void BufferedStream_base::s_get_stream(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Stream_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(BufferedStream_base);

		hr = pInst->get_stream(vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_get_EOL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(BufferedStream_base);

		hr = pInst->get_EOL(vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_set_EOL(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(BufferedStream_base);

		PROPERTY_VAL(arg_string);
		hr = pInst->set_EOL(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void BufferedStream_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<BufferedStream_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline void BufferedStream_base::s_readText(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_readText(v0, vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_readLine(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_readLine(v0, vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_readLines(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->readLines(v0, vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_readUntil(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(int32_t, 1, -1);

		hr = pInst->ac_readUntil(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_readPacket(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_readPacket(v0, vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_writeText(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_writeText(v0);

		METHOD_VOID();
	}

	inline void BufferedStream_base::s_writeLine(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_writeLine(v0);

		METHOD_VOID();
	}

	inline void BufferedStream_base::s_writePacket(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_writePacket(v0);

		METHOD_VOID();
	}

}

#endif

