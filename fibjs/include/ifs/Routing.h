/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Routing_base_H_
#define _Routing_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;

class Routing_base : public Handler_base
{
public:
	// Routing_base
	static result_t _new(obj_ptr<Routing_base>& retVal);
	static result_t _new(v8::Handle<v8::Object> map, obj_ptr<Routing_base>& retVal);
	virtual result_t append(const char* pattern, obj_ptr<Handler_base>& hdlr) = 0;
	virtual result_t append(const char* pattern, v8::Handle<v8::Function> hdlr) = 0;
	virtual result_t append(v8::Handle<v8::Object> map) = 0;

	DECLARE_CLASSINFO(Routing_base);

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_append(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& Routing_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"append", s_append}
		};

		static ClassData s_cd = 
		{ 
			"Routing", s__new, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Routing_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Routing_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Routing_base::s_append(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Routing_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(obj_ptr<Handler_base>, 1);

		hr = pInst->append(v0, v1);

		METHOD_OVER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = pInst->append(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->append(v0);

		METHOD_VOID();
	}

}

#endif

