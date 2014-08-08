/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _PacketHandler_base_H_
#define _PacketHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;
class Stats_base;

class PacketHandler_base : public Handler_base
{
public:
	// PacketHandler_base
	static result_t _new(v8::Local<v8::Value> hdlr, obj_ptr<PacketHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t get_maxSize(int32_t& retVal) = 0;
	virtual result_t set_maxSize(int32_t newVal) = 0;
	virtual result_t get_stats(obj_ptr<Stats_base>& retVal) = 0;

	DECLARE_CLASSINFO(PacketHandler_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Stats.h"

namespace fibjs
{
	inline ClassInfo& PacketHandler_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"maxSize", s_get_maxSize, s_set_maxSize},
			{"stats", s_get_stats, block_set}
		};

		static ClassData s_cd = 
		{ 
			"PacketHandler", s__new, 
			0, NULL, 0, NULL, 2, s_property, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void PacketHandler_base::s_get_maxSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PacketHandler_base);

		hr = pInst->get_maxSize(vr);

		METHOD_RETURN();
	}

	inline void PacketHandler_base::s_set_maxSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PacketHandler_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxSize(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void PacketHandler_base::s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Stats_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PacketHandler_base);

		hr = pInst->get_stats(vr);

		METHOD_RETURN();
	}

	inline void PacketHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<PacketHandler_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

}

#endif

