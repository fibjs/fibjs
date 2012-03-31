/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Function_base_H_
#define _Function_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class Function_base : public object_base
{
public:
	// Function_base
	static result_t start(const v8::Arguments& args);

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"start", s_start}
		};

		static ClassData s_cd = 
		{ 
			"Function", NULL, 
			1, s_method, 0, NULL, NULL,
			&object_base::info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual v8::Handle<v8::Value> JSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> s_start(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> Function_base::s_start(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 0);

		hr = start(args);

		METHOD_VOID();
	}

}

#endif

