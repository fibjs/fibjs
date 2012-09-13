/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _zlib_base_H_
#define _zlib_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Buffer_base;

class zlib_base : public module_base
{
public:
	enum{
		_NO_COMPRESSION = 0,
		_Z_BEST_SPEED = 1,
		_Z_BEST_COMPRESSION = 9,
		_Z_DEFAULT_COMPRESSION = -1
	};

public:
	// zlib_base
	static result_t deflate(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	static result_t inflate(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	static result_t gunzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	static result_t gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(zlib_base);

public:
	static v8::Handle<v8::Value> s_get_NO_COMPRESSION(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_Z_BEST_SPEED(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_Z_BEST_COMPRESSION(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_Z_DEFAULT_COMPRESSION(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_deflate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_inflate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_gunzip(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_gzip(const v8::Arguments& args);

public:
	ASYNC_STATIC3(zlib_base, deflate);
	ASYNC_STATIC2(zlib_base, inflate);
	ASYNC_STATIC2(zlib_base, gunzip);
	ASYNC_STATIC2(zlib_base, gzip);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& zlib_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"deflate", s_deflate, true},
			{"inflate", s_inflate, true},
			{"gunzip", s_gunzip, true},
			{"gzip", s_gzip, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"NO_COMPRESSION", s_get_NO_COMPRESSION, NULL, true},
			{"Z_BEST_SPEED", s_get_Z_BEST_SPEED, NULL, true},
			{"Z_BEST_COMPRESSION", s_get_Z_BEST_COMPRESSION, NULL, true},
			{"Z_DEFAULT_COMPRESSION", s_get_Z_DEFAULT_COMPRESSION, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"zlib", NULL, 
			4, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> zlib_base::s_get_NO_COMPRESSION(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _NO_COMPRESSION;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_get_Z_BEST_SPEED(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _Z_BEST_SPEED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_get_Z_BEST_COMPRESSION(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _Z_BEST_COMPRESSION;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_get_Z_DEFAULT_COMPRESSION(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _Z_DEFAULT_COMPRESSION;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_deflate(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Buffer_base>, 0);
		OPT_ARG(int32_t, 1, _Z_DEFAULT_COMPRESSION);

		hr = ac_deflate(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_inflate(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ac_inflate(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_gunzip(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ac_gunzip(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zlib_base::s_gzip(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ac_gzip(v0, vr);

		METHOD_RETURN();
	}

}

#endif

