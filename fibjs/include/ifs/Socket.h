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
	virtual result_t connect(const char* host, int32_t port) = 0;
	virtual result_t bind(const char* addr, int32_t port) = 0;
	virtual result_t bind(int32_t port) = 0;
	virtual result_t listen() = 0;
	virtual result_t accept(obj_ptr<Socket_base>& retVal) = 0;
	virtual result_t recv(int32_t size, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t recvFrom(int32_t size, obj_ptr<Buffer_base>& retVal) = 0;
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
			{"type", s_get_type}
		};

		static ClassData s_cd = 
		{ 
			"Socket", NULL, 
			9, s_method, 2, s_property, NULL,
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

		return 0;
	}

private:
	static v8::Handle<v8::Value> s_get_family(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_connect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_bind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_listen(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_accept(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recvFrom(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sendto(const v8::Arguments& args);
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

	inline v8::Handle<v8::Value> Socket_base::s_connect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = pInst->connect(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_bind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = pInst->bind(v0, v1);

		METHOD_OVER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->bind(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_listen(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->listen();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Socket_base::s_accept(const v8::Arguments& args)
	{
		obj_ptr<Socket_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->accept(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_recv(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->recv(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_recvFrom(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->recvFrom(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Socket_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Socket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->send(v0);

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

