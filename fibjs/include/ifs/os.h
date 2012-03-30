/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _os_H_
#define _os_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class os_base : public object_base
{
public:
	// os_base
	static result_t hostname(std::string& retVal);
	static result_t type(std::string& retVal);
	static result_t release(std::string& retVal);
	static result_t arch(std::string& retVal);
	static result_t CPUInfo(v8::Local<v8::Array>& retVal);
	static result_t networkInfo(v8::Local<v8::Array>& retVal);

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"hostname", s_hostname},
			{"type", s_type},
			{"release", s_release},
			{"arch", s_arch},
			{"CPUInfo", s_CPUInfo},
			{"networkInfo", s_networkInfo}
		};

		static ClassInfo s_ci("os", 6, s_method, 0, NULL, &object_base::info());

		return s_ci;
	}

    virtual v8::Handle<v8::Value> ToJSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> s_hostname(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = hostname(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_type(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = type(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_release(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = release(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_arch(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = arch(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_CPUInfo(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		v8::Local<v8::Array> vr;
		hr = CPUInfo(vr);

		METHOD_RETURN();
	}

	static v8::Handle<v8::Value> s_networkInfo(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		v8::Local<v8::Array> vr;
		hr = networkInfo(vr);

		METHOD_RETURN();
	}

};

}

#endif

