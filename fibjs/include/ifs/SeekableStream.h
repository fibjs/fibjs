/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _SeekableStream_base_H_
#define _SeekableStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs
{

class Stream_base;
class Buffer_base;
class Stat_base;

class SeekableStream_base : public Stream_base
{
public:
	// SeekableStream_base
	virtual result_t seek(int64_t offset, int32_t whence) = 0;
	virtual result_t tell(int64_t& retVal) = 0;
	virtual result_t rewind() = 0;
	virtual result_t size(int64_t& retVal) = 0;
	virtual result_t readAll(obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncReadAll() = 0;
	virtual result_t onreadall(v8::Handle<v8::Function> func) = 0;
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncStat() = 0;
	virtual result_t onstat(v8::Handle<v8::Function> func) = 0;

	DECLARE_CLASSINFO(SeekableStream_base);

public:
	static void s_seek(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_tell(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_rewind(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_size(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncReadAll(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_onreadall(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_asyncStat(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_onstat(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE1(SeekableStream_base, readAll, obj_ptr<Buffer_base>);
	ASYNC_VALUEBACK0(SeekableStream_base, readAll, obj_ptr<Buffer_base>);
	ASYNC_MEMBERVALUE1(SeekableStream_base, stat, obj_ptr<Stat_base>);
	ASYNC_VALUEBACK0(SeekableStream_base, stat, obj_ptr<Stat_base>);
};

}

#include "Buffer.h"
#include "Stat.h"

namespace fibjs
{
	inline ClassInfo& SeekableStream_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"seek", s_seek},
			{"tell", s_tell},
			{"rewind", s_rewind},
			{"size", s_size},
			{"readAll", s_readAll},
			{"asyncReadAll", s_asyncReadAll},
			{"onreadall", s_onreadall},
			{"stat", s_stat},
			{"asyncStat", s_asyncStat},
			{"onstat", s_onstat}
		};

		static ClassData s_cd = 
		{ 
			"SeekableStream", NULL, 
			10, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void SeekableStream_base::s_seek(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(2, 2);

		ARG(int64_t, 0);
		ARG(int32_t, 1);

		hr = pInst->seek(v0, v1);

		METHOD_VOID();
	}

	inline void SeekableStream_base::s_tell(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->tell(vr);

		METHOD_RETURN();
	}

	inline void SeekableStream_base::s_rewind(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->rewind();

		METHOD_VOID();
	}

	inline void SeekableStream_base::s_size(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->size(vr);

		METHOD_RETURN();
	}

	inline void SeekableStream_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_readAll(vr);

		METHOD_RETURN();
	}

	inline void SeekableStream_base::s_asyncReadAll(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncReadAll();

		METHOD_VOID();
	}

	inline void SeekableStream_base::s_onreadall(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onreadall(v0);

		METHOD_VOID();
	}

	inline void SeekableStream_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Stat_base> vr;

		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_stat(vr);

		METHOD_RETURN();
	}

	inline void SeekableStream_base::s_asyncStat(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncStat();

		METHOD_VOID();
	}

	inline void SeekableStream_base::s_onstat(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onstat(v0);

		METHOD_VOID();
	}

}

#endif

