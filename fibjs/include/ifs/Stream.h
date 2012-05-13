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
	virtual result_t write(obj_ptr<Buffer_base> data, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncWrite(obj_ptr<Buffer_base> data) = 0;
	virtual result_t flush(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncFlush() = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncClose() = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

protected:
	static v8::Handle<v8::Value> s_read(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncRead(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_write(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncWrite(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_flush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncFlush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncClose(const v8::Arguments& args);

protected:
	ASYNC_MEMBER2(Stream_base, read);
	ASYNC_VALUEBACK1(Stream_base, read, obj_ptr<Buffer_base>);
	ASYNC_MEMBER1(Stream_base, write);
	ASYNC_CALLBACK1(Stream_base, write);
	ASYNC_MEMBER0(Stream_base, flush);
	ASYNC_CALLBACK0(Stream_base, flush);
	ASYNC_MEMBER0(Stream_base, close);
	ASYNC_CALLBACK0(Stream_base, close);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Stream_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"read", s_read},
			{"asyncRead", s_asyncRead},
			{"write", s_write},
			{"asyncWrite", s_asyncWrite},
			{"flush", s_flush},
			{"asyncFlush", s_asyncFlush},
			{"close", s_close},
			{"asyncClose", s_asyncClose}
		};

		static ClassData s_cd = 
		{ 
			"Stream", NULL, 
			8, s_method, 0, NULL, 0, NULL, NULL,
			&Trigger_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Stream_base::s_read(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_read(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stream_base::s_asyncRead(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->asyncRead(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_write(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_write(s_acPool, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_asyncWrite(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->asyncWrite(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_flush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_flush(s_acPool);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_asyncFlush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncFlush();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close(s_acPool);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_asyncClose(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncClose();

		METHOD_VOID();
	}

}

#endif

