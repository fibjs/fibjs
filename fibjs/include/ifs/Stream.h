/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Stream_base_H_
#define _Stream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class Stream_base : public object_base
{
public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t write(Buffer_base* data, exlib::AsyncEvent* ac) = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(Stream_base);

public:
	static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyTo(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE2(Stream_base, read, int32_t, obj_ptr<Buffer_base>);
	ASYNC_MEMBER1(Stream_base, write, Buffer_base*);
	ASYNC_MEMBER0(Stream_base, close);
	ASYNC_MEMBERVALUE3(Stream_base, copyTo, Stream_base*, int64_t, int64_t);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Stream_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"read", s_read},
			{"write", s_write},
			{"close", s_close},
			{"copyTo", s_copyTo}
		};

		static ClassData s_cd = 
		{ 
			"Stream", NULL, 
			4, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Stream_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_read(v0, vr);

		METHOD_RETURN();
	}

	inline void Stream_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_write(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

	inline void Stream_base::s_copyTo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Stream_base>, 0);
		OPT_ARG(int64_t, 1, -1);

		hr = pInst->ac_copyTo(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

