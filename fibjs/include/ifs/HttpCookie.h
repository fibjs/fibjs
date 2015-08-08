/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpCookie_base_H_
#define _HttpCookie_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HttpCookie_base : public object_base
{
	DECLARE_CLASS(HttpCookie_base);

public:
	// HttpCookie_base
	static result_t _new(v8::Local<v8::Object> opts, obj_ptr<HttpCookie_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(const char* name, const char* value, v8::Local<v8::Object> opts, obj_ptr<HttpCookie_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t parse(const char* header) = 0;
	virtual result_t match(const char* url, bool& retVal) = 0;
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t set_name(const char* newVal) = 0;
	virtual result_t get_value(std::string& retVal) = 0;
	virtual result_t set_value(const char* newVal) = 0;
	virtual result_t get_domain(std::string& retVal) = 0;
	virtual result_t set_domain(const char* newVal) = 0;
	virtual result_t get_path(std::string& retVal) = 0;
	virtual result_t set_path(const char* newVal) = 0;
	virtual result_t get_expires(date_t& retVal) = 0;
	virtual result_t set_expires(date_t newVal) = 0;
	virtual result_t get_httpOnly(bool& retVal) = 0;
	virtual result_t set_httpOnly(bool newVal) = 0;
	virtual result_t get_secure(bool& retVal) = 0;
	virtual result_t set_secure(bool newVal) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_match(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_domain(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_domain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_path(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_path(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_expires(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_expires(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_httpOnly(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_httpOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_secure(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_secure(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

namespace fibjs
{
inline ClassInfo& HttpCookie_base::class_info()
{
	static ClassData::ClassMethod s_method[] =
	{
		{"parse", s_parse, false},
		{"match", s_match, false}
	};

	static ClassData::ClassProperty s_property[] =
	{
		{"name", s_get_name, s_set_name, false},
		{"value", s_get_value, s_set_value, false},
		{"domain", s_get_domain, s_set_domain, false},
		{"path", s_get_path, s_set_path, false},
		{"expires", s_get_expires, s_set_expires, false},
		{"httpOnly", s_get_httpOnly, s_set_httpOnly, false},
		{"secure", s_get_secure, s_set_secure, false}
	};

	static ClassData s_cd =
	{
		"HttpCookie", s__new,
		2, s_method, 0, NULL, 7, s_property, NULL, NULL,
		&object_base::class_info()
	};

	static ClassInfo s_ci(s_cd);
	return s_ci;
}

inline void HttpCookie_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	std::string vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_name(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(arg_string);
	hr = pInst->set_name(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	std::string vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_value(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(arg_string);
	hr = pInst->set_value(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_domain(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	std::string vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_domain(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_domain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(arg_string);
	hr = pInst->set_domain(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_path(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	std::string vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_path(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_path(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(arg_string);
	hr = pInst->set_path(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_expires(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	date_t vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_expires(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_expires(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(date_t);
	hr = pInst->set_expires(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_httpOnly(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	bool vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_httpOnly(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_httpOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(bool);
	hr = pInst->set_httpOnly(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_secure(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
{
	bool vr;

	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	hr = pInst->get_secure(vr);

	METHOD_RETURN();
}

inline void HttpCookie_base::s_set_secure(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
{
	PROPERTY_ENTER();
	PROPERTY_INSTANCE(HttpCookie_base);

	PROPERTY_VAL(bool);
	hr = pInst->set_secure(v0);

	PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	CONSTRUCT_INIT();
	__new(args);
}

template<typename T>void HttpCookie_base::__new(const T& args)
{
	obj_ptr<HttpCookie_base> vr;

	CONSTRUCT_ENTER(1, 0);

	OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(Isolate::now()->m_isolate));

	hr = _new(v0, vr, args.This());

	METHOD_OVER(3, 2);

	ARG(arg_string, 0);
	ARG(arg_string, 1);
	OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(Isolate::now()->m_isolate));

	hr = _new(v0, v1, v2, vr, args.This());

	CONSTRUCT_RETURN();
}

inline void HttpCookie_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	METHOD_INSTANCE(HttpCookie_base);
	METHOD_ENTER(1, 1);

	ARG(arg_string, 0);

	hr = pInst->parse(v0);

	METHOD_VOID();
}

inline void HttpCookie_base::s_match(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	bool vr;

	METHOD_INSTANCE(HttpCookie_base);
	METHOD_ENTER(1, 1);

	ARG(arg_string, 0);

	hr = pInst->match(v0, vr);

	METHOD_RETURN();
}

}

#endif

