/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _SandBox_base_H_
#define _SandBox_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class SandBox_base : public object_base
{
public:
	// SandBox_base
	virtual result_t add(const char* id, v8::Handle<v8::Value> mod, bool clone) = 0;
	virtual result_t add(v8::Handle<v8::Object> mods, bool clone) = 0;
	virtual result_t remove(const char* id) = 0;
	virtual result_t run(const char* fname) = 0;
	virtual result_t require(const char* id, v8::Handle<v8::Value>& retVal) = 0;

	DECLARE_CLASSINFO(SandBox_base);

public:
	static v8::Handle<v8::Value> s_add(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_remove(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_run(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_require(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& SandBox_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"add", s_add},
			{"remove", s_remove},
			{"run", s_run},
			{"require", s_require}
		};

		static ClassData s_cd = 
		{ 
			"SandBox", NULL, 
			4, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> SandBox_base::s_add(const v8::Arguments& args)
	{
		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(3, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG(bool, 2, true);

		hr = pInst->add(v0, v1, v2);

		METHOD_OVER(2, 1);

		ARG(v8::Handle<v8::Object>, 0);
		OPT_ARG(bool, 1, true);

		hr = pInst->add(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> SandBox_base::s_remove(const v8::Arguments& args)
	{
		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->remove(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> SandBox_base::s_run(const v8::Arguments& args)
	{
		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->run(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> SandBox_base::s_require(const v8::Arguments& args)
	{
		v8::Handle<v8::Value> vr;

		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->require(v0, vr);

		METHOD_RETURN();
	}

}

#endif

