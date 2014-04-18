/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Cert_base_H_
#define _X509Cert_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class PKey_base;

class X509Cert_base : public object_base
{
public:
	// X509Cert_base
	static result_t _new(obj_ptr<X509Cert_base>& retVal);
	virtual result_t load(Buffer_base* derCert) = 0;
	virtual result_t load(const char* txtCert) = 0;
	virtual result_t loadFile(const char* filename) = 0;
	virtual result_t dump(v8::Local<v8::Array>& retVal) = 0;
	virtual result_t clear() = 0;
	virtual result_t get_issuer(std::string& retVal) = 0;
	virtual result_t get_subject(std::string& retVal) = 0;
	virtual result_t get_notBefore(date_t& retVal) = 0;
	virtual result_t get_notAfter(date_t& retVal) = 0;
	virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;
	virtual result_t get_next(obj_ptr<X509Cert_base>& retVal) = 0;

	DECLARE_CLASSINFO(X509Cert_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_dump(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_issuer(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_notBefore(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_notAfter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_next(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Buffer.h"
#include "PKey.h"

namespace fibjs
{
	inline ClassInfo& X509Cert_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"load", s_load},
			{"loadFile", s_loadFile},
			{"dump", s_dump},
			{"clear", s_clear}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"issuer", s_get_issuer, block_set},
			{"subject", s_get_subject, block_set},
			{"notBefore", s_get_notBefore, block_set},
			{"notAfter", s_get_notAfter, block_set},
			{"publicKey", s_get_publicKey, block_set},
			{"next", s_get_next, block_set}
		};

		static ClassData s_cd = 
		{ 
			"X509Cert", s__new, 
			4, s_method, 0, NULL, 6, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void X509Cert_base::s_get_issuer(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Cert_base);

		hr = pInst->get_issuer(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s_get_subject(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Cert_base);

		hr = pInst->get_subject(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s_get_notBefore(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		date_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Cert_base);

		hr = pInst->get_notBefore(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s_get_notAfter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		date_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Cert_base);

		hr = pInst->get_notAfter(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<PKey_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Cert_base);

		hr = pInst->get_publicKey(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s_get_next(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<X509Cert_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(X509Cert_base);

		hr = pInst->get_next(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<X509Cert_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline void X509Cert_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Cert_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->load(v0);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = pInst->load(v0);

		METHOD_VOID();
	}

	inline void X509Cert_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Cert_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->loadFile(v0);

		METHOD_VOID();
	}

	inline void X509Cert_base::s_dump(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_INSTANCE(X509Cert_base);
		METHOD_ENTER(0, 0);

		hr = pInst->dump(vr);

		METHOD_RETURN();
	}

	inline void X509Cert_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Cert_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

}

#endif

