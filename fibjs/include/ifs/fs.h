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

#include "if.h"

namespace fibjs
{

class file_base;

class fs_base : public object_base
{
public:
	static const int32_t FSEEK_SET = 0;
	static const int32_t FSEEK_CUR = 1;
	static const int32_t FSEEK_END = 2;

public:
	// fs_base
	static result_t open(const char* fname, const char* mode, obj_ptr<file_base>& retVal);
	static result_t create(const char* fname, bool Overwrite, obj_ptr<file_base>& retVal);
	static result_t tmpFile(obj_ptr<file_base>& retVal);
	static result_t readFile(const char* fname, std::string& retVal);
	static result_t writeFile(const char* fname, const char* txt);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"open", s_open},
			{"create", s_create},
			{"tmpFile", s_tmpFile},
			{"readFile", s_readFile},
			{"writeFile", s_writeFile}
		};

		static ClassProperty s_property[] = 
		{
			{"FSEEK_SET", s_get_FSEEK_SET},
			{"FSEEK_CUR", s_get_FSEEK_CUR},
			{"FSEEK_END", s_get_FSEEK_END}
		};

		static ClassData s_cd = 
		{ 
			"fs", NULL, 
			5, s_method, 3, s_property, NULL,
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
	static v8::Handle<v8::Value> s_get_FSEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_FSEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_FSEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_create(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tmpFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readFile(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_writeFile(const v8::Arguments& args);
};

}

#include "file.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> fs_base::s_get_FSEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_SET;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_get_FSEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_CUR;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_get_FSEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_END;

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_open(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		obj_ptr<file_base> vr;
		hr = open(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_create(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(bool, 1, true);

		obj_ptr<file_base> vr;
		hr = create(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_tmpFile(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		obj_ptr<file_base> vr;
		hr = tmpFile(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_readFile(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		std::string vr;
		hr = readFile(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> fs_base::s_writeFile(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = writeFile(v0, v1);

		METHOD_VOID();
	}

}

#endif

