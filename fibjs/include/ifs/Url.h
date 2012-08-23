/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
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
public:
	// Url_base
	static result_t _new(v8::Handle<v8::Object> args, obj_ptr<Url_base>& retVal);
	static result_t _new(const char* url, obj_ptr<Url_base>& retVal);
	virtual result_t parse(const char* url) = 0;
	virtual result_t format(v8::Handle<v8::Object> args) = 0;
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

	DECLARE_CLASSINFO(Url_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(href);
		CLONE_String(protocol);
		CLONE(slashes, int32_t);
		CLONE_String(auth);
		CLONE_String(username);
		CLONE_String(password);
		CLONE_String(host);
		CLONE_String(hostname);
		CLONE_String(port);
		CLONE_String(path);
		CLONE_String(pathname);
		CLONE_String(search);
		CLONE_String(query);
		CLONE_String(hash);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_parse(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_format(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_resolve(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_normalize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_href(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_slashes(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_auth(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_username(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_password(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_host(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_hostname(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_port(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_path(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_pathname(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_search(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_query(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_hash(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

namespace fibjs
{
	inline ClassInfo& Url_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"parse", s_parse},
			{"format", s_format},
			{"resolve", s_resolve},
			{"normalize", s_normalize}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"href", s_get_href},
			{"protocol", s_get_protocol},
			{"slashes", s_get_slashes},
			{"auth", s_get_auth},
			{"username", s_get_username},
			{"password", s_get_password},
			{"host", s_get_host},
			{"hostname", s_get_hostname},
			{"port", s_get_port},
			{"path", s_get_path},
			{"pathname", s_get_pathname},
			{"search", s_get_search},
			{"query", s_get_query},
			{"hash", s_get_hash}
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

	inline v8::Handle<v8::Value> Url_base::s_get_href(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_href(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_protocol(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_slashes(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_slashes(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_auth(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_auth(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_username(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_username(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_password(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_password(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_host(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_host(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_hostname(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_hostname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_port(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_port(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_path(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_path(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_pathname(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_pathname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_search(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_search(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_query(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_query(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_get_hash(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Url_base);

		hr = pInst->get_hash(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Url_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = _new(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_parse(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->parse(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Url_base::s_format(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->format(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Url_base::s_resolve(const v8::Arguments& args)
	{
		obj_ptr<Url_base> vr;

		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->resolve(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Url_base::s_normalize(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Url_base);
		METHOD_ENTER(0, 0);

		hr = pInst->normalize();

		METHOD_VOID();
	}

}

#endif

