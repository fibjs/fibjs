/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Socket_base_H_
#define _Socket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs
{

class Stream_base;
class Buffer_base;

class Socket_base : public Stream_base
{
public:
	// Socket_base
	virtual result_t get_family(int32_t& retVal) = 0;
	virtual result_t get_type(int32_t& retVal) = 0;
	virtual result_t get_remoteAddress(std::string& retVal) = 0;
	virtual result_t get_remotePort(int32_t& retVal) = 0;
	virtual result_t get_localAddress(std::string& retVal) = 0;
	virtual result_t get_localPort(int32_t& retVal) = 0;
	virtual result_t connect(const char* addr, int32_t port) = 0;
	virtual result_t bind(int32_t port, bool onlyIPv6) = 0;
	virtual result_t bind(const char* addr, int32_t port, bool onlyIPv6) = 0;
	virtual result_t listen(int32_t backlog) = 0;
	virtual result_t accept(obj_ptr<Socket_base>& retVal) = 0;
	virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t send(obj_ptr<Buffer_base> data) = 0;
	virtual result_t sendto(obj_ptr<Buffer_base> data, const char* host, int32_t port) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"connect", s_connect},
			{"bind", s_bind},
			{"bind", s_bind},
			{"listen", s_listen},
			{"accept", s_accept},
			{"recv", s_recv},
			{"recvFrom", s_recvFrom},
			{"send", s_send},
			{"sendto", s_sendto}
		};

		static ClassProperty s_property[] = 
		{
			{"family", s_get_family},
			{"type", s_get_type},
			{"remoteAddress", s_get_remoteAddress},
			{"remotePort", s_get_remotePort},
			{"localAddress", s_get_localAddress},
			{"localPort", s_get_localPort}
		};

		static ClassData s_cd = 
		{ 
			"Socket", NULL, 
			9, s_method, 6, s_property, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Stream_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(family, int32_t);
		CLONE(type, int32_t);
		CLONE_String(remoteAddress);
		CLONE(remotePort, int32_t);
		CLONE_String(localAddress);
		CLONE(localPort, int32_t);

		return 0;
	}

private:
	static v8::Handle<v8::Value> s_get_family(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_remoteAddress(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_remotePort(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_localAddress(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_localPort(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_connect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_bind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_listen(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_accept(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recvFrom(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sendto(const v8::Arguments& args);

private:
	ASYNC_MEMBER2(Socket_base, connect);
	ASYNC_MEMBER1(Socket_base, accept);
	ASYNC_MEMBER2(Socket_base, recv);
	ASYNC_MEMBER2(Socket_base, recvFrom);
	ASYNC_MEMBER1(Socket_base, send);
	ASYNC_MEMBER3(Socket_base, sendto);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> Socket_base::s_get_family(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Socket_base);

		hr = pInst->get_family(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Socket_base);

		hr = pInst->get_type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_get_remoteAddress(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Socket_base);

		hr = pInst->get_remoteAddress(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_get_remotePort(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Socket_base);

		hr = pInst->get_remotePort(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_get_localAddress(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Socket_base);

		hr = pInst->get_localAddress(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_get_localPort(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Socket_base);

		hr = pInst->get_localPort(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_connect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = pInst->ac_connect(s_acPool, v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_bind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, false);

		hr = pInst->bind(v0, v1);

		METHOD_OVER(3, 2);

		ARG_String(0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->bind(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_listen(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 20);

		hr = pInst->listen(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_accept(const v8::Arguments& args)
	{
		obj_ptr<Socket_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_accept(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_recv(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_recv(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_recvFrom(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_recvFrom(s_acPool, v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_send(s_acPool, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_sendto(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(3, 3);

		ARG(obj_ptr<Buffer_base>, 0);
		ARG_String(1);
		ARG(int32_t, 2);

		hr = pInst->ac_sendto(s_acPool, v0, v1, v2);

		METHOD_VOID();
	}

}

#endif

