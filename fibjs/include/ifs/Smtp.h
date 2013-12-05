/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Smtp_base_H_
#define _Smtp_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class net_base;
class Socket_base;

class Smtp_base : public object_base
{
public:
	// Smtp_base
	static result_t _new(obj_ptr<Smtp_base>& retVal);
	virtual result_t connect(const char* host, int32_t port, int32_t family, exlib::AsyncEvent* ac) = 0;
	virtual result_t command(const char* cmd, const char* arg, std::string& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t hello(const char* hostname, exlib::AsyncEvent* ac) = 0;
	virtual result_t login(const char* username, const char* password, exlib::AsyncEvent* ac) = 0;
	virtual result_t from(const char* adress, exlib::AsyncEvent* ac) = 0;
	virtual result_t to(const char* adress, exlib::AsyncEvent* ac) = 0;
	virtual result_t data(const char* txt, exlib::AsyncEvent* ac) = 0;
	virtual result_t quit(exlib::AsyncEvent* ac) = 0;
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal) = 0;
	virtual result_t set_socket(Socket_base* newVal) = 0;

	DECLARE_CLASSINFO(Smtp_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_command(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hello(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_login(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_from(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_to(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_data(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_quit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_socket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_socket(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);

public:
	ASYNC_MEMBER3(Smtp_base, connect, const char*, int32_t, int32_t);
	ASYNC_MEMBERVALUE3(Smtp_base, command, const char*, const char*, std::string);
	ASYNC_MEMBER1(Smtp_base, hello, const char*);
	ASYNC_MEMBER2(Smtp_base, login, const char*, const char*);
	ASYNC_MEMBER1(Smtp_base, from, const char*);
	ASYNC_MEMBER1(Smtp_base, to, const char*);
	ASYNC_MEMBER1(Smtp_base, data, const char*);
	ASYNC_MEMBER0(Smtp_base, quit);
};

}

#include "net.h"
#include "Socket.h"

namespace fibjs
{
	inline ClassInfo& Smtp_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"connect", s_connect},
			{"command", s_command},
			{"hello", s_hello},
			{"login", s_login},
			{"from", s_from},
			{"to", s_to},
			{"data", s_data},
			{"quit", s_quit}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"socket", s_get_socket, s_set_socket}
		};

		static ClassData s_cd = 
		{ 
			"Smtp", s__new, 
			8, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Smtp_base::s_get_socket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Socket_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Smtp_base);

		hr = pInst->get_socket(vr);

		METHOD_RETURN();
	}

	inline void Smtp_base::s_set_socket(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Smtp_base);

		PROPERTY_VAL(obj_ptr<Socket_base>);
		hr = pInst->set_socket(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void Smtp_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Smtp_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline void Smtp_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(3, 2);

		ARG_String(0);
		ARG(int32_t, 1);
		OPT_ARG(int32_t, 2, net_base::_AF_INET);

		hr = pInst->ac_connect(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Smtp_base::s_command(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = pInst->ac_command(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Smtp_base::s_hello(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "localhost");

		hr = pInst->ac_hello(v0);

		METHOD_VOID();
	}

	inline void Smtp_base::s_login(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = pInst->ac_login(v0, v1);

		METHOD_VOID();
	}

	inline void Smtp_base::s_from(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->ac_from(v0);

		METHOD_VOID();
	}

	inline void Smtp_base::s_to(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->ac_to(v0);

		METHOD_VOID();
	}

	inline void Smtp_base::s_data(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->ac_data(v0);

		METHOD_VOID();
	}

	inline void Smtp_base::s_quit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Smtp_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_quit();

		METHOD_VOID();
	}

}

#endif

