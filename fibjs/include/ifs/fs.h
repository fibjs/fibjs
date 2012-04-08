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

namespace fibjs
{

class File_base;
class Stat_base;

class fs_base : public object_base
{
public:
	static const int32_t SEEK_SET = 0;
	static const int32_t SEEK_CUR = 1;
	static const int32_t SEEK_END = 2;

public:
	// fs_base
	static result_t open(const char* fname, const char* mode, obj_ptr<File_base>& retVal);
	static result_t create(const char* fname, bool Overwrite, obj_ptr<File_base>& retVal);
	static result_t tmpFile(obj_ptr<File_base>& retVal);
	static result_t readFile(const char* fname, std::string& retVal);
	static result_t writeFile(const char* fname, const char* txt);
	static result_t exists(const char* path, bool& retVal);
	static result_t mkdir(const char* path);
	static result_t rmdir(const char* path);
	static result_t rename(const char* from, const char* to);
	static result_t stat(const char* path, obj_ptr<Stat_base>& retVal);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"open", s_open},
			{"create", s_create},
			{"tmpFile", s_tmpFile},
			{"readFile", s_readFile},
			{"writeFile", s_writeFile},
			{"exists", s_exists},
			{"mkdir", s_mkdir},
			{"rmdir", s_rmdir},
			{"rename", s_rename},
			{"stat", s_stat}
		};

		static ClassProperty s_property[] = 
		{
			{"SEEK_SET", s_get_SEEK_SET},
			{"SEEK_CUR", s_get_SEEK_CUR},
			{"SEEK_END", s_get_SEEK_END}
		};

		static ClassData s_cd = 
		{ 
			"fs", NULL, 
			10, s_method, 3, s_property, NULL,
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
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_create(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tmpFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_writeFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_exists(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_mkdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rmdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rename(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_stat(const v8::Arguments& args);

private:
	ASYNC_STATIC3(fs_base, open);
	ASYNC_STATIC3(fs_base, create);
	ASYNC_STATIC1(fs_base, tmpFile);
	ASYNC_STATIC2(fs_base, readFile);
	ASYNC_STATIC2(fs_base, writeFile);
	ASYNC_STATIC2(fs_base, exists);
	ASYNC_STATIC1(fs_base, mkdir);
	ASYNC_STATIC1(fs_base, rmdir);
	ASYNC_STATIC2(fs_base, rename);
	ASYNC_STATIC2(fs_base, stat);
};

}

#include "File.h"
#include "Stat.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> fs_base::s_get_SEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = SEEK_SET;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = SEEK_CUR;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_get_SEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = SEEK_END;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_open(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		obj_ptr<File_base> vr;
		hr = ac_open(s_acPool, v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_create(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(bool, 1, true);

		obj_ptr<File_base> vr;
		hr = ac_create(s_acPool, v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_tmpFile(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		obj_ptr<File_base> vr;
		hr = ac_tmpFile(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_readFile(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		std::string vr;
		hr = ac_readFile(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_writeFile(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_writeFile(s_acPool, v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_exists(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		bool vr;
		hr = ac_exists(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_mkdir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_mkdir(s_acPool, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_rmdir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_rmdir(s_acPool, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_rename(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_rename(s_acPool, v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> fs_base::s_stat(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		obj_ptr<Stat_base> vr;
		hr = ac_stat(s_acPool, v0, vr);

		METHOD_RETURN();
	}

}

#endif

