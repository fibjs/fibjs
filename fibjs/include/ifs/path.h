/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
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
	DECLARE_CLASS(path_base);

public:
	// path_base
	static result_t normalize(const char* path, std::string& retVal);
	static result_t basename(const char* path, const char* ext, std::string& retVal);
	static result_t extname(const char* path, std::string& retVal);
	static result_t dirname(const char* path, std::string& retVal);
	static result_t combine(const v8::FunctionCallbackInfo<v8::Value>& args, std::string& retVal);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_basename(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_extname(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_dirname(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_combine(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& path_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"normalize", s_normalize},
			{"basename", s_basename},
			{"extname", s_extname},
			{"dirname", s_dirname},
			{"combine", s_combine}
		};

		static ClassData s_cd = 
		{ 
			"path", NULL, 
			5, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void path_base::s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = normalize(v0, vr);

		METHOD_RETURN();
	}

	inline void path_base::s_basename(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(arg_string, 1, "");

		hr = basename(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void path_base::s_extname(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = extname(v0, vr);

		METHOD_RETURN();
	}

	inline void path_base::s_dirname(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = dirname(v0, vr);

		METHOD_RETURN();
	}

	inline void path_base::s_combine(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(-1, 0);

		hr = combine(args, vr);

		METHOD_RETURN();
	}

}

#endif

