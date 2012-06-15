/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _BufferedReader_base_H_
#define _BufferedReader_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Stream_base;
class Buffer_base;

class BufferedReader_base : public object_base
{
public:
	// BufferedReader_base
	static result_t _new(obj_ptr<Stream_base>& stm, obj_ptr<BufferedReader_base>& retVal);
	virtual result_t readText(int32_t size, std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readLine(std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t readUntil(const char* mk, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

protected:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readText(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readLine(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_read(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readUntil(const v8::Arguments& args);

protected:
	ASYNC_MEMBER2(BufferedReader_base, readText);
	ASYNC_VALUEBACK1(BufferedReader_base, readText, std::string);
	ASYNC_MEMBER1(BufferedReader_base, readLine);
	ASYNC_VALUEBACK0(BufferedReader_base, readLine, std::string);
	ASYNC_MEMBER2(BufferedReader_base, read);
	ASYNC_VALUEBACK1(BufferedReader_base, read, obj_ptr<Buffer_base>);
	ASYNC_MEMBER2(BufferedReader_base, readUntil);
	ASYNC_VALUEBACK1(BufferedReader_base, readUntil, obj_ptr<Buffer_base>);
};

}

#include "Stream.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& BufferedReader_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"readText", s_readText},
			{"readLine", s_readLine},
			{"read", s_read},
			{"readUntil", s_readUntil}
		};

		static ClassData s_cd = 
		{ 
			"BufferedReader", s__new, 
			4, s_method, 0, NULL, 0, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> BufferedReader_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<BufferedReader_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedReader_base::s_readText(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedReader_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_readText(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedReader_base::s_readLine(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(BufferedReader_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_readLine(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedReader_base::s_read(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(BufferedReader_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_read(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> BufferedReader_base::s_readUntil(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(BufferedReader_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->ac_readUntil(s_acPool, v0, vr);

		METHOD_RETURN();
	}

}

#endif

