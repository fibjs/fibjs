/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _File_base_H_
#define _File_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "SeekableStream.h"

namespace fibjs
{

class SeekableStream_base;

class File_base : public SeekableStream_base
{
public:
	File_base()
	{
		File_base::class_info().Ref();
	}

	virtual ~File_base()
	{
		File_base::class_info().Unref();
	}

public:
	// File_base
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t truncate(int64_t bytes, exlib::AsyncEvent* ac) = 0;
	virtual result_t eof(bool& retVal) = 0;
	virtual result_t flush(exlib::AsyncEvent* ac) = 0;
	virtual result_t chmod(int32_t mode, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(File_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_eof(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_flush(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(File_base, truncate, int64_t);
	ASYNC_MEMBER0(File_base, flush);
	ASYNC_MEMBER1(File_base, chmod, int32_t);
};

}


namespace fibjs
{
	inline ClassInfo& File_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"truncate", s_truncate},
			{"eof", s_eof},
			{"flush", s_flush},
			{"chmod", s_chmod}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"name", s_get_name, block_set}
		};

		static ClassData s_cd = 
		{ 
			"File", NULL, 
			4, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&SeekableStream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void File_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(File_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline void File_base::s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(int64_t, 0);

		hr = pInst->ac_truncate(v0);

		METHOD_VOID();
	}

	inline void File_base::s_eof(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->eof(vr);

		METHOD_RETURN();
	}

	inline void File_base::s_flush(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_flush();

		METHOD_VOID();
	}

	inline void File_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_chmod(v0);

		METHOD_VOID();
	}

}

#endif

