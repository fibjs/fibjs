/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Req_base_H_
#define _X509Req_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class PKey_base;
class hash_base;
class Buffer_base;
class X509Cert_base;

class X509Req_base : public object_base
{
public:
	// X509Req_base
	static result_t _new(obj_ptr<X509Req_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(const char* subject, PKey_base* key, int32_t hash, obj_ptr<X509Req_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t load(Buffer_base* derReq) = 0;
	virtual result_t load(const char* pemReq) = 0;
	virtual result_t loadFile(const char* filename) = 0;
	virtual result_t exportPem(std::string& retVal) = 0;
	virtual result_t exportDer(obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t sign(const char* issuer, PKey_base* key, v8::Local<v8::Object> opts, obj_ptr<X509Cert_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t get_subject(std::string& retVal) = 0;
	virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;

	DECLARE_CLASSINFO(X509Req_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
	ASYNC_MEMBERVALUE4(X509Req_base, sign, const char*, PKey_base*, v8::Local<v8::Object>, obj_ptr<X509Cert_base>);
};

}

#include "PKey.h"
#include "hash.h"
#include "Buffer.h"
#include "X509Cert.h"

namespace fibjs
{
	inline ClassInfo& X509Req_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"load", s_load},
			{"loadFile", s_loadFile},
			{"exportPem", s_exportPem},
			{"exportDer", s_exportDer},
			{"sign", s_sign}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"subject", s_get_subject, block_set},
			{"publicKey", s_get_publicKey, block_set}
		};

		static ClassData s_cd = 
		{ 
			"X509Req", s__new, 
			5, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void X509Req_base::s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Req_base);

		hr = pInst->get_subject(vr);

		METHOD_RETURN();
	}

	inline void X509Req_base::s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<PKey_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Req_base);

		hr = pInst->get_publicKey(vr);

		METHOD_RETURN();
	}

	inline void X509Req_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<X509Req_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr, args.This());

		METHOD_OVER(3, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<PKey_base>, 1);
		OPT_ARG(int32_t, 2, hash_base::_SHA1);

		hr = _new(v0, v1, v2, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void X509Req_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->load(v0);

		METHOD_OVER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->load(v0);

		METHOD_VOID();
	}

	inline void X509Req_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->loadFile(v0);

		METHOD_VOID();
	}

	inline void X509Req_base::s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(0, 0);

		hr = pInst->exportPem(vr);

		METHOD_RETURN();
	}

	inline void X509Req_base::s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(0, 0);

		hr = pInst->exportDer(vr);

		METHOD_RETURN();
	}

	inline void X509Req_base::s_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<X509Cert_base> vr;

		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(3, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<PKey_base>, 1);
		OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

		hr = pInst->ac_sign(v0, v1, v2, vr);

		METHOD_RETURN();
	}

}

#endif

