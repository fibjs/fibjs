/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _global_base_H_
#define _global_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class console_base;
class os_base;
class fs_base;

class global_base : public object_base
{
public:
	// global_base
	virtual result_t yield() = 0;
	virtual result_t get_console(console_base*& retVal) = 0;
	virtual result_t get_os(os_base*& retVal) = 0;
	virtual result_t get_fs(fs_base*& retVal) = 0;

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"yield", m_yield}
		};

		static ClassProperty s_property[] = 
		{
			{"console", m_get_console},
			{"os", m_get_os},
			{"fs", m_get_fs}
		};

		static ClassInfo s_ci("global", NULL, 1, s_method, 3, s_property, NULL, &object_base::info());

		return s_ci;
	}

    virtual v8::Handle<v8::Value> ToJSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> m_yield(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(global_base);

		hr = pInst->yield();

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> m_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(global_base);

		console_base* vr;
		hr = pInst->get_console(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> m_get_os(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(global_base);

		os_base* vr;
		hr = pInst->get_os(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> m_get_fs(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(global_base);

		fs_base* vr;
		hr = pInst->get_fs(vr);

		METHOD_RETURN();
	}

};

}

#include "console.h"
#include "os.h"
#include "fs.h"

#endif

