/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoDB_base_H_
#define _MongoDB_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoCollection_base;

class MongoDB_base : public object_base
{
public:
	// MongoDB_base
	virtual result_t getCollection(const char* name, obj_ptr<MongoCollection_base>& retVal) = 0;
	virtual result_t runCommand(v8::Handle<v8::Object> cmd, v8::Handle<v8::Object>& retVal) = 0;
	virtual result_t runCommand(const char* cmd, v8::Handle<v8::Value> arg, v8::Handle<v8::Object>& retVal) = 0;
	virtual result_t _named_getter(const char* property, obj_ptr<MongoCollection_base>& retVal) = 0;

	DECLARE_CLASSINFO(MongoDB_base);

public:
	static v8::Handle<v8::Value> s_getCollection(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_runCommand(const v8::Arguments& args);
	static v8::Handle<v8::Value> i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
};

}

#include "MongoCollection.h"

namespace fibjs
{
	inline ClassInfo& MongoDB_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"getCollection", s_getCollection},
			{"runCommand", s_runCommand}
		};

		static ClassData::ClassNamed s_named = 
		{
			i_NamedGetter, i_NamedSetter
		};

		static ClassData s_cd = 
		{ 
			"MongoDB", NULL, 
			2, s_method, 0, NULL, 0, NULL, NULL, &s_named,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> MongoDB_base::i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		obj_ptr<MongoCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(MongoDB_base);

		v8::String::Utf8Value k(property);
		if(class_info().has(*k))return v8::Handle<v8::Value>();

		hr = pInst->_named_getter(*k, vr);
		if(hr == CALL_RETURN_NULL)return v8::Handle<v8::Value>();

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoDB_base::s_getCollection(const v8::Arguments& args)
	{
		obj_ptr<MongoCollection_base> vr;

		METHOD_INSTANCE(MongoDB_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->getCollection(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> MongoDB_base::s_runCommand(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_INSTANCE(MongoDB_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->runCommand(v0, vr);

		METHOD_OVER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Value>, 1);

		hr = pInst->runCommand(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

