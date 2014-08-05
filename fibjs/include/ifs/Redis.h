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
class RedisHash_base;
class RedisList_base;
class RedisSet_base;
class Buffer_base;

class Redis_base : public object_base
{
public:
	// Redis_base
	virtual result_t command(const char* cmd, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal) = 0;
	virtual result_t set(const char* key, const char* value, int64_t ttl) = 0;
	virtual result_t setNX(const char* key, const char* value, int64_t ttl) = 0;
	virtual result_t setXX(const char* key, const char* value, int64_t ttl) = 0;
	virtual result_t mset(v8::Local<v8::Array> kvs) = 0;
	virtual result_t mset(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
	virtual result_t msetNX(v8::Local<v8::Array> kvs) = 0;
	virtual result_t msetNX(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
	virtual result_t append(const char* key, const char* value, int32_t& retVal) = 0;
	virtual result_t setRange(const char* key, int32_t offset, const char* value, int32_t& retVal) = 0;
	virtual result_t getRange(const char* key, int32_t start, int32_t end, std::string& retVal) = 0;
	virtual result_t strlen(const char* key, int32_t& retVal) = 0;
	virtual result_t bitcount(const char* key, int32_t start, int32_t end, int32_t& retVal) = 0;
	virtual result_t get(const char* key, std::string& retVal) = 0;
	virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<List_base>& retVal) = 0;
	virtual result_t mget(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<List_base>& retVal) = 0;
	virtual result_t getset(const char* key, const char* value, std::string& retVal) = 0;
	virtual result_t decr(const char* key, int64_t num, int64_t& retVal) = 0;
	virtual result_t incr(const char* key, int64_t num, int64_t& retVal) = 0;
	virtual result_t setBit(const char* key, int32_t offset, int32_t value, int32_t& retVal) = 0;
	virtual result_t getBit(const char* key, int32_t offset, int32_t& retVal) = 0;
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
	virtual result_t getHash(const char* key, obj_ptr<RedisHash_base>& retVal) = 0;
	virtual result_t getList(const char* key, obj_ptr<RedisList_base>& retVal) = 0;
	virtual result_t getSet(const char* key, obj_ptr<RedisSet_base>& retVal) = 0;
	virtual result_t dump(const char* key, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t restore(const char* key, Buffer_base* data, int64_t ttl) = 0;
	virtual result_t close() = 0;

	DECLARE_CLASSINFO(Redis_base);

public:
	static void s_command(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setXX(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_mset(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_msetNX(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setRange(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getRange(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_strlen(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_bitcount(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_mget(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getset(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_decr(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_incr(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_setBit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getBit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_type(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_expire(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ttl(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_persist(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_renameNX(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getHash(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getList(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_getSet(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_dump(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_restore(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "List.h"
#include "RedisHash.h"
#include "RedisList.h"
#include "RedisSet.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Redis_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"command", s_command},
			{"set", s_set},
			{"setNX", s_setNX},
			{"setXX", s_setXX},
			{"mset", s_mset},
			{"msetNX", s_msetNX},
			{"append", s_append},
			{"setRange", s_setRange},
			{"getRange", s_getRange},
			{"strlen", s_strlen},
			{"bitcount", s_bitcount},
			{"get", s_get},
			{"mget", s_mget},
			{"getset", s_getset},
			{"decr", s_decr},
			{"incr", s_incr},
			{"setBit", s_setBit},
			{"getBit", s_getBit},
			{"exists", s_exists},
			{"type", s_type},
			{"keys", s_keys},
			{"del", s_del},
			{"expire", s_expire},
			{"ttl", s_ttl},
			{"persist", s_persist},
			{"rename", s_rename},
			{"renameNX", s_renameNX},
			{"getHash", s_getHash},
			{"getList", s_getList},
			{"getSet", s_getSet},
			{"dump", s_dump},
			{"restore", s_restore},
			{"close", s_close}
		};

		static ClassData s_cd = 
		{ 
			"Redis", NULL, 
			33, s_method, 0, NULL, 0, NULL, NULL, NULL,
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

	inline void Redis_base::s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		OPT_ARG(int64_t, 2, 0);

		hr = pInst->setNX(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Redis_base::s_setXX(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);
		OPT_ARG(int64_t, 2, 0);

		hr = pInst->setXX(v0, v1, v2);

		METHOD_VOID();
	}

	inline void Redis_base::s_mset(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->mset(v0);

		METHOD_OVER(-1, 0);

		hr = pInst->mset(args);

		METHOD_VOID();
	}

	inline void Redis_base::s_msetNX(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->msetNX(v0);

		METHOD_OVER(-1, 0);

		hr = pInst->msetNX(args);

		METHOD_VOID();
	}

	inline void Redis_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->append(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_setRange(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 3);

		ARG(arg_string, 0);
		ARG(int32_t, 1);
		ARG(arg_string, 2);

		hr = pInst->setRange(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_getRange(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 3);

		ARG(arg_string, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->getRange(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_strlen(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->strlen(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_bitcount(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 1);

		ARG(arg_string, 0);
		OPT_ARG(int32_t, 1, 0);
		OPT_ARG(int32_t, 2, -1);

		hr = pInst->bitcount(v0, v1, v2, vr);

		METHOD_RETURN();
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

	inline void Redis_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->mget(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->mget(args, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_getset(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->getset(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_decr(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(int64_t, 1, 1);

		hr = pInst->decr(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_incr(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(int64_t, 1, 1);

		hr = pInst->incr(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_setBit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(3, 3);

		ARG(arg_string, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->setBit(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_getBit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(int32_t, 1);

		hr = pInst->getBit(v0, v1, vr);

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

	inline void Redis_base::s_getHash(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<RedisHash_base> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getHash(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_getList(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<RedisList_base> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getList(v0, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_getSet(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<RedisSet_base> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getSet(v0, vr);

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

