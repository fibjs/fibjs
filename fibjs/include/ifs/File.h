/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
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
	// File_base
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t truncate(int64_t bytes, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncTruncate(int64_t bytes) = 0;
	virtual result_t ontruncate(v8::Handle<v8::Function> func) = 0;
	virtual result_t eof(bool& retVal) = 0;
	virtual result_t flush(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncFlush() = 0;
	virtual result_t onflush(v8::Handle<v8::Function> func) = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncClose() = 0;
	virtual result_t onclose(v8::Handle<v8::Function> func) = 0;

	DECLARE_CLASSINFO(File_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = SeekableStream_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(name);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_truncate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncTruncate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ontruncate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_eof(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_flush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncFlush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onflush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncClose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onclose(const v8::Arguments& args);

public:
	ASYNC_MEMBER1(File_base, truncate);
	ASYNC_CALLBACK1(File_base, truncate);
	ASYNC_MEMBER0(File_base, flush);
	ASYNC_CALLBACK0(File_base, flush);
	ASYNC_MEMBER0(File_base, close);
	ASYNC_CALLBACK0(File_base, close);
};

}


namespace fibjs
{
	inline ClassInfo& File_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"truncate", s_truncate},
			{"asyncTruncate", s_asyncTruncate},
			{"ontruncate", s_ontruncate},
			{"eof", s_eof},
			{"flush", s_flush},
			{"asyncFlush", s_asyncFlush},
			{"onflush", s_onflush},
			{"close", s_close},
			{"asyncClose", s_asyncClose},
			{"onclose", s_onclose}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"name", s_get_name}
		};

		static ClassData s_cd = 
		{ 
			"File", NULL, 
			10, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&SeekableStream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> File_base::s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(File_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_truncate(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(int64_t, 0);

		hr = pInst->ac_truncate(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_asyncTruncate(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(int64_t, 0);

		hr = pInst->asyncTruncate(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_ontruncate(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->ontruncate(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_eof(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->eof(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_flush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_flush();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_asyncFlush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncFlush();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_onflush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onflush(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_asyncClose(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->asyncClose();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_onclose(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onclose(v0);

		METHOD_VOID();
	}

}

#endif

