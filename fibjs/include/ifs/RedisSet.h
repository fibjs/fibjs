/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RedisSet_base_H_
#define _RedisSet_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class List_base;

class RedisSet_base : public object_base
{
	DECLARE_CLASS(RedisSet_base);

public:
	// RedisSet_base
	virtual result_t add(v8::Local<v8::Array> members, int32_t& retVal) = 0;
	virtual result_t add(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
	virtual result_t remove(v8::Local<v8::Array> members, int32_t& retVal) = 0;
	virtual result_t remove(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
	virtual result_t len(int32_t& retVal) = 0;
	virtual result_t exists(Buffer_base* member, bool& retVal) = 0;
	virtual result_t members(obj_ptr<List_base>& retVal) = 0;
	virtual result_t pop(obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t randMember(v8::Local<v8::Value>& retVal) = 0;
	virtual result_t randMember(int32_t count, v8::Local<v8::Value>& retVal) = 0;

public:
	static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_len(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_members(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_pop(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_randMember(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "List.h"

namespace fibjs
{
	inline ClassInfo& RedisSet_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"add", s_add},
			{"remove", s_remove},
			{"len", s_len},
			{"exists", s_exists},
			{"members", s_members},
			{"pop", s_pop},
			{"randMember", s_randMember}
		};

		static ClassData s_cd = 
		{ 
			"RedisSet", NULL, 
			7, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void RedisSet_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->add(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->add(args, vr);

		METHOD_RETURN();
	}

	inline void RedisSet_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->remove(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->remove(args, vr);

		METHOD_RETURN();
	}

	inline void RedisSet_base::s_len(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(0, 0);

		hr = pInst->len(vr);

		METHOD_RETURN();
	}

	inline void RedisSet_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->exists(v0, vr);

		METHOD_RETURN();
	}

	inline void RedisSet_base::s_members(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(0, 0);

		hr = pInst->members(vr);

		METHOD_RETURN();
	}

	inline void RedisSet_base::s_pop(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(0, 0);

		hr = pInst->pop(vr);

		METHOD_RETURN();
	}

	inline void RedisSet_base::s_randMember(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(RedisSet_base);
		METHOD_ENTER(0, 0);

		hr = pInst->randMember(vr);

		METHOD_OVER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->randMember(v0, vr);

		METHOD_RETURN();
	}

}

#endif

