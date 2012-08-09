/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _TCPServer_base_H_
#define _TCPServer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Socket_base;

class TCPServer_base : public object_base
{
public:
	// TCPServer_base
	static result_t _new(int32_t port, v8::Handle<v8::Function> listener, obj_ptr<TCPServer_base>& retVal);
	static result_t _new(int32_t port, const char* addr, v8::Handle<v8::Function> listener, obj_ptr<TCPServer_base>& retVal);
	virtual result_t run(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncRun() = 0;
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_CLASS(socket, Socket_base);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_run(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncRun(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_socket(v8::Local<v8::String> property, const v8::AccessorInfo &info);

public:
	ASYNC_MEMBER0(TCPServer_base, run);
	ASYNC_CALLBACK0(TCPServer_base, run);
};

}

#include "Socket.h"

namespace fibjs
{
	inline ClassInfo& TCPServer_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"run", s_run},
			{"asyncRun", s_asyncRun}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"socket", s_get_socket}
		};

		static ClassData s_cd = 
		{ 
			"TCPServer", s__new, 
			2, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> TCPServer_base::s_get_socket(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Socket_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(TCPServer_base);

		hr = pInst->get_socket(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> TCPServer_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<TCPServer_base> vr;

		CONSTRUCT_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = _new(v0, v1, vr);

		METHOD_OVER(3, 3);

		ARG(int32_t, 0);
		ARG_String(1);
		ARG(v8::Handle<v8::Function>, 2);

		hr = _new(v0, v1, v2, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> TCPServer_base::s_run(const v8::Arguments& args)
	{
		METHOD_INSTANCE(TCPServer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_run();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> TCPServer_base::s_asyncRun(const v8::Arguments& args)
	{
		METHOD_INSTANCE(TCPServer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncRun();

		METHOD_VOID();
	}

}

#endif

