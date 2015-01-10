/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Url_base_H_
#define _Url_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Url_base : public object_base
{
	DECLARE_CLASS(Url_base);

public:
	// Url_base
	static result_t _new(v8::Local<v8::Object> args, obj_ptr<Url_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(const char* url, obj_ptr<Url_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t parse(const char* url) = 0;
	virtual result_t format(v8::Local<v8::Object> args) = 0;
	virtual result_t resolve(const char* url, obj_ptr<Url_base>& retVal) = 0;
	virtual result_t normalize() = 0;
	virtual result_t get_href(std::string& retVal) = 0;
	virtual result_t get_protocol(std::string& retVal) = 0;
	virtual result_t get_slashes(int32_t& retVal) = 0;
	virtual result_t get_auth(std::string& retVal) = 0;
	virtual result_t get_username(std::string& retVal) = 0;
	virtual result_t get_password(std::string& retVal) = 0;
	virtual result_t get_host(std::string& retVal) = 0;
	virtual result_t get_hostname(std::string& retVal) = 0;
	virtual result_t get_port(std::string& retVal) = 0;
	virtual result_t get_path(std::string& retVal) = 0;
	virtual result_t get_pathname(std::string& retVal) = 0;
	virtual result_t get_search(std::string& retVal) = 0;
	virtual result_t get_query(std::string& retVal) = 0;
	virtual result_t get_hash(std::string& retVal) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_href(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_protocol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_slashes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_auth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_username(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_password(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_host(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_port(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_path(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_pathname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_search(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_query(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_hash(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

namespace fibjs
{
	inline ClassInfo& Url_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"parse", s_parse, false},
			{"format", s_format, false},
			{"resolve", s_resolve, false},
			{"normalize", s_normalize, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"href", s_get_href, block_set},
			{"protocol", s_get_protocol, block_set},
			{"slashes", s_get_slashes, block_set},
			{"auth", s_get_auth, block_set},
			{"username", s_get_username, block_set},
			{"password", s_get_password, block_set},
			{"host", s_get_host, block_set},
			{"hostname", s_get_hostname, block_set},
			{"port", s_get_port, block_set},
			{"path", s_get_path, block_set},
			{"pathname", s_get_pathname, block_set},
			{"search", s_get_search, block_set},
			{"query", s_get_query, block_set},
			{"hash", s_get_hash, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Url", s__new, 
			4, s_method, 0, NULL, 14, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Url_base::s_get_href(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_href(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_protocol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_protocol(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_slashes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_slashes(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_auth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_auth(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_username(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_username(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_password(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_password(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_host(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_host(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_hostname(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_port(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_port(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_path(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_path(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_pathname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_pathname(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_search(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_search(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_query(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_query(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_get_hash(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_hash(vr);

		METHOD_RETURN();
	}

	inline void Url_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CONSTRUCT_INIT();
		__new(args);
	}

	template<typename T>void Url_base::__new(const T& args)
	{
		obj_ptr<Url_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = _new(v0, vr, args.This());

		METHOD_OVER(1, 0);

		OPT_ARG(arg_string, 0, "");

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void Url_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->parse(v0);

		METHOD_VOID();
	}

	inline void Url_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->format(v0);

		METHOD_VOID();
	}

	inline void Url_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Url_base> vr;

		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->resolve(v0, vr);

		METHOD_RETURN();
	}

	inline void Url_base::s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(0, 0);

		hr = pInst->normalize();

		METHOD_VOID();
	}

}

#endif

