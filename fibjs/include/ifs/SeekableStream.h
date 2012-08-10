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

class SeekableStream_base : public Stream_base
{
public:
	// SeekableStream_base
	virtual result_t seek(int64_t offset, int32_t whence) = 0;
	virtual result_t tell(int64_t& retVal) = 0;
	virtual result_t rewind() = 0;
	virtual result_t size(int64_t& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s_seek(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_tell(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rewind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_size(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& SeekableStream_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"seek", s_seek},
			{"tell", s_tell},
			{"rewind", s_rewind},
			{"size", s_size}
		};

		static ClassData s_cd = 
		{ 
			"SeekableStream", NULL, 
			4, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Stream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> SeekableStream_base::s_seek(const v8::Arguments& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(2, 2);

		ARG(int64_t, 0);
		ARG(int32_t, 1);

		hr = pInst->seek(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> SeekableStream_base::s_tell(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->tell(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> SeekableStream_base::s_rewind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->rewind();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> SeekableStream_base::s_size(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_INSTANCE(SeekableStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->size(vr);

		METHOD_RETURN();
	}

}

#endif

