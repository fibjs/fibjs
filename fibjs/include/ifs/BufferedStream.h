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
	static result_t _new(obj_ptr<Stream_base>& stm, obj_ptr<BufferedStream_base>& retVal);
	virtual result_t readText(int32_t size, std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readLine(std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readUntil(int32_t mk, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t get_EOL(std::string& retVal) = 0;
	virtual result_t set_EOL(const char* newVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Stream_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(EOL);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readText(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readLine(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readUntil(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_EOL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_EOL(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

public:
	ASYNC_MEMBER2(BufferedStream_base, readText);
	ASYNC_VALUEBACK1(BufferedStream_base, readText, std::string);
	ASYNC_MEMBER1(BufferedStream_base, readLine);
	ASYNC_VALUEBACK0(BufferedStream_base, readLine, std::string);
	ASYNC_MEMBER2(BufferedStream_base, readUntil);
	ASYNC_VALUEBACK1(BufferedStream_base, readUntil, obj_ptr<Buffer_base>);
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
			{"readUntil", s_readUntil}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"EOL", s_get_EOL, s_set_EOL}
		};

		static ClassData s_cd = 
		{ 
			"BufferedStream", s__new, 
			3, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> BufferedStream_base::s_get_EOL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(BufferedStream_base);

		hr = pInst->get_EOL(vr);

		METHOD_RETURN();
	}

	inline void BufferedStream_base::s_set_EOL(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(BufferedStream_base);

		PROPERTY_VAL_String();
		hr = pInst->set_EOL(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> BufferedStream_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<BufferedStream_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedStream_base::s_readText(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_readText(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedStream_base::s_readLine(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_readLine(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedStream_base::s_readUntil(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(BufferedStream_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_readUntil(v0, vr);

		METHOD_RETURN();
	}

}

#endif

