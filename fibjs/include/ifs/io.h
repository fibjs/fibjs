/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _io_base_H_
#define _io_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class File_base;
class MemoryStream_base;
class BufferedStream_base;

class io_base : public module_base
{
public:
	enum{
		_SEEK_SET = 0,
		_SEEK_CUR = 1,
		_SEEK_END = 2
	};

public:
	// io_base
	static result_t open(const char* fname, const char* mode, obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t create(const char* fname, bool Overwrite, obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t tmpFile(obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readFile(const char* fname, std::string& retVal, exlib::AsyncEvent* ac);
	static result_t writeFile(const char* fname, const char* txt, exlib::AsyncEvent* ac);

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_create(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tmpFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_writeFile(const v8::Arguments& args);

public:
	ASYNC_STATIC3(io_base, open);
	ASYNC_STATIC3(io_base, create);
	ASYNC_STATIC1(io_base, tmpFile);
	ASYNC_STATIC2(io_base, readFile);
	ASYNC_STATIC2(io_base, writeFile);
};

}

#include "File.h"
#include "MemoryStream.h"
#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& io_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"open", s_open, true},
			{"create", s_create, true},
			{"tmpFile", s_tmpFile, true},
			{"readFile", s_readFile, true},
			{"writeFile", s_writeFile, true}
		};

		static ClassObject s_object[] = 
		{
			{"File", File_base::class_info},
			{"MemoryStream", MemoryStream_base::class_info},
			{"BufferedStream", BufferedStream_base::class_info}
		};

		static ClassProperty s_property[] = 
		{
			{"SEEK_SET", s_get_SEEK_SET, NULL, true},
			{"SEEK_CUR", s_get_SEEK_CUR, NULL, true},
			{"SEEK_END", s_get_SEEK_END, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"io", NULL, 
			5, s_method, 3, s_object, 3, s_property, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> io_base::s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SEEK_SET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SEEK_CUR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SEEK_END;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_open(const v8::Arguments& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		hr = ac_open(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_create(const v8::Arguments& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(bool, 1, true);

		hr = ac_create(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_tmpFile(const v8::Arguments& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(0, 0);

		hr = ac_tmpFile(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_readFile(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_readFile(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> io_base::s_writeFile(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_writeFile(v0, v1);

		METHOD_VOID();
	}

}

#endif

