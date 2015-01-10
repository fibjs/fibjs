/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SslSocket_base_H_
#define _SslSocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs
{

class Stream_base;
class X509Cert_base;
class PKey_base;

class SslSocket_base : public Stream_base
{
	DECLARE_CLASS(SslSocket_base);

public:
	// SslSocket_base
	static result_t _new(v8::Local<v8::Array> certs, obj_ptr<SslSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(X509Cert_base* crt, PKey_base* key, obj_ptr<SslSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t get_verification(int32_t& retVal) = 0;
	virtual result_t set_verification(int32_t newVal) = 0;
	virtual result_t get_ca(obj_ptr<X509Cert_base>& retVal) = 0;
	virtual result_t get_peerCert(obj_ptr<X509Cert_base>& retVal) = 0;
	virtual result_t connect(Stream_base* s, const char* server_name, int32_t& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t accept(Stream_base* s, obj_ptr<SslSocket_base>& retVal, exlib::AsyncEvent* ac) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_peerCert(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_accept(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE3(SslSocket_base, connect, Stream_base*, const char*, int32_t);
	ASYNC_MEMBERVALUE2(SslSocket_base, accept, Stream_base*, obj_ptr<SslSocket_base>);
};

}

#include "X509Cert.h"
#include "PKey.h"

namespace fibjs
{
	inline ClassInfo& SslSocket_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"connect", s_connect, false},
			{"accept", s_accept, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"verification", s_get_verification, s_set_verification},
			{"ca", s_get_ca, block_set},
			{"peerCert", s_get_peerCert, block_set}
		};

		static ClassData s_cd = 
		{ 
			"SslSocket", s__new, 
			2, s_method, 0, NULL, 3, s_property, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void SslSocket_base::s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslSocket_base);

		hr = pInst->get_verification(vr);

		METHOD_RETURN();
	}

	inline void SslSocket_base::s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslSocket_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_verification(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void SslSocket_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<X509Cert_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslSocket_base);

		hr = pInst->get_ca(vr);

		METHOD_RETURN();
	}

	inline void SslSocket_base::s_get_peerCert(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<X509Cert_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslSocket_base);

		hr = pInst->get_peerCert(vr);

		METHOD_RETURN();
	}

	inline void SslSocket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CONSTRUCT_INIT();
		__new(args);
	}

	template<typename T>void SslSocket_base::__new(const T& args)
	{
		obj_ptr<SslSocket_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate));

		hr = _new(v0, vr, args.This());

		METHOD_OVER(2, 2);

		ARG(obj_ptr<X509Cert_base>, 0);
		ARG(obj_ptr<PKey_base>, 1);

		hr = _new(v0, v1, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void SslSocket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(SslSocket_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Stream_base>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = pInst->ac_connect(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void SslSocket_base::s_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SslSocket_base> vr;

		METHOD_INSTANCE(SslSocket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->ac_accept(v0, vr);

		METHOD_RETURN();
	}

}

#endif

