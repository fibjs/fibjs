/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fs_H_
#define _fs_H_

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
	static result_t open(const char* fname, const char* IOMode, const char* encoding, file_base*& retVal);
	static result_t create(const char* fname, bool Overwrite, file_base*& retVal);
	static result_t tmpFile(file_base*& retVal);

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"open", s_open},
			{"create", s_create},
			{"tmpFile", s_tmpFile}
		};

		static ClassProperty s_property[] = 
		{
			{"FSEEK_SET", s_get_FSEEK_SET},
			{"FSEEK_CUR", s_get_FSEEK_CUR},
			{"FSEEK_END", s_get_FSEEK_END}
		};

		static ClassInfo s_ci("fs", 3, s_method, 3, s_property, &object_base::info());

		return s_ci;
	}

    virtual v8::Handle<v8::Value> ToJSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> s_get_FSEEK_SET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_SET;

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_get_FSEEK_CUR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_CUR;

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_get_FSEEK_END(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		int32_t vr = FSEEK_END;

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_open(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 1);

		ARG_string(0);
		OPT_ARG_string(1, "r");
		OPT_ARG_string(2, "utf-8");

		file_base* vr;
		hr = open(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_create(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG_string(0);
		OPT_ARG_boolean(1, true);

		file_base* vr;
		hr = create(v0, v1, vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_tmpFile(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		file_base* vr;
		hr = tmpFile(vr);

		METHOD_RETURN();
	}

};

}

#include "file.h"

#endif

