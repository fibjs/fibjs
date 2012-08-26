/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpHandler_base_H_
#define _HttpHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;

class HttpHandler_base : public Handler_base
{
public:
	// HttpHandler_base
	static result_t _new(obj_ptr<Handler_base>& hdlr, obj_ptr<HttpHandler_base>& retVal);
	static result_t _new(v8::Handle<v8::Function> hdlr, obj_ptr<HttpHandler_base>& retVal);

	DECLARE_CLASSINFO(HttpHandler_base);

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& HttpHandler_base::class_info()
	{
		static ClassData s_cd = 
		{ 
			"HttpHandler", s__new, 
			0, NULL, 0, NULL, 0, NULL, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> HttpHandler_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<HttpHandler_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(obj_ptr<Handler_base>, 0);

		hr = _new(v0, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

}

#endif

