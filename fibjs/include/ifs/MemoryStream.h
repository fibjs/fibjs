/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MemoryStream_base_H_
#define _MemoryStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "SeekableStream.h"

namespace fibjs
{

class SeekableStream_base;

class MemoryStream_base : public SeekableStream_base
{
public:
	// MemoryStream_base
	static result_t _new(obj_ptr<MemoryStream_base>& retVal);
	virtual result_t setTime(date_t d) = 0;
	virtual result_t clone(obj_ptr<MemoryStream_base>& retVal) = 0;
	virtual result_t clear() = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_setTime(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_clone(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& MemoryStream_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"setTime", s_setTime},
			{"clone", s_clone},
			{"clear", s_clear}
		};

		static ClassData s_cd = 
		{ 
			"MemoryStream", s__new, 
			3, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&SeekableStream_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> MemoryStream_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<MemoryStream_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_setTime(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(1, 1);

		ARG(date_t, 0);

		hr = pInst->setTime(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_clone(const v8::Arguments& args)
	{
		obj_ptr<MemoryStream_base> vr;

		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clone(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MemoryStream_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(MemoryStream_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

}

#endif

