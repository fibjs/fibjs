/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Lock_base_H_
#define _Lock_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Lock_base : public object_base
{
public:
	// Lock_base
	static result_t _new(obj_ptr<Lock_base>& retVal);
	virtual result_t acquire(bool blocking, bool& retVal) = 0;
	virtual result_t release() = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_acquire(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_release(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& Lock_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"acquire", s_acquire},
			{"release", s_release}
		};

		static ClassData s_cd = 
		{ 
			"Lock", s__new, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Lock_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Lock_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Lock_base::s_acquire(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Lock_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(bool, 0, true);

		hr = pInst->acquire(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Lock_base::s_release(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Lock_base);
		METHOD_ENTER(0, 0);

		hr = pInst->release();

		METHOD_VOID();
	}

}

#endif

