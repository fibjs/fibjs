/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _mq_base_H_
#define _mq_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class JSHandler_base;
class Handler_base;
class object_base;

class mq_base : public module_base
{
public:
	// mq_base
	static result_t invoke(obj_ptr<Handler_base>& hdlr, obj_ptr<object_base>& v, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(mq_base);

public:
	static v8::Handle<v8::Value> s_invoke(const v8::Arguments& args);

public:
	ASYNC_STATIC2(mq_base, invoke);
};

}

#include "JSHandler.h"
#include "Handler.h"

namespace fibjs
{
	inline ClassInfo& mq_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"invoke", s_invoke, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"JSHandler", JSHandler_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"mq", NULL, 
			1, s_method, 1, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> mq_base::s_invoke(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Handler_base>, 0);
		ARG(obj_ptr<object_base>, 1);

		hr = ac_invoke(v0, v1);

		METHOD_VOID();
	}

}

#endif

