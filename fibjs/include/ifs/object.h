/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _object_base_H_
#define _object_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class object_base : public object_base
{
public:
	// object_base
	virtual result_t dispose() = 0;
	virtual result_t toString(std::string& retVal) = 0;
	virtual result_t toJSON(const char* key, v8::Local<v8::Value>& retVal) = 0;
	virtual result_t ValueOf(v8::Local<v8::Value>& retVal) = 0;

	DECLARE_CLASSINFO(object_base);

public:
	static void s_dispose(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ValueOf(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
	inline ClassInfo& object_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"dispose", s_dispose},
			{"toString", s_toString},
			{"toJSON", s_toJSON},
			{"ValueOf", s_ValueOf}
		};

		static ClassData s_cd = 
		{ 
			"object", NULL, 
			4, s_method, 0, NULL, 0, NULL, NULL, NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void object_base::s_dispose(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(object_base);
		METHOD_ENTER(0, 0);

		hr = pInst->dispose();

		METHOD_VOID();
	}

	inline void object_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(object_base);
		METHOD_ENTER(0, 0);

		hr = pInst->toString(vr);

		METHOD_RETURN();
	}

	inline void object_base::s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(object_base);
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "");

		hr = pInst->toJSON(v0, vr);

		METHOD_RETURN();
	}

	inline void object_base::s_ValueOf(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(object_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ValueOf(vr);

		METHOD_RETURN();
	}

}

#endif

