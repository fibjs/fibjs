/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _SslServer_base_H_
#define _SslServer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class X509Cert_base;
class PKey_base;
class Socket_base;
class Stats_base;

class SslServer_base : public object_base
{
public:
	// SslServer_base
	static result_t _new(v8::Local<v8::Array> certs, int32_t port, v8::Local<v8::Value> listener, obj_ptr<SslServer_base>& retVal);
	static result_t _new(v8::Local<v8::Array> certs, const char* addr, int32_t port, v8::Local<v8::Value> listener, obj_ptr<SslServer_base>& retVal);
	static result_t _new(X509Cert_base* crt, PKey_base* key, int32_t port, v8::Local<v8::Value> listener, obj_ptr<SslServer_base>& retVal);
	static result_t _new(X509Cert_base* crt, PKey_base* key, const char* addr, int32_t port, v8::Local<v8::Value> listener, obj_ptr<SslServer_base>& retVal);
	virtual result_t run(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncRun() = 0;
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal) = 0;
	virtual result_t get_verification(int32_t& retVal) = 0;
	virtual result_t set_verification(int32_t newVal) = 0;
	virtual result_t get_ca(obj_ptr<X509Cert_base>& retVal) = 0;
	virtual result_t get_stats(obj_ptr<Stats_base>& retVal) = 0;

	DECLARE_CLASSINFO(SslServer_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncRun(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_socket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
	ASYNC_MEMBER0(SslServer_base, run);
};

}

#include "X509Cert.h"
#include "PKey.h"
#include "Socket.h"
#include "Stats.h"

namespace fibjs
{
	inline ClassInfo& SslServer_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"run", s_run},
			{"asyncRun", s_asyncRun}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"socket", s_get_socket, block_set},
			{"verification", s_get_verification, s_set_verification},
			{"ca", s_get_ca, block_set},
			{"stats", s_get_stats, block_set}
		};

		static ClassData s_cd = 
		{ 
			"SslServer", s__new, 
			2, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void SslServer_base::s_get_socket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Socket_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslServer_base);

		hr = pInst->get_socket(vr);

		METHOD_RETURN();
	}

	inline void SslServer_base::s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslServer_base);

		hr = pInst->get_verification(vr);

		METHOD_RETURN();
	}

	inline void SslServer_base::s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslServer_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_verification(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void SslServer_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<X509Cert_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslServer_base);

		hr = pInst->get_ca(vr);

		METHOD_RETURN();
	}

	inline void SslServer_base::s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Stats_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslServer_base);

		hr = pInst->get_stats(vr);

		METHOD_RETURN();
	}

	inline void SslServer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SslServer_base> vr;

		CONSTRUCT_ENTER(3, 3);

		ARG(v8::Local<v8::Array>, 0);
		ARG(int32_t, 1);
		ARG(v8::Local<v8::Value>, 2);

		hr = _new(v0, v1, v2, vr);

		METHOD_OVER(4, 4);

		ARG(v8::Local<v8::Array>, 0);
		ARG_String(1);
		ARG(int32_t, 2);
		ARG(v8::Local<v8::Value>, 3);

		hr = _new(v0, v1, v2, v3, vr);

		METHOD_OVER(4, 4);

		ARG(obj_ptr<X509Cert_base>, 0);
		ARG(obj_ptr<PKey_base>, 1);
		ARG(int32_t, 2);
		ARG(v8::Local<v8::Value>, 3);

		hr = _new(v0, v1, v2, v3, vr);

		METHOD_OVER(5, 5);

		ARG(obj_ptr<X509Cert_base>, 0);
		ARG(obj_ptr<PKey_base>, 1);
		ARG_String(2);
		ARG(int32_t, 3);
		ARG(v8::Local<v8::Value>, 4);

		hr = _new(v0, v1, v2, v3, v4, vr);

		CONSTRUCT_RETURN();
	}

	inline void SslServer_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SslServer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_run();

		METHOD_VOID();
	}

	inline void SslServer_base::s_asyncRun(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SslServer_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncRun();

		METHOD_VOID();
	}

}

#endif

