/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _uuid_base_H_
#define _uuid_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class uuidValue_base;
class Buffer_base;

class uuid_base : public object_base
{
	DECLARE_CLASS(uuid_base);

public:
	enum{
		_DNS = 0,
		_URL = 1,
		_OID = 2,
		_X509 = 3
	};

public:
	// uuid_base
	static result_t uuid(const char* s, obj_ptr<uuidValue_base>& retVal);
	static result_t uuid(Buffer_base* data, obj_ptr<uuidValue_base>& retVal);
	static result_t node(obj_ptr<uuidValue_base>& retVal);
	static result_t md5(int32_t ns, const char* name, obj_ptr<uuidValue_base>& retVal);
	static result_t random(obj_ptr<uuidValue_base>& retVal);
	static result_t sha1(int32_t ns, const char* name, obj_ptr<uuidValue_base>& retVal);

public:
	static void s_get_DNS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_URL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_OID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_X509(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_uuid(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_node(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_random(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "uuidValue.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& uuid_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"uuid", s_uuid},
			{"node", s_node},
			{"md5", s_md5},
			{"random", s_random},
			{"sha1", s_sha1}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"DNS", s_get_DNS, block_set},
			{"URL", s_get_URL, block_set},
			{"OID", s_get_OID, block_set},
			{"X509", s_get_X509, block_set}
		};

		static ClassData s_cd = 
		{ 
			"uuid", NULL, 
			5, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void uuid_base::s_get_DNS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DNS;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void uuid_base::s_get_URL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _URL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void uuid_base::s_get_OID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _OID;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void uuid_base::s_get_X509(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _X509;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void uuid_base::s_uuid(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = uuid(v0, vr);

		METHOD_OVER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = uuid(v0, vr);

		METHOD_RETURN();
	}

	inline void uuid_base::s_node(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(0, 0);

		hr = node(vr);

		METHOD_RETURN();
	}

	inline void uuid_base::s_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(arg_string, 1);

		hr = md5(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void uuid_base::s_random(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(0, 0);

		hr = random(vr);

		METHOD_RETURN();
	}

	inline void uuid_base::s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(arg_string, 1);

		hr = sha1(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

