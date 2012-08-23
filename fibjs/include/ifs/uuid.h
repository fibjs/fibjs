/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _uuid_base_H_
#define _uuid_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Buffer_base;
class uuidValue_base;

class uuid_base : public module_base
{
public:
	enum{
		_DNS = 0,
		_URL = 1,
		_OID = 2,
		_X509 = 3
	};

public:
	// uuid_base
	static result_t uuid(obj_ptr<Buffer_base>& data, obj_ptr<uuidValue_base>& retVal);
	static result_t uuid(const char* s, obj_ptr<uuidValue_base>& retVal);
	static result_t node(obj_ptr<uuidValue_base>& retVal);
	static result_t md5(int32_t ns, const char* name, obj_ptr<uuidValue_base>& retVal);
	static result_t random(obj_ptr<uuidValue_base>& retVal);
	static result_t sha1(int32_t ns, const char* name, obj_ptr<uuidValue_base>& retVal);

	DECLARE_CLASSINFO(uuid_base);

public:
	static v8::Handle<v8::Value> s_get_DNS(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_URL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_OID(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_X509(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_uuid(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_node(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_md5(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_random(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sha1(const v8::Arguments& args);
};

}

#include "Buffer.h"
#include "uuidValue.h"

namespace fibjs
{
	inline ClassInfo& uuid_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"uuid", s_uuid, true},
			{"node", s_node, true},
			{"md5", s_md5, true},
			{"random", s_random, true},
			{"sha1", s_sha1, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"DNS", s_get_DNS, NULL, true},
			{"URL", s_get_URL, NULL, true},
			{"OID", s_get_OID, NULL, true},
			{"X509", s_get_X509, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"uuid", NULL, 
			5, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> uuid_base::s_get_DNS(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _DNS;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_get_URL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _URL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_get_OID(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _OID;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_get_X509(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _X509;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_uuid(const v8::Arguments& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = uuid(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = uuid(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_node(const v8::Arguments& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(0, 0);

		hr = node(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_md5(const v8::Arguments& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG_String(1);

		hr = md5(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_random(const v8::Arguments& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(0, 0);

		hr = random(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> uuid_base::s_sha1(const v8::Arguments& args)
	{
		obj_ptr<uuidValue_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG_String(1);

		hr = sha1(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

