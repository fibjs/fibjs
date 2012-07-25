/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _net_base_H_
#define _net_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Socket_base;
class Url_base;

class net_base : public module_base
{
public:
	enum{
		_AF_INET = 2,
		_AF_INET6 = 10,
		_SOCK_STREAM = 1,
		_SOCK_DGRAM = 2
	};

public:
	// net_base
	static result_t resolve(const char* name, int32_t family, std::string& retVal, exlib::AsyncEvent* ac);
	static result_t ip(const char* name, std::string& retVal, exlib::AsyncEvent* ac);
	static result_t ipv6(const char* name, std::string& retVal, exlib::AsyncEvent* ac);
	static result_t backend(std::string& retVal);

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s_get_AF_INET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_AF_INET6(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_resolve(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ip(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ipv6(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_backend(const v8::Arguments& args);

public:
	ASYNC_STATIC3(net_base, resolve);
	ASYNC_STATIC2(net_base, ip);
	ASYNC_STATIC2(net_base, ipv6);
};

}

#include "Socket.h"
#include "Url.h"

namespace fibjs
{
	inline ClassInfo& net_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"resolve", s_resolve, true},
			{"ip", s_ip, true},
			{"ipv6", s_ipv6, true},
			{"backend", s_backend, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Socket", Socket_base::class_info},
			{"Url", Url_base::class_info}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"AF_INET", s_get_AF_INET, NULL, true},
			{"AF_INET6", s_get_AF_INET6, NULL, true},
			{"SOCK_STREAM", s_get_SOCK_STREAM, NULL, true},
			{"SOCK_DGRAM", s_get_SOCK_DGRAM, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"net", NULL, 
			4, s_method, 2, s_object, 4, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> net_base::s_get_AF_INET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _AF_INET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_get_AF_INET6(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _AF_INET6;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SOCK_STREAM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SOCK_DGRAM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_resolve(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG(int32_t, 1, _AF_INET);

		hr = ac_resolve(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_ip(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_ip(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_ipv6(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_ipv6(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> net_base::s_backend(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = backend(vr);

		METHOD_RETURN();
	}

}

#endif

