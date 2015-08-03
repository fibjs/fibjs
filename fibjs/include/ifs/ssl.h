/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _ssl_base_H_
#define _ssl_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class SslSocket_base;
class SslHandler_base;
class SslServer_base;
class Stream_base;
class X509Cert_base;
class PKey_base;

class ssl_base : public object_base
{
	DECLARE_CLASS(ssl_base);

public:
	enum{
		_VERIFY_NONE = 0,
		_VERIFY_OPTIONAL = 1,
		_VERIFY_REQUIRED = 2,
		_BADCERT_EXPIRED = 1,
		_BADCERT_REVOKED = 2,
		_BADCERT_CN_MISMATCH = 4,
		_BADCERT_NOT_TRUSTED = 8
	};

public:
	// ssl_base
	static result_t connect(const char* url, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
	static result_t setClientCert(X509Cert_base* crt, PKey_base* key);
	static result_t loadClientCertFile(const char* crtFile, const char* keyFile, const char* password);
	static result_t get_ca(obj_ptr<X509Cert_base>& retVal);
	static result_t get_verification(int32_t& retVal);
	static result_t set_verification(int32_t newVal);

public:
	static void s_get_VERIFY_NONE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_VERIFY_OPTIONAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_VERIFY_REQUIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BADCERT_EXPIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BADCERT_REVOKED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BADCERT_CN_MISMATCH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BADCERT_NOT_TRUSTED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadClientCertFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);

public:
	ASYNC_STATICVALUE2(ssl_base, connect, const char*, obj_ptr<Stream_base>);
};

}

#include "SslSocket.h"
#include "SslHandler.h"
#include "SslServer.h"
#include "Stream.h"
#include "X509Cert.h"
#include "PKey.h"

namespace fibjs
{
	inline ClassInfo& ssl_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"connect", s_connect, true},
			{"setClientCert", s_setClientCert, true},
			{"loadClientCertFile", s_loadClientCertFile, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Socket", SslSocket_base::class_info},
			{"Handler", SslHandler_base::class_info},
			{"Server", SslServer_base::class_info}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"VERIFY_NONE", s_get_VERIFY_NONE, block_set, true},
			{"VERIFY_OPTIONAL", s_get_VERIFY_OPTIONAL, block_set, true},
			{"VERIFY_REQUIRED", s_get_VERIFY_REQUIRED, block_set, true},
			{"BADCERT_EXPIRED", s_get_BADCERT_EXPIRED, block_set, true},
			{"BADCERT_REVOKED", s_get_BADCERT_REVOKED, block_set, true},
			{"BADCERT_CN_MISMATCH", s_get_BADCERT_CN_MISMATCH, block_set, true},
			{"BADCERT_NOT_TRUSTED", s_get_BADCERT_NOT_TRUSTED, block_set, true},
			{"ca", s_get_ca, block_set, true},
			{"verification", s_get_verification, s_set_verification, true}
		};

		static ClassData s_cd = 
		{ 
			"ssl", NULL, 
			3, s_method, 3, s_object, 9, s_property, NULL, NULL,
			NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void ssl_base::s_get_VERIFY_NONE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _VERIFY_NONE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_VERIFY_OPTIONAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _VERIFY_OPTIONAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_VERIFY_REQUIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _VERIFY_REQUIRED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_BADCERT_EXPIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BADCERT_EXPIRED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_BADCERT_REVOKED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BADCERT_REVOKED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_BADCERT_CN_MISMATCH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BADCERT_CN_MISMATCH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_BADCERT_NOT_TRUSTED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BADCERT_NOT_TRUSTED;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void ssl_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<X509Cert_base> vr;

		PROPERTY_ENTER();

		hr = get_ca(vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_verification(vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_VAL(int32_t);

		hr = set_verification(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void ssl_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Stream_base> vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = ac_connect(v0, vr);

		METHOD_RETURN();
	}

	inline void ssl_base::s_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<X509Cert_base>, 0);
		ARG(obj_ptr<PKey_base>, 1);

		hr = setClientCert(v0, v1);

		METHOD_VOID();
	}

	inline void ssl_base::s_loadClientCertFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		OPT_ARG(arg_string, 2, "");

		hr = loadClientCertFile(v0, v1, v2);

		METHOD_VOID();
	}

}

#endif

