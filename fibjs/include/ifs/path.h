/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _path_base_H_
#define _path_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;

class path_base : public module_base
{
public:
	// path_base
	static result_t normalize(const char* path, std::string& retVal);
	static result_t basename(const char* path, const char* ext, std::string& retVal);
	static result_t extname(const char* path, std::string& retVal);
	static result_t dirname(const char* path, std::string& retVal);
	static result_t combine(const v8::Arguments& args, std::string& retVal);

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s_normalize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_basename(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_extname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_dirname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_combine(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& path_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"normalize", s_normalize, true},
			{"basename", s_basename, true},
			{"extname", s_extname, true},
			{"dirname", s_dirname, true},
			{"combine", s_combine, true}
		};

		static ClassData s_cd = 
		{ 
			"path", NULL, 
			5, s_method, 0, NULL, 0, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> path_base::s_normalize(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = normalize(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_basename(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "");

		hr = basename(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_extname(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = extname(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_dirname(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = dirname(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> path_base::s_combine(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(-1, 0);

		hr = combine(args, vr);

		METHOD_RETURN();
	}

}

#endif

