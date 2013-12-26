/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _PacketMessage_base_H_
#define _PacketMessage_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Message.h"

namespace fibjs
{

class Message_base;

class PacketMessage_base : public Message_base
{
public:
	// PacketMessage_base
	static result_t _new(int32_t maxSize, obj_ptr<PacketMessage_base>& retVal);
	virtual result_t get_maxSize(int32_t& retVal) = 0;
	virtual result_t set_maxSize(int32_t newVal) = 0;

	DECLARE_CLASSINFO(PacketMessage_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}


namespace fibjs
{
	inline ClassInfo& PacketMessage_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"maxSize", s_get_maxSize, s_set_maxSize}
		};

		static ClassData s_cd = 
		{ 
			"PacketMessage", s__new, 
			0, NULL, 0, NULL, 1, s_property, NULL, NULL,
			&Message_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void PacketMessage_base::s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PacketMessage_base);

		hr = pInst->get_maxSize(vr);

		METHOD_RETURN();
	}

	inline void PacketMessage_base::s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PacketMessage_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxSize(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void PacketMessage_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<PacketMessage_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 64);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

}

#endif

