/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _http_base_H_
#define _http_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class HttpRequest_base;
class HttpResponse_base;
class Handler_base;
class HttpHandler_base;

class http_base : public module_base
{
public:
	// http_base
	static result_t handler(Handler_base* hdlr, obj_ptr<HttpHandler_base>& retVal);
	static result_t handler(v8::Handle<v8::Function> hdlr, obj_ptr<HttpHandler_base>& retVal);
	static result_t fileHandler(const char* root, obj_ptr<Handler_base>& retVal);

	DECLARE_CLASSINFO(http_base);

public:
	static void s_handler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Handler.h"
#include "HttpHandler.h"

namespace fibjs
{
	inline ClassInfo& http_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"handler", s_handler, true},
			{"fileHandler", s_fileHandler, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Request", HttpRequest_base::class_info},
			{"Response", HttpResponse_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"http", NULL, 
			2, s_method, 2, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void http_base::s_handler(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpHandler_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Handler_base>, 0);

		hr = handler(v0, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = handler(v0, vr);

		METHOD_RETURN();
	}

	inline void http_base::s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = fileHandler(v0, vr);

		METHOD_RETURN();
	}

}

#endif

