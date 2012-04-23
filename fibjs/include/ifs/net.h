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

class net_base : public module_base
{
public:
	static const int32_t _AF_INET = 2;
	static const int32_t _AF_INET6 = 10;
	static const int32_t _SOCK_STREAM = 1;
	static const int32_t _SOCK_DGRAM = 2;

public:
	// net_base
	static result_t socket(int32_t family, int32_t type, obj_ptr<Socket_base>& retVal);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"socket", s_socket}
		};

		static ClassProperty s_property[] = 
		{
			{"AF_INET", s_get_AF_INET},
			{"AF_INET6", s_get_AF_INET6},
			{"SOCK_STREAM", s_get_SOCK_STREAM},
			{"SOCK_DGRAM", s_get_SOCK_DGRAM}
		};

		static ClassData s_cd = 
		{ 
			"net", NULL, 
			1, s_method, 4, s_property, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_get_AF_INET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_AF_INET6(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_socket(const v8::Arguments& args);

private:
	ASYNC_STATIC3(net_base, socket);
};

}

#include "Socket.h"

namespace fibjs
{
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

	inline v8::Handle<v8::Value> net_base::s_socket(const v8::Arguments& args)
	{
		obj_ptr<Socket_base> vr;

		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, _AF_INET);
		OPT_ARG(int32_t, 1, _SOCK_STREAM);

		hr = ac_socket(s_acPool, v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

