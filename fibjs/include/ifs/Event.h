/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Event_base_H_
#define _Event_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Event_base : public object_base
{
public:
	// Event_base
	static result_t _new(obj_ptr<Event_base>& retVal);
	virtual result_t on(const char* ev, v8::Handle<v8::Function> func) = 0;
	virtual result_t once(const char* ev, v8::Handle<v8::Function> func) = 0;
	virtual result_t off(const char* ev, v8::Handle<v8::Function> func) = 0;
	virtual result_t trigger(const char* ev, const v8::Arguments& args) = 0;

public:
	static ClassInfo& class_info()
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
			"Event", s__new, 
			4, s_method, 0, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_on(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_once(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_off(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_trigger(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> Event_base::s__new(const v8::Arguments& args)
	{
		CONSTRUCT_ENTER(0, 0);

		obj_ptr<Event_base> vr;
		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Event_base::s_on(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);
		METHOD_INSTANCE(Event_base);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->on(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Event_base::s_once(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);
		METHOD_INSTANCE(Event_base);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->once(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Event_base::s_off(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);
		METHOD_INSTANCE(Event_base);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->off(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Event_base::s_trigger(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);
		METHOD_INSTANCE(Event_base);

		ARG_String(0);

		hr = pInst->trigger(v0, args);

		METHOD_VOID();
	}

}

#endif

