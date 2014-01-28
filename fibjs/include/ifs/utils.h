/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _utils_base_H_
#define _utils_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Stats_base;
class LruCache_base;

class utils_base : public module_base
{
public:
	// utils_base
	static result_t buildInfo(v8::Local<v8::Object>& retVal);

	DECLARE_CLASSINFO(utils_base);

public:
	static void s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Stats.h"
#include "LruCache.h"

namespace fibjs
{
	inline ClassInfo& utils_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"buildInfo", s_buildInfo, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Stats", Stats_base::class_info},
			{"LruCache", LruCache_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"utils", NULL, 
			1, s_method, 2, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void utils_base::s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = buildInfo(vr);

		METHOD_RETURN();
	}

}

#endif

