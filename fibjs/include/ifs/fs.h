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
class BufferedStream_base;

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
	static result_t umask(int32_t mask, exlib::AsyncEvent* ac);
	static result_t mkdir(const char* path, int32_t mode, exlib::AsyncEvent* ac);
	static result_t rmdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rename(const char* from, const char* to, exlib::AsyncEvent* ac);
	static result_t chmod(const char* path, int32_t mode, exlib::AsyncEvent* ac);
	static result_t stat(const char* path, obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readdir(const char* path, obj_ptr<List_base>& retVal, exlib::AsyncEvent* ac);
	static result_t open(const char* fname, const char* flags, obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t tmpFile(obj_ptr<File_base>& retVal, exlib::AsyncEvent* ac);
	static result_t openTextStream(const char* fname, const char* flags, obj_ptr<BufferedStream_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readFile(const char* fname, std::string& retVal, exlib::AsyncEvent* ac);
	static result_t writeFile(const char* fname, const char* txt, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(fs_base);

public:
	static void s_get_SEEK_SET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SEEK_END(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_unlink(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_umask(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readdir(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_tmpFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE2(fs_base, exists, const char*, bool);
	ASYNC_STATIC1(fs_base, unlink, const char*);
	ASYNC_STATIC1(fs_base, umask, int32_t);
	ASYNC_STATIC2(fs_base, mkdir, const char*, int32_t);
	ASYNC_STATIC1(fs_base, rmdir, const char*);
	ASYNC_STATIC2(fs_base, rename, const char*, const char*);
	ASYNC_STATIC2(fs_base, chmod, const char*, int32_t);
	ASYNC_STATICVALUE2(fs_base, stat, const char*, obj_ptr<Stat_base>);
	ASYNC_STATICVALUE2(fs_base, readdir, const char*, obj_ptr<List_base>);
	ASYNC_STATICVALUE3(fs_base, open, const char*, const char*, obj_ptr<File_base>);
	ASYNC_STATICVALUE1(fs_base, tmpFile, obj_ptr<File_base>);
	ASYNC_STATICVALUE3(fs_base, openTextStream, const char*, const char*, obj_ptr<BufferedStream_base>);
	ASYNC_STATICVALUE2(fs_base, readFile, const char*, std::string);
	ASYNC_STATIC2(fs_base, writeFile, const char*, const char*);
};

}

#include "Stat.h"
#include "List.h"
#include "File.h"
#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& fs_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"exists", s_exists, true},
			{"unlink", s_unlink, true},
			{"umask", s_umask, true},
			{"mkdir", s_mkdir, true},
			{"rmdir", s_rmdir, true},
			{"rename", s_rename, true},
			{"chmod", s_chmod, true},
			{"stat", s_stat, true},
			{"readdir", s_readdir, true},
			{"open", s_open, true},
			{"tmpFile", s_tmpFile, true},
			{"openTextStream", s_openTextStream, true},
			{"readFile", s_readFile, true},
			{"writeFile", s_writeFile, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"SEEK_SET", s_get_SEEK_SET, block_set, true},
			{"SEEK_CUR", s_get_SEEK_CUR, block_set, true},
			{"SEEK_END", s_get_SEEK_END, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"fs", NULL, 
			14, s_method, 0, NULL, 3, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void fs_base::s_get_SEEK_SET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SEEK_SET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void fs_base::s_get_SEEK_CUR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SEEK_CUR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void fs_base::s_get_SEEK_END(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SEEK_END;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void fs_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_exists(v0, vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_unlink(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_unlink(v0);

		METHOD_VOID();
	}

	inline void fs_base::s_umask(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = ac_umask(v0);

		METHOD_VOID();
	}

	inline void fs_base::s_mkdir(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(int32_t, 1, 0777);

		hr = ac_mkdir(v0, v1);

		METHOD_VOID();
	}

	inline void fs_base::s_rmdir(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_rmdir(v0);

		METHOD_VOID();
	}

	inline void fs_base::s_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_rename(v0, v1);

		METHOD_VOID();
	}

	inline void fs_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = ac_chmod(v0, v1);

		METHOD_VOID();
	}

	inline void fs_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Stat_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_stat(v0, vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_readdir(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_readdir(v0, vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		hr = ac_open(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_tmpFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<File_base> vr;

		METHOD_ENTER(0, 0);

		hr = ac_tmpFile(vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_openTextStream(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<BufferedStream_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		hr = ac_openTextStream(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_readFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_readFile(v0, vr);

		METHOD_RETURN();
	}

	inline void fs_base::s_writeFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_writeFile(v0, v1);

		METHOD_VOID();
	}

}

#endif

