/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Image_base_H_
#define _Image_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class gd_base;
class Buffer_base;
class Stream_base;

class Image_base : public object_base
{
public:
	// Image_base
	virtual result_t getData(int32_t format, int32_t quality, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t save(Stream_base* stm, int32_t format, int32_t quality, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(Image_base);

public:
	static v8::Handle<v8::Value> s_getData(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_save(const v8::Arguments& args);

public:
	ASYNC_MEMBER3(Image_base, getData);
	ASYNC_VALUEBACK2(Image_base, getData, obj_ptr<Buffer_base>);
	ASYNC_MEMBER3(Image_base, save);
	ASYNC_CALLBACK3(Image_base, save);
};

}

#include "gd.h"
#include "Buffer.h"
#include "Stream.h"

namespace fibjs
{
	inline ClassInfo& Image_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"getData", s_getData},
			{"save", s_save}
		};

		static ClassData s_cd = 
		{ 
			"Image", NULL, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Image_base::s_getData(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, gd_base::_PNG);
		OPT_ARG(int32_t, 1, 85);

		hr = pInst->ac_getData(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_save(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 1);

		ARG(obj_ptr<Stream_base>, 0);
		OPT_ARG(int32_t, 1, gd_base::_PNG);
		OPT_ARG(int32_t, 2, 85);

		hr = pInst->ac_save(v0, v1, v2);

		METHOD_VOID();
	}

}

#endif

