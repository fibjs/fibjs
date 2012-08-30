/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _JSHandler_base_H_
#define _JSHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;
class object_base;

class JSHandler_base : public Handler_base
{
public:
	// JSHandler_base
	virtual result_t js_invoke(object_base* v, v8::Handle<v8::Value>& retVal) = 0;

	DECLARE_CLASSINFO(JSHandler_base);

public:
	static v8::Handle<v8::Value> s_js_invoke(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& JSHandler_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"js_invoke", s_js_invoke}
		};

		static ClassData s_cd = 
		{ 
			"JSHandler", NULL, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> JSHandler_base::s_js_invoke(const v8::Arguments& args)
	{
		v8::Handle<v8::Value> vr;

		METHOD_INSTANCE(JSHandler_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<object_base>, 0);

		hr = pInst->js_invoke(v0, vr);

		METHOD_RETURN();
	}

}

#endif

