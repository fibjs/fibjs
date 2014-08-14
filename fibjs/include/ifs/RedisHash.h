/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RedisHash_base_H_
#define _RedisHash_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class List_base;

class RedisHash_base : public object_base
{
public:
	// RedisHash_base
	virtual result_t set(Buffer_base* field, Buffer_base* value) = 0;
	virtual result_t setNX(Buffer_base* field, Buffer_base* value) = 0;
	virtual result_t mset(v8::Local<v8::Array> kvs) = 0;
	virtual result_t mset(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
	virtual result_t get(Buffer_base* field, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t mget(v8::Local<v8::Array> fields, obj_ptr<List_base>& retVal) = 0;
	virtual result_t mget(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<List_base>& retVal) = 0;
	virtual result_t incr(Buffer_base* field, int64_t num, int64_t& retVal) = 0;
	virtual result_t getAll(obj_ptr<List_base>& retVal) = 0;
	virtual result_t keys(obj_ptr<List_base>& retVal) = 0;
	virtual result_t len(int32_t& retVal) = 0;
	virtual result_t exists(Buffer_base* field, bool& retVal) = 0;
	virtual result_t del(v8::Local<v8::Array> fields, int32_t& retVal) = 0;
	virtual result_t del(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;

	DECLARE_CLASSINFO(RedisHash_base);

public:
	static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_mset(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_mget(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_incr(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getAll(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_len(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "List.h"

namespace fibjs
{
	inline ClassInfo& RedisHash_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"set", s_set},
			{"setNX", s_setNX},
			{"mset", s_mset},
			{"get", s_get},
			{"mget", s_mget},
			{"incr", s_incr},
			{"getAll", s_getAll},
			{"keys", s_keys},
			{"len", s_len},
			{"exists", s_exists},
			{"del", s_del}
		};

		static ClassData s_cd = 
		{ 
			"RedisHash", NULL, 
			11, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void RedisHash_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Buffer_base>, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = pInst->set(v0, v1);

		METHOD_VOID();
	}

	inline void RedisHash_base::s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Buffer_base>, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = pInst->setNX(v0, v1);

		METHOD_VOID();
	}

	inline void RedisHash_base::s_mset(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->mset(v0);

		METHOD_OVER(-1, 0);

		hr = pInst->mset(args);

		METHOD_VOID();
	}

	inline void RedisHash_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->get(v0, vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->mget(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->mget(args, vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_incr(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Buffer_base>, 0);
		OPT_ARG(int64_t, 1, 1);

		hr = pInst->incr(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_getAll(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(0, 0);

		hr = pInst->getAll(vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(0, 0);

		hr = pInst->keys(vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_len(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(0, 0);

		hr = pInst->len(vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->exists(v0, vr);

		METHOD_RETURN();
	}

	inline void RedisHash_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisHash_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->del(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->del(args, vr);

		METHOD_RETURN();
	}

}

#endif

