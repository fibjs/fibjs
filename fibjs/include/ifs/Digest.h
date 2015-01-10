/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Digest_base_H_
#define _Digest_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class Digest_base : public object_base
{
	DECLARE_CLASS(Digest_base);

public:
	// Digest_base
	virtual result_t update(Buffer_base* data) = 0;
	virtual result_t digest(Buffer_base* data, obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t digest(obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t get_size(int32_t& retVal) = 0;

public:
	static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_digest(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Digest_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"update", s_update, false},
			{"digest", s_digest, false}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"size", s_get_size, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Digest", NULL, 
			2, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Digest_base::s_get_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Digest_base);

		hr = pInst->get_size(vr);

		METHOD_RETURN();
	}

	inline void Digest_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Digest_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->update(v0);

		METHOD_VOID();
	}

	inline void Digest_base::s_digest(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Digest_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->digest(v0, vr);

		METHOD_OVER(0, 0);

		hr = pInst->digest(vr);

		METHOD_RETURN();
	}

}

#endif

