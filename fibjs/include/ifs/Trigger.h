/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Trigger_base_H_
#define _Trigger_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Trigger_base : public object_base
{
public:
	// Trigger_base
	static result_t _new(obj_ptr<Trigger_base>& retVal);
	virtual result_t on(const char* ev, v8::Handle<v8::Function> func) = 0;
	virtual result_t on(v8::Handle<v8::Object> map) = 0;
	virtual result_t once(const char* ev, v8::Handle<v8::Function> func) = 0;
	virtual result_t once(v8::Handle<v8::Object> map) = 0;
	virtual result_t off(const char* ev, v8::Handle<v8::Function> func) = 0;
	virtual result_t off(v8::Handle<v8::Object> map) = 0;
	virtual result_t trigger(const char* ev, const v8::Arguments& args) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_on(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_once(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_off(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_trigger(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& Trigger_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"on", s_on},
			{"once", s_once},
			{"off", s_off},
			{"trigger", s_trigger}
		};

		static ClassData s_cd = 
		{ 
			"Trigger", s__new, 
			4, s_method, 0, NULL, 0, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Trigger_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Trigger_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Trigger_base::s_on(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->on(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->on(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Trigger_base::s_once(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->once(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->once(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Trigger_base::s_off(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->off(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->off(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Trigger_base::s_trigger(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = pInst->trigger(v0, args);

		METHOD_VOID();
	}

}

#endif

