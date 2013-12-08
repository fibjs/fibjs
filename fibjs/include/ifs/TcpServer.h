/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _TcpServer_base_H_
#define _TcpServer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Handler_base;
class Socket_base;
class Stats_base;

class TcpServer_base : public object_base
{
public:
	// TcpServer_base
	static result_t _new(int32_t port, Handler_base* listener, obj_ptr<TcpServer_base>& retVal);
	static result_t _new(int32_t port, v8::Handle<v8::Function> listener, obj_ptr<TcpServer_base>& retVal);
	static result_t _new(const char* addr, int32_t port, Handler_base* listener, obj_ptr<TcpServer_base>& retVal);
	static result_t _new(const char* addr, int32_t port, v8::Handle<v8::Function> listener, obj_ptr<TcpServer_base>& retVal);
	virtual result_t run(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncRun() = 0;
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal) = 0;
	virtual result_t get_stats(obj_ptr<Stats_base>& retVal) = 0;

	DECLARE_CLASSINFO(TcpServer_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncRun(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_socket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
	ASYNC_MEMBER0(TcpServer_base, run);
};

}

#include "Handler.h"
#include "Socket.h"
#include "Stats.h"

namespace fibjs
{
	inline ClassInfo& TcpServer_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"run", s_run},
			{"asyncRun", s_asyncRun}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"socket", s_get_socket, block_set},
			{"stats", s_get_stats, block_set}
		};

		static ClassData s_cd = 
		{ 
			"TcpServer", s__new, 
			2, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void TcpServer_base::s_get_socket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Socket_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(TcpServer_base);

		hr = pInst->get_socket(vr);

		METHOD_RETURN();
	}

	inline void TcpServer_base::s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Stats_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(TcpServer_base);

		hr = pInst->get_stats(vr);

		METHOD_RETURN();
	}

	inline void TcpServer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<TcpServer_base> vr;

		CONSTRUCT_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(obj_ptr<Handler_base>, 1);

		hr = _new(v0, v1, vr);

		METHOD_OVER(2, 2);

		ARG(int32_t, 0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = _new(v0, v1, vr);

		METHOD_OVER(3, 3);

		ARG_String(0);
		ARG(int32_t, 1);
		ARG(obj_ptr<Handler_base>, 2);

		hr = _new(v0, v1, v2, vr);

		METHOD_OVER(3, 3);

		ARG_String(0);
		ARG(int32_t, 1);
		ARG(v8::Handle<v8::Function>, 2);

		hr = _new(v0, v1, v2, vr);

		CONSTRUCT_RETURN();
	}

	inline void TcpServer_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(TcpServer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_run();

		METHOD_VOID();
	}

	inline void TcpServer_base::s_asyncRun(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(TcpServer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncRun();

		METHOD_VOID();
	}

}

#endif

