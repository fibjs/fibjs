/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _LevelDB_base_H_
#define _LevelDB_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class LevelDB_base : public object_base
{
public:
	// LevelDB_base
	virtual result_t has(Buffer_base* key, bool& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t has(const char* key, bool& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t get(Buffer_base* key, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t get(const char* key, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t put(v8::Local<v8::Object> map) = 0;
	virtual result_t put(Buffer_base* key, Buffer_base* value, exlib::AsyncEvent* ac) = 0;
	virtual result_t put(const char* key, Buffer_base* value, exlib::AsyncEvent* ac) = 0;
	virtual result_t remove(Buffer_base* key, exlib::AsyncEvent* ac) = 0;
	virtual result_t remove(const char* key, exlib::AsyncEvent* ac) = 0;
	virtual result_t begin(obj_ptr<LevelDB_base>& retVal) = 0;
	virtual result_t commit() = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(LevelDB_base);

public:
	static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_put(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_commit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE2(LevelDB_base, has, Buffer_base*, bool);
	ASYNC_MEMBERVALUE2(LevelDB_base, has, const char*, bool);
	ASYNC_MEMBERVALUE2(LevelDB_base, get, Buffer_base*, obj_ptr<Buffer_base>);
	ASYNC_MEMBERVALUE2(LevelDB_base, get, const char*, obj_ptr<Buffer_base>);
	ASYNC_MEMBER2(LevelDB_base, put, Buffer_base*, Buffer_base*);
	ASYNC_MEMBER2(LevelDB_base, put, const char*, Buffer_base*);
	ASYNC_MEMBER1(LevelDB_base, remove, Buffer_base*);
	ASYNC_MEMBER1(LevelDB_base, remove, const char*);
	ASYNC_MEMBER0(LevelDB_base, close);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& LevelDB_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"has", s_has},
			{"get", s_get},
			{"put", s_put},
			{"remove", s_remove},
			{"begin", s_begin},
			{"commit", s_commit},
			{"close", s_close}
		};

		static ClassData s_cd = 
		{ 
			"LevelDB", NULL, 
			7, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void LevelDB_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_has(v0, vr);

		METHOD_OVER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_has(v0, vr);

		METHOD_RETURN();
	}

	inline void LevelDB_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_get(v0, vr);

		METHOD_OVER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_get(v0, vr);

		METHOD_RETURN();
	}

	inline void LevelDB_base::s_put(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->put(v0);

		METHOD_OVER(2, 2);

		ARG(obj_ptr<Buffer_base>, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = pInst->ac_put(v0, v1);

		METHOD_OVER(2, 2);

		ARG(arg_string, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = pInst->ac_put(v0, v1);

		METHOD_VOID();
	}

	inline void LevelDB_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_remove(v0);

		METHOD_OVER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->ac_remove(v0);

		METHOD_VOID();
	}

	inline void LevelDB_base::s_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<LevelDB_base> vr;

		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(0, 0);

		hr = pInst->begin(vr);

		METHOD_RETURN();
	}

	inline void LevelDB_base::s_commit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(0, 0);

		hr = pInst->commit();

		METHOD_VOID();
	}

	inline void LevelDB_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(LevelDB_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

}

#endif

