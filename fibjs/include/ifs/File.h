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
#include "Stream.h"

namespace fibjs
{

class Stream_base;
class io_base;

class File_base : public Stream_base
{
public:
	// File_base
	static result_t _new(obj_ptr<File_base>& retVal);
	virtual result_t open(const char* fname, const char* mode, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncOpen(const char* fname, const char* mode) = 0;
	virtual result_t onopen(v8::Handle<v8::Function> func) = 0;
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t seek(int64_t offset, int32_t whence) = 0;
	virtual result_t tell(int64_t& retVal) = 0;
	virtual result_t rewind() = 0;
	virtual result_t truncate(int64_t bytes, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncTruncate(int64_t bytes) = 0;
	virtual result_t ontruncate(v8::Handle<v8::Function> func) = 0;
	virtual result_t size(int64_t& retVal) = 0;
	virtual result_t eof(bool& retVal) = 0;
	virtual result_t flush(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncFlush() = 0;
	virtual result_t onflush(v8::Handle<v8::Function> func) = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncClose() = 0;
	virtual result_t onclose(v8::Handle<v8::Function> func) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Stream_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(name);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_open(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncOpen(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onopen(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_seek(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tell(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rewind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_truncate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncTruncate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ontruncate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_size(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_eof(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_flush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncFlush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onflush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncClose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onclose(const v8::Arguments& args);

public:
	ASYNC_MEMBER2(File_base, open);
	ASYNC_CALLBACK2(File_base, open);
	ASYNC_MEMBER1(File_base, truncate);
	ASYNC_CALLBACK1(File_base, truncate);
	ASYNC_MEMBER0(File_base, flush);
	ASYNC_CALLBACK0(File_base, flush);
	ASYNC_MEMBER0(File_base, close);
	ASYNC_CALLBACK0(File_base, close);
};

}

#include "io.h"

namespace fibjs
{
	inline ClassInfo& File_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"open", s_open},
			{"asyncOpen", s_asyncOpen},
			{"onopen", s_onopen},
			{"seek", s_seek},
			{"tell", s_tell},
			{"rewind", s_rewind},
			{"truncate", s_truncate},
			{"asyncTruncate", s_asyncTruncate},
			{"ontruncate", s_ontruncate},
			{"size", s_size},
			{"eof", s_eof},
			{"flush", s_flush},
			{"asyncFlush", s_asyncFlush},
			{"onflush", s_onflush},
			{"close", s_close},
			{"asyncClose", s_asyncClose},
			{"onclose", s_onclose}
		};

		static ClassProperty s_property[] = 
		{
			{"name", s_get_name}
		};

		static ClassData s_cd = 
		{ 
			"File", s__new, 
			17, s_method, 0, NULL, 1, s_property, NULL,
			&Stream_base::class_info()
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

	inline v8::Handle<v8::Value> File_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<File_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_open(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		hr = pInst->ac_open(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_asyncOpen(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "r");

		hr = pInst->asyncOpen(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_onopen(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onopen(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_seek(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(2, 1);

		ARG(int64_t, 0);
		OPT_ARG(int32_t, 1, io_base::_SEEK_SET);

		hr = pInst->seek(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_tell(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->tell(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_rewind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->rewind();

		METHOD_VOID();
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

	inline v8::Handle<v8::Value> File_base::s_size(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->size(vr);

		METHOD_RETURN();
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

