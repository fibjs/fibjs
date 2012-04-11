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
	static const int32_t SEEK_SET = 0;
	static const int32_t SEEK_CUR = 1;
	static const int32_t SEEK_END = 2;

public:
	// Stream_base
	virtual result_t read(double bytes, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t write(obj_ptr<Buffer_base> data) = 0;
	virtual result_t flush() = 0;
	virtual result_t close() = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"read", s_read},
			{"write", s_write},
			{"flush", s_flush},
			{"close", s_close}
		};

		static ClassProperty s_property[] = 
		{
			{"SEEK_SET", s_get_SEEK_SET},
			{"SEEK_CUR", s_get_SEEK_CUR},
			{"SEEK_END", s_get_SEEK_END}
		};

		static ClassData s_cd = 
		{ 
			"Stream", NULL, 
			4, s_method, 3, s_property, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_read(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_write(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_flush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);

private:
	ASYNC_MEMBER2(Stream_base, read);
	ASYNC_MEMBER1(Stream_base, write);
	ASYNC_MEMBER0(Stream_base, flush);
	ASYNC_MEMBER0(Stream_base, close);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> Stream_base::s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = SEEK_SET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stream_base::s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = SEEK_CUR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stream_base::s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = SEEK_END;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stream_base::s_read(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(double, 0, -1);

		hr = pInst->ac_read(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Stream_base::s_write(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_write(s_acPool, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_flush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_flush(s_acPool);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stream_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close(s_acPool);

		METHOD_VOID();
	}

}

#endif

