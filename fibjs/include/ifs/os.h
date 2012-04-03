/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _os_base_H_
#define _os_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

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
	static result_t CPUInfo(v8::Handle<v8::Array>& retVal);
	static result_t networkInfo(v8::Handle<v8::Array>& retVal);

public:
	static ClassInfo& class_info()
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

		static ClassData s_cd = 
		{ 
			"os", NULL, 
			6, s_method, 0, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_hostname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_type(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_release(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_arch(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUInfo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_networkInfo(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> os_base::s_hostname(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = hostname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_type(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_release(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = release(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_arch(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		std::string vr;
		hr = arch(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUInfo(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		v8::Handle<v8::Array> vr;
		hr = CPUInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_networkInfo(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		v8::Handle<v8::Array> vr;
		hr = networkInfo(vr);

		METHOD_RETURN();
	}

}

#endif

