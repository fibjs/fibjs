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
	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal) = 0;
	virtual result_t ValueOf(v8::Handle<v8::Object>& retVal) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"dispose", s_dispose},
			{"toString", s_toString},
			{"toJSON", s_toJSON},
			{"ValueOf", s_ValueOf}
		};

		static ClassData s_cd = 
		{ 
			"object", NULL, 
			4, s_method, 0, NULL, NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_dispose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toString(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toJSON(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ValueOf(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> object_base::s_dispose(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		hr = pInst->dispose();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> object_base::s_toString(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		std::string vr;
		hr = pInst->toString(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> object_base::s_toJSON(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);
		METHOD_INSTANCE(object_base);

		OPT_ARG_String(0, "");

		v8::Handle<v8::Object> vr;
		hr = pInst->toJSON(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> object_base::s_ValueOf(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		v8::Handle<v8::Object> vr;
		hr = pInst->ValueOf(vr);

		METHOD_RETURN();
	}

}

#endif

