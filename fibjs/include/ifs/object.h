/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _object_base_H_
#define _object_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class object_base : public object_base
{
public:
	// object_base
	virtual result_t dispose() = 0;
	virtual result_t toString(std::string& retVal) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"dispose", s_dispose},
			{"toString", s_toString}
		};

		static ClassData s_cd = 
		{ 
			"object", NULL, 
			2, s_method, 0, NULL, NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_dispose(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toString(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> object_base::s_dispose(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		hr = pInst->dispose();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> object_base::s_toString(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		std::string vr;
		hr = pInst->toString(vr);

		METHOD_RETURN();
	}

}

#endif

