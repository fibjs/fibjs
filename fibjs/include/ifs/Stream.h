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
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;
class Buffer_base;

class Stream_base : public Trigger_base
{
public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncRead(int32_t bytes) = 0;
	virtual result_t onread(v8::Handle<v8::Function> func) = 0;
	virtual result_t write(Buffer_base* data, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncWrite(Buffer_base* data) = 0;
	virtual result_t onwrite(v8::Handle<v8::Function> func) = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncClose() = 0;
	virtual result_t onclose(v8::Handle<v8::Function> func) = 0;
	virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncCopyTo(Stream_base* stm, int64_t bytes) = 0;
	virtual result_t oncopyto(v8::Handle<v8::Function> func) = 0;
	virtual result_t onerror(v8::Handle<v8::Function> func) = 0;

	DECLARE_CLASSINFO(Stream_base);

public:
	static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncRead(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_onread(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncWrite(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_onwrite(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncClose(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_copyTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncCopyTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_oncopyto(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE2(Stream_base, read, int32_t, obj_ptr<Buffer_base>);
	ASYNC_VALUEBACK1(Stream_base, read, obj_ptr<Buffer_base>);
	ASYNC_MEMBER1(Stream_base, write, Buffer_base*);
	ASYNC_CALLBACK1(Stream_base, write);
	ASYNC_MEMBER0(Stream_base, close);
	ASYNC_CALLBACK0(Stream_base, close);
	ASYNC_MEMBERVALUE3(Stream_base, copyTo, Stream_base*, int64_t, int64_t);
	ASYNC_VALUEBACK2(Stream_base, copyTo, int64_t);
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
			{"asyncRead", s_asyncRead},
			{"onread", s_onread},
			{"write", s_write},
			{"asyncWrite", s_asyncWrite},
			{"onwrite", s_onwrite},
			{"close", s_close},
			{"asyncClose", s_asyncClose},
			{"onclose", s_onclose},
			{"copyTo", s_copyTo},
			{"asyncCopyTo", s_asyncCopyTo},
			{"oncopyto", s_oncopyto},
			{"onerror", s_onerror}
		};

		static ClassData s_cd = 
		{ 
			"Stream", NULL, 
			13, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Trigger_base::class_info()
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

	inline void Stream_base::s_asyncRead(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->asyncRead(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_onread(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onread(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_write(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_asyncWrite(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->asyncWrite(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_onwrite(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onwrite(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

	inline void Stream_base::s_asyncClose(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncClose();

		METHOD_VOID();
	}

	inline void Stream_base::s_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onclose(v0);

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

	inline void Stream_base::s_asyncCopyTo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Stream_base>, 0);
		OPT_ARG(int64_t, 1, -1);

		hr = pInst->asyncCopyTo(v0, v1);

		METHOD_VOID();
	}

	inline void Stream_base::s_oncopyto(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->oncopyto(v0);

		METHOD_VOID();
	}

	inline void Stream_base::s_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onerror(v0);

		METHOD_VOID();
	}

}

#endif

