/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SslHandler_base_H_
#define _SslHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;
class X509Cert_base;
class PKey_base;

class SslHandler_base : public Handler_base
{
public:
	// SslHandler_base
	static result_t _new(v8::Local<v8::Array> certs, v8::Local<v8::Value> hdlr, obj_ptr<SslHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(X509Cert_base* crt, PKey_base* key, v8::Local<v8::Value> hdlr, obj_ptr<SslHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t get_verification(int32_t& retVal) = 0;
	virtual result_t set_verification(int32_t newVal) = 0;
	virtual result_t get_ca(obj_ptr<X509Cert_base>& retVal) = 0;
	virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
	virtual result_t set_handler(Handler_base* newVal) = 0;

	DECLARE_CLASSINFO(SslHandler_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_handler(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_handler(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

#include "X509Cert.h"
#include "PKey.h"

namespace fibjs
{
	inline ClassInfo& SslHandler_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"verification", s_get_verification, s_set_verification},
			{"ca", s_get_ca, block_set},
			{"handler", s_get_handler, s_set_handler}
		};

		static ClassData s_cd = 
		{ 
			"SslHandler", s__new, 
			0, NULL, 0, NULL, 3, s_property, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void SslHandler_base::s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslHandler_base);

		hr = pInst->get_verification(vr);

		METHOD_RETURN();
	}

	inline void SslHandler_base::s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslHandler_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_verification(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void SslHandler_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<X509Cert_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslHandler_base);

		hr = pInst->get_ca(vr);

		METHOD_RETURN();
	}

	inline void SslHandler_base::s_get_handler(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Handler_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslHandler_base);

		hr = pInst->get_handler(vr);

		METHOD_RETURN();
	}

	inline void SslHandler_base::s_set_handler(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(SslHandler_base);

		PROPERTY_VAL(obj_ptr<Handler_base>);
		hr = pInst->set_handler(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void SslHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SslHandler_base> vr;

		CONSTRUCT_ENTER(2, 2);

		ARG(v8::Local<v8::Array>, 0);
		ARG(v8::Local<v8::Value>, 1);

		hr = _new(v0, v1, vr, args.This());

		METHOD_OVER(3, 3);

		ARG(obj_ptr<X509Cert_base>, 0);
		ARG(obj_ptr<PKey_base>, 1);
		ARG(v8::Local<v8::Value>, 2);

		hr = _new(v0, v1, v2, vr, args.This());

		CONSTRUCT_RETURN();
	}

}

#endif

