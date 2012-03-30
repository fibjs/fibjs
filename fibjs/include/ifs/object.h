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
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"dispose", m_dispose},
			{"toString", m_toString}
		};

		static ClassInfo s_ci("object", NULL, 2, s_method, 0, NULL, NULL);

		return s_ci;
	}

    virtual v8::Handle<v8::Value> ToJSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> m_dispose(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		hr = pInst->dispose();

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> m_toString(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(object_base);

		std::string vr;
		hr = pInst->toString(vr);

		METHOD_RETURN();
	}

};

}

#endif

