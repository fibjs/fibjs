/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Chain_base_H_
#define _Chain_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;

class Chain_base : public Handler_base
{
public:
	// Chain_base
	static result_t _new(obj_ptr<Chain_base>& retVal);
	static result_t _new(v8::Handle<v8::Array> hdlrs, obj_ptr<Chain_base>& retVal);
	virtual result_t apppend(obj_ptr<Handler_base>& hdlr) = 0;
	virtual result_t append(v8::Handle<v8::Function> hdlr) = 0;
	virtual result_t apped(v8::Handle<v8::Array> hdlrs) = 0;

	DECLARE_CLASSINFO(Chain_base);

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_apppend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_append(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_apped(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& Chain_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"apppend", s_apppend},
			{"append", s_append},
			{"apped", s_apped}
		};

		static ClassData s_cd = 
		{ 
			"Chain", s__new, 
			3, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Chain_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Chain_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Array>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Chain_base::s_apppend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Chain_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Handler_base>, 0);

		hr = pInst->apppend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Chain_base::s_append(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Chain_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->append(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Chain_base::s_apped(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Chain_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Array>, 0);

		hr = pInst->apped(v0);

		METHOD_VOID();
	}

}

#endif

