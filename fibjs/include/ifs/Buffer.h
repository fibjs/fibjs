/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Buffer_base_H_
#define _Buffer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class Buffer_base : public object_base
{
public:
	// Buffer_base
	static result_t _new(const char* str, const char* encoding, Buffer_base*& retVal);
	virtual result_t _indexed_getter(uint32_t index, int32_t& retVal) = 0;
	virtual result_t _indexed_setter(uint32_t index, int32_t newVal) = 0;

public:
	static ClassInfo& info()
	{
		static ClassIndexed s_indexed = 
		{
			i_IndexedGetter, i_IndexedSetter
		};

		static ClassInfo s_ci("Buffer", c__new, 0, NULL, 0, NULL, &s_indexed, &object_base::info());

		return s_ci;
	}

    virtual v8::Handle<v8::Value> ToJSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> c__new(const v8::Arguments& args)
	{
		CONSTRUCT_ENTER(2, 0);

		OPT_ARG_String(0, "");
		OPT_ARG_String(1, "utf-8");

		Buffer_base* vr;
		hr = _new(v0, v1, vr);

		CONSTRUCT_RETURN();
	}

	static v8::Handle<v8::Value> i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Buffer_base);

		int32_t vr;
		hr = pInst->_indexed_getter(index, vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Buffer_base);

		PROPERTY_VAL_Integer();
		hr = pInst->_indexed_setter(index, v0);

		METHOD_VOID();
	}

};

}

#endif

