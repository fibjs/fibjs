/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fs_base_H_
#define _fs_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Stat_base;
class List_base;
class File_base;

class fs_base : public module_base
{
public:
	enum{
		_SEEK_SET = 0,
		_SEEK_CUR = 1,
		_SEEK_END = 2
	};

public:
	// fs_base
	static result_t exists(const char* path, bool& retVal, exlib::AsyncEvent* ac);
	static result_t unlink(const char* path, exlib::AsyncEvent* ac);
	static result_t mkdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rmdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rename(const char* from, const char* to, exlib::AsyncEvent* ac);
	static result_t stat(const char* path, obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readdir(const char* path, obj_ptr<List_base>& retVal, exlib::AsyncEvent* ac);
	static result_t open(const char* fname, const char* mode, obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t tmpFile(obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readFile(const char* fname, std::string& retVal, exlib::AsyncEvent* ac);
	static result_t writeFile(const char* fname, const char* txt, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(fs_base);

public:
	static v8::Handle<v8::Value> s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_exists(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_unlink(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_mkdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rmdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rename(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_stat(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tmpFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_writeFile(const v8::Arguments& args);

public:
	ASYNC_STATICVALUE2(fs_base, exists, const char*, bool);
	ASYNC_STATIC1(fs_base, unlink, const char*);
	ASYNC_STATIC1(fs_base, mkdir, const char*);
	ASYNC_STATIC1(fs_base, rmdir, const char*);
	ASYNC_STATIC2(fs_base, rename, const char*, const char*);
	ASYNC_STATICVALUE2(fs_base, stat, const char*, obj_ptr<Stat_base>);
	ASYNC_STATICVALUE2(fs_base, readdir, const char*, obj_ptr<List_base>);
	ASYNC_STATICVALUE3(fs_base, open, const char*, const char*, obj_ptr<File_base>);
	ASYNC_STATICVALUE1(fs_base, tmpFile, obj_ptr<File_base>);
	ASYNC_STATICVALUE2(fs_base, readFile, const char*, std::string);
	ASYNC_STATIC2(fs_base, writeFile, const char*, const char*);
};

}

#include "Stat.h"
#include "List.h"
#include "File.h"

namespace fibjs
{
	inline ClassInfo& fs_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"exists", s_exists, true},
			{"unlink", s_unlink, true},
			{"mkdir", s_mkdir, true},
			{"rmdir", s_rmdir, true},
			{"rename", s_rename, true},
			{"stat", s_stat, true},
			{"readdir", s_readdir, true},
			{"open", s_open, true},
			{"tmpFile", s_tmpFile, true},
			{"readFile", s_readFile, true},
			{"writeFile", s_writeFile, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"SEEK_SET", s_get_SEEK_SET, NULL, true},
			{"SEEK_CUR", s_get_SEEK_CUR, NULL, true},
			{"SEEK_END", s_get_SEEK_END, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"fs", NULL, 
			11, s_method, 0, NULL, 3, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> fs_base::s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SEEK_SET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SEEK_CUR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SEEK_END;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_exists(const v8::Arguments& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_exists(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_unlink(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_unlink(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_mkdir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_mkdir(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_rmdir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_rmdir(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_rename(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_rename(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_stat(const v8::Arguments& args)
	{
		obj_ptr<Stat_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_stat(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_readdir(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_readdir(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_open(const v8::Arguments& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		hr = ac_open(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_tmpFile(const v8::Arguments& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(0, 0);

		hr = ac_tmpFile(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_readFile(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_readFile(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_writeFile(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_writeFile(v0, v1);

		METHOD_VOID();
	}

}

#endif

