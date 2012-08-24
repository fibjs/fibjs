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
class net_base;
class Buffer_base;

class Socket_base : public Stream_base
{
public:
	// Socket_base
	static result_t _new(int32_t family, int32_t type, obj_ptr<Socket_base>& retVal);
	virtual result_t get_family(int32_t& retVal) = 0;
	virtual result_t get_type(int32_t& retVal) = 0;
	virtual result_t get_remoteAddress(std::string& retVal) = 0;
	virtual result_t get_remotePort(int32_t& retVal) = 0;
	virtual result_t get_localAddress(std::string& retVal) = 0;
	virtual result_t get_localPort(int32_t& retVal) = 0;
	virtual result_t connect(const char* host, int32_t port, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncConnect(const char* host, int32_t port) = 0;
	virtual result_t onconnect(v8::Handle<v8::Function> func) = 0;
	virtual result_t bind(int32_t port, bool allowIPv4) = 0;
	virtual result_t bind(const char* addr, int32_t port, bool allowIPv4) = 0;
	virtual result_t listen(int32_t backlog) = 0;
	virtual result_t accept(obj_ptr<Socket_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncAccept() = 0;
	virtual result_t onaccept(v8::Handle<v8::Function> func) = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncClose() = 0;
	virtual result_t onclose(v8::Handle<v8::Function> func) = 0;
	virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncRecv(int32_t bytes) = 0;
	virtual result_t onrecv(v8::Handle<v8::Function> func) = 0;
	virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t send(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncSend(obj_ptr<Buffer_base>& data) = 0;
	virtual result_t onsend(v8::Handle<v8::Function> func) = 0;
	virtual result_t sendto(obj_ptr<Buffer_base>& data, const char* host, int32_t port) = 0;

	DECLARE_CLASSINFO(Socket_base);

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

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_family(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_remoteAddress(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_remotePort(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_localAddress(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_localPort(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_connect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncConnect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onconnect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_bind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_listen(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_accept(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncAccept(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onaccept(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncClose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onclose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncRecv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onrecv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recvFrom(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncSend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onsend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sendto(const v8::Arguments& args);

public:
	ASYNC_MEMBER2(Socket_base, connect);
	ASYNC_CALLBACK2(Socket_base, connect);
	ASYNC_MEMBER1(Socket_base, accept);
	ASYNC_VALUEBACK0(Socket_base, accept, obj_ptr<Socket_base>);
	ASYNC_MEMBER0(Socket_base, close);
	ASYNC_CALLBACK0(Socket_base, close);
	ASYNC_MEMBER2(Socket_base, recv);
	ASYNC_VALUEBACK1(Socket_base, recv, obj_ptr<Buffer_base>);
	ASYNC_MEMBER1(Socket_base, send);
	ASYNC_CALLBACK1(Socket_base, send);
};

}

#include "net.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Socket_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"connect", s_connect},
			{"asyncConnect", s_asyncConnect},
			{"onconnect", s_onconnect},
			{"bind", s_bind},
			{"listen", s_listen},
			{"accept", s_accept},
			{"asyncAccept", s_asyncAccept},
			{"onaccept", s_onaccept},
			{"close", s_close},
			{"asyncClose", s_asyncClose},
			{"onclose", s_onclose},
			{"recv", s_recv},
			{"asyncRecv", s_asyncRecv},
			{"onrecv", s_onrecv},
			{"recvFrom", s_recvFrom},
			{"send", s_send},
			{"asyncSend", s_asyncSend},
			{"onsend", s_onsend},
			{"sendto", s_sendto}
		};

		static ClassData::ClassProperty s_property[] = 
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
			"Socket", s__new, 
			19, s_method, 0, NULL, 6, s_property, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

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

	inline v8::Handle<v8::Value> Socket_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Socket_base> vr;

		CONSTRUCT_ENTER(2, 0);

		OPT_ARG(int32_t, 0, net_base::_AF_INET);
		OPT_ARG(int32_t, 1, net_base::_SOCK_STREAM);

		hr = _new(v0, v1, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_connect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = pInst->ac_connect(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_asyncConnect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = pInst->asyncConnect(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_onconnect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onconnect(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_bind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG(bool, 1, true);

		hr = pInst->bind(v0, v1);

		METHOD_OVER(3, 2);

		ARG_String(0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, true);

		hr = pInst->bind(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_listen(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 120);

		hr = pInst->listen(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_accept(const v8::Arguments& args)
	{
		obj_ptr<Socket_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_accept(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_asyncAccept(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncAccept();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_onaccept(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onaccept(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_asyncClose(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncClose();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_onclose(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onclose(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_recv(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->ac_recv(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_asyncRecv(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->asyncRecv(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_onrecv(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onrecv(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_recvFrom(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, -1);

		hr = pInst->recvFrom(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_send(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_asyncSend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->asyncSend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_onsend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onsend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_sendto(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(3, 3);

		ARG(obj_ptr<Buffer_base>, 0);
		ARG_String(1);
		ARG(int32_t, 2);

		hr = pInst->sendto(v0, v1, v2);

		METHOD_VOID();
	}

}

#endif

