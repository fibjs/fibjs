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
class HttpServer_base;
class Handler_base;
class HttpHandler_base;
class SeekableStream_base;
class Buffer_base;

class http_base : public module_base
{
public:
	// http_base
	static result_t handler(Handler_base* hdlr, obj_ptr<HttpHandler_base>& retVal);
	static result_t handler(v8::Handle<v8::Function> hdlr, obj_ptr<HttpHandler_base>& retVal);
	static result_t fileHandler(const char* root, obj_ptr<Handler_base>& retVal);
	static result_t request(const char* host, int32_t port, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, exlib::AsyncEvent* ac);
	static result_t request(const char* method, const char* url, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t request(const char* method, const char* url, SeekableStream_base* body, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t request(const char* method, const char* url, Buffer_base* body, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t request(const char* method, const char* url, const char* body, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t get(const char* url, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, SeekableStream_base* body, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, Buffer_base* body, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, const char* body, v8::Handle<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);

	DECLARE_CLASSINFO(http_base);

public:
	static void s_handler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_request(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE4(http_base, request, const char*, int32_t, HttpRequest_base*, obj_ptr<HttpResponse_base>);
};

}

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"
#include "Handler.h"
#include "HttpHandler.h"
#include "SeekableStream.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& http_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"handler", s_handler, true},
			{"fileHandler", s_fileHandler, true},
			{"request", s_request, true},
			{"get", s_get, true},
			{"post", s_post, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Request", HttpRequest_base::class_info},
			{"Response", HttpResponse_base::class_info},
			{"Server", HttpServer_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"http", NULL, 
			5, s_method, 3, s_object, 0, NULL, NULL, NULL,
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

	inline void http_base::s_request(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpResponse_base> vr;

		METHOD_ENTER(3, 3);

		ARG_String(0);
		ARG(int32_t, 1);
		ARG(obj_ptr<HttpRequest_base>, 2);

		hr = ac_request(v0, v1, v2, vr);

		METHOD_OVER(3, 2);

		ARG_String(0);
		ARG_String(1);
		OPT_ARG(v8::Handle<v8::Object>, 2, v8::Object::New());

		hr = request(v0, v1, v2, vr);

		METHOD_OVER(4, 3);

		ARG_String(0);
		ARG_String(1);
		ARG(obj_ptr<SeekableStream_base>, 2);
		OPT_ARG(v8::Handle<v8::Object>, 3, v8::Object::New());

		hr = request(v0, v1, v2, v3, vr);

		METHOD_OVER(4, 3);

		ARG_String(0);
		ARG_String(1);
		ARG(obj_ptr<Buffer_base>, 2);
		OPT_ARG(v8::Handle<v8::Object>, 3, v8::Object::New());

		hr = request(v0, v1, v2, v3, vr);

		METHOD_OVER(4, 3);

		ARG_String(0);
		ARG_String(1);
		ARG_String(2);
		OPT_ARG(v8::Handle<v8::Object>, 3, v8::Object::New());

		hr = request(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void http_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpResponse_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(v8::Handle<v8::Object>, 1, v8::Object::New());

		hr = get(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void http_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpResponse_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(v8::Handle<v8::Object>, 1, v8::Object::New());

		hr = post(v0, v1, vr);

		METHOD_OVER(3, 2);

		ARG_String(0);
		ARG(obj_ptr<SeekableStream_base>, 1);
		OPT_ARG(v8::Handle<v8::Object>, 2, v8::Object::New());

		hr = post(v0, v1, v2, vr);

		METHOD_OVER(3, 2);

		ARG_String(0);
		ARG(obj_ptr<Buffer_base>, 1);
		OPT_ARG(v8::Handle<v8::Object>, 2, v8::Object::New());

		hr = post(v0, v1, v2, vr);

		METHOD_OVER(3, 2);

		ARG_String(0);
		ARG_String(1);
		OPT_ARG(v8::Handle<v8::Object>, 2, v8::Object::New());

		hr = post(v0, v1, v2, vr);

		METHOD_RETURN();
	}

}

#endif

