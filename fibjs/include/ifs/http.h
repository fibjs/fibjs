/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _http_base_H_
#define _http_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HttpRequest_base;
class HttpResponse_base;
class HttpCookie_base;
class HttpServer_base;
class HttpsServer_base;
class HttpHandler_base;
class Handler_base;
class Stream_base;
class SeekableStream_base;
class Buffer_base;

class http_base : public object_base
{
	DECLARE_CLASS(http_base);

public:
	// http_base
	static result_t fileHandler(const char* root, v8::Local<v8::Object> mimes, obj_ptr<Handler_base>& retVal);
	static result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, exlib::AsyncEvent* ac);
	static result_t request(const char* method, const char* url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t request(const char* method, const char* url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t request(const char* method, const char* url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t request(const char* method, const char* url, const char* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t get(const char* url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
	static result_t post(const char* url, const char* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);

public:
	static void s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_request(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE3(http_base, request, Stream_base*, HttpRequest_base*, obj_ptr<HttpResponse_base>);
};

}

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpCookie.h"
#include "HttpServer.h"
#include "HttpsServer.h"
#include "HttpHandler.h"
#include "Handler.h"
#include "Stream.h"
#include "SeekableStream.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& http_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"fileHandler", s_fileHandler},
			{"request", s_request},
			{"get", s_get},
			{"post", s_post}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Request", HttpRequest_base::class_info},
			{"Response", HttpResponse_base::class_info},
			{"Cookie", HttpCookie_base::class_info},
			{"Server", HttpServer_base::class_info},
			{"HttpsServer", HttpsServer_base::class_info},
			{"Handler", HttpHandler_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"http", NULL, 
			4, s_method, 6, s_object, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void http_base::s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

		hr = fileHandler(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void http_base::s_request(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpResponse_base> vr;

		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Stream_base>, 0);
		ARG(obj_ptr<HttpRequest_base>, 1);

		hr = ac_request(v0, v1, vr);

		METHOD_OVER(3, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

		hr = request(v0, v1, v2, vr);

		METHOD_OVER(4, 3);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		ARG(obj_ptr<SeekableStream_base>, 2);
		OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

		hr = request(v0, v1, v2, v3, vr);

		METHOD_OVER(4, 3);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		ARG(obj_ptr<Buffer_base>, 2);
		OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

		hr = request(v0, v1, v2, v3, vr);

		METHOD_OVER(4, 3);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		ARG(arg_string, 2);
		OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

		hr = request(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline void http_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpResponse_base> vr;

		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

		hr = get(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void http_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpResponse_base> vr;

		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

		hr = post(v0, v1, vr);

		METHOD_OVER(3, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<SeekableStream_base>, 1);
		OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

		hr = post(v0, v1, v2, vr);

		METHOD_OVER(3, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<Buffer_base>, 1);
		OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

		hr = post(v0, v1, v2, vr);

		METHOD_OVER(3, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

		hr = post(v0, v1, v2, vr);

		METHOD_RETURN();
	}

}

#endif

