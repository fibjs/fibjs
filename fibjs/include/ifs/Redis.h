/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Redis_base_H_
#define _Redis_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class List_base;
class Buffer_base;

class Redis_base : public object_base
{
public:
	// Redis_base
	virtual result_t command(const char* cmd, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal) = 0;
	virtual result_t set(const char* key, const char* value, int64_t ttl) = 0;
	virtual result_t get(const char* key, std::string& retVal) = 0;
	virtual result_t exists(const char* key, bool& retVal) = 0;
	virtual result_t type(const char* key, std::string& retVal) = 0;
	virtual result_t keys(const char* pattern, obj_ptr<List_base>& retVal) = 0;
	virtual result_t del(v8::Local<v8::Array> keys, int32_t& retVal) = 0;
	virtual result_t del(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
	virtual result_t expire(const char* key, int64_t ttl, bool& retVal) = 0;
	virtual result_t ttl(const char* key, int64_t& retVal) = 0;
	virtual result_t persist(const char* key, bool& retVal) = 0;
	virtual result_t rename(const char* key, const char* newkey) = 0;
	virtual result_t renameNX(const char* key, const char* newkey, bool& retVal) = 0;
	virtual result_t dump(const char* key, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t restore(const char* key, Buffer_base* data, int64_t ttl) = 0;
	virtual result_t close() = 0;

	DECLARE_CLASSINFO(Redis_base);

public:
	static void s_command(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_type(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_expire(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ttl(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_persist(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_renameNX(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_dump(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_restore(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "List.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Redis_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"command", s_command},
			{"set", s_set},
			{"get", s_get},
			{"exists", s_exists},
			{"type", s_type},
			{"keys", s_keys},
			{"del", s_del},
			{"expire", s_expire},
			{"ttl", s_ttl},
			{"persist", s_persist},
			{"rename", s_rename},
			{"renameNX", s_renameNX},
			{"dump", s_dump},
			{"restore", s_restore},
			{"close", s_close}
		};

		static ClassData s_cd = 
		{ 
			"Redis", NULL, 
			15, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Redis_base::s_command(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = pInst->command(v0, args, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		OPT_ARG(int64_t, 2, 0);

		hr = pInst->set(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Redis_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->get(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->exists(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_type(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->type(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->keys(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->del(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->del(args, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_expire(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(int64_t, 1);

		hr = pInst->expire(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_ttl(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ttl(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_persist(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->persist(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->rename(v0, v1);

		METHOD_VOID();
	}

	inline void Redis_base::s_renameNX(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->renameNX(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_dump(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->dump(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_restore(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<Buffer_base>, 1);
		OPT_ARG(int64_t, 2, 0);

		hr = pInst->restore(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Redis_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(0, 0);

		hr = pInst->close();

		METHOD_VOID();
	}

}

#endif

