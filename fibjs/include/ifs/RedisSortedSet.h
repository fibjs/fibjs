/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RedisSortedSet_base_H_
#define _RedisSortedSet_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class List_base;

class RedisSortedSet_base : public object_base
{
public:
	// RedisSortedSet_base
	virtual result_t add(v8::Local<v8::Object> sms, int32_t& retVal) = 0;
	virtual result_t add(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
	virtual result_t score(Buffer_base* member, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t incr(Buffer_base* member, int64_t num, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t remove(v8::Local<v8::Array> members, int32_t& retVal) = 0;
	virtual result_t remove(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
	virtual result_t len(int32_t& retVal) = 0;
	virtual result_t count(int32_t min, int32_t max, int32_t& retVal) = 0;
	virtual result_t range(int32_t start, int32_t stop, bool withScores, obj_ptr<List_base>& retVal) = 0;
	virtual result_t rangeRev(int32_t start, int32_t stop, bool withScores, obj_ptr<List_base>& retVal) = 0;
	virtual result_t rank(Buffer_base* member, int32_t& retVal) = 0;
	virtual result_t rankRev(Buffer_base* member, int32_t& retVal) = 0;

	DECLARE_CLASSINFO(RedisSortedSet_base);

public:
	static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_score(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_incr(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_len(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_count(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_range(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rangeRev(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rank(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rankRev(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "List.h"

namespace fibjs
{
	inline ClassInfo& RedisSortedSet_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"add", s_add},
			{"score", s_score},
			{"incr", s_incr},
			{"remove", s_remove},
			{"len", s_len},
			{"count", s_count},
			{"range", s_range},
			{"rangeRev", s_rangeRev},
			{"rank", s_rank},
			{"rankRev", s_rankRev}
		};

		static ClassData s_cd = 
		{ 
			"RedisSortedSet", NULL, 
			10, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void RedisSortedSet_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->add(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->add(args, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_score(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->score(v0, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_incr(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Buffer_base>, 0);
		OPT_ARG(int64_t, 1, 1);

		hr = pInst->incr(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = pInst->remove(v0, vr);

		METHOD_OVER(-1, 0);

		hr = pInst->remove(args, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_len(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(0, 0);

		hr = pInst->len(vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_count(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->count(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_range(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->range(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_rangeRev(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(3, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		OPT_ARG(bool, 2, false);

		hr = pInst->rangeRev(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_rank(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->rank(v0, vr);

		METHOD_RETURN();
	}

	inline void RedisSortedSet_base::s_rankRev(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(RedisSortedSet_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->rankRev(v0, vr);

		METHOD_RETURN();
	}

}

#endif

