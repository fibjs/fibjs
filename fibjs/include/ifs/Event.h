/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Event_base_H_
#define _Event_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Lock.h"

namespace fibjs
{

class Lock_base;

class Event_base : public Lock_base
{
public:
	// Event_base
	static result_t _new(bool value, obj_ptr<Event_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t isSet(bool& retVal) = 0;
	virtual result_t set() = 0;
	virtual result_t pulse() = 0;
	virtual result_t clear() = 0;
	virtual result_t wait() = 0;

	DECLARE_CLASSINFO(Event_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isSet(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_pulse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& Event_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"isSet", s_isSet},
			{"set", s_set},
			{"pulse", s_pulse},
			{"clear", s_clear},
			{"wait", s_wait}
		};

		static ClassData s_cd = 
		{ 
			"Event", s__new, 
			5, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Lock_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Event_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Event_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(bool, 0, false);

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void Event_base::s_isSet(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Event_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isSet(vr);

		METHOD_RETURN();
	}

	inline void Event_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Event_base);
		METHOD_ENTER(0, 0);

		hr = pInst->set();

		METHOD_VOID();
	}

	inline void Event_base::s_pulse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Event_base);
		METHOD_ENTER(0, 0);

		hr = pInst->pulse();

		METHOD_VOID();
	}

	inline void Event_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Event_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline void Event_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Event_base);
		METHOD_ENTER(0, 0);

		hr = pInst->wait();

		METHOD_VOID();
	}

}

#endif

