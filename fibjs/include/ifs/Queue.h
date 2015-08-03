/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Queue_base_H_
#define _Queue_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Queue_base : public object_base
{
	DECLARE_CLASS(Queue_base);

public:
	// Queue_base
	static result_t _new(int32_t size, obj_ptr<Queue_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t add(v8::Local<v8::Value> e, bool& retVal) = 0;
	virtual result_t offer(v8::Local<v8::Value> e, bool& retVal) = 0;
	virtual result_t remove(v8::Local<v8::Value>& retVal) = 0;
	virtual result_t poll(v8::Local<v8::Value>& retVal) = 0;
	virtual result_t element(v8::Local<v8::Value>& retVal) = 0;
	virtual result_t peek(v8::Local<v8::Value>& retVal) = 0;
	virtual result_t clear() = 0;
	virtual result_t toArray(v8::Local<v8::Array>& retVal) = 0;
	virtual result_t get_length(int32_t& retVal) = 0;

public:
	template<typename T>
	static void __new(const T &args);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_offer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_poll(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_element(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_peek(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

namespace fibjs
{
	inline ClassInfo& Queue_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"add", s_add, false},
			{"offer", s_offer, false},
			{"remove", s_remove, false},
			{"poll", s_poll, false},
			{"element", s_element, false},
			{"peek", s_peek, false},
			{"clear", s_clear, false},
			{"toArray", s_toArray, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"length", s_get_length, block_set, false}
		};

		static ClassData s_cd = 
		{ 
			"Queue", s__new, 
			8, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Queue_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Queue_base);

		hr = pInst->get_length(vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CONSTRUCT_INIT();
		__new(args);
	}

	template<typename T>void Queue_base::__new(const T& args)
	{
		obj_ptr<Queue_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void Queue_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->add(v0, vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s_offer(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->offer(v0, vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->remove(vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s_poll(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->poll(vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s_element(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->element(vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s_peek(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->peek(vr);

		METHOD_RETURN();
	}

	inline void Queue_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline void Queue_base::s_toArray(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_INSTANCE(Queue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->toArray(vr);

		METHOD_RETURN();
	}

}

#endif

