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
class Stat_base;

class File_base : public Stream_base
{
public:
	// File_base
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t stat(obj_ptr<Stat_base>& retVal) = 0;
	virtual result_t size(double& retVal) = 0;
	virtual result_t eof(bool& retVal) = 0;
	virtual result_t seek(double offset, int32_t whence) = 0;
	virtual result_t tell(double& retVal) = 0;
	virtual result_t rewind() = 0;
	virtual result_t flush() = 0;
	virtual result_t close() = 0;
	virtual result_t truncate(double bytes) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"stat", s_stat},
			{"size", s_size},
			{"eof", s_eof},
			{"seek", s_seek},
			{"tell", s_tell},
			{"rewind", s_rewind},
			{"flush", s_flush},
			{"close", s_close},
			{"truncate", s_truncate}
		};

		static ClassProperty s_property[] = 
		{
			{"name", s_get_name}
		};

		static ClassData s_cd = 
		{ 
			"File", NULL, 
			9, s_method, 1, s_property, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

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

private:
	static v8::Handle<v8::Value> s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_stat(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_size(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_eof(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_seek(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tell(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rewind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_flush(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_truncate(const v8::Arguments& args);

private:
	ASYNC_MEMBER1(File_base, stat);
	ASYNC_MEMBER1(File_base, size);
	ASYNC_MEMBER1(File_base, eof);
	ASYNC_MEMBER2(File_base, seek);
	ASYNC_MEMBER1(File_base, tell);
	ASYNC_MEMBER0(File_base, rewind);
	ASYNC_MEMBER0(File_base, flush);
	ASYNC_MEMBER0(File_base, close);
	ASYNC_MEMBER1(File_base, truncate);
};

}

#include "Stat.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> File_base::s_get_name(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(File_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_stat(const v8::Arguments& args)
	{
		obj_ptr<Stat_base> vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_stat(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_size(const v8::Arguments& args)
	{
		double vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_size(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_eof(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_eof(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_seek(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(2, 1);

		ARG(double, 0);
		OPT_ARG(int32_t, 1, SEEK_SET);

		hr = pInst->ac_seek(s_acPool, v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_tell(const v8::Arguments& args)
	{
		double vr;

		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_tell(s_acPool, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> File_base::s_rewind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_rewind(s_acPool);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_flush(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_flush(s_acPool);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close(s_acPool);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> File_base::s_truncate(const v8::Arguments& args)
	{
		METHOD_INSTANCE(File_base);
		METHOD_ENTER(1, 1);

		ARG(double, 0);

		hr = pInst->ac_truncate(s_acPool, v0);

		METHOD_VOID();
	}

}

#endif

