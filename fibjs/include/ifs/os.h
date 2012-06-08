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
#include "module.h"

namespace fibjs
{

class module_base;

class os_base : public module_base
{
public:
	// os_base
	static result_t hostname(std::string& retVal);
	static result_t type(std::string& retVal);
	static result_t release(std::string& retVal);
	static result_t arch(std::string& retVal);
	static result_t CPUInfo(v8::Handle<v8::Array>& retVal);
	static result_t CPUs(int32_t& retVal);
	static result_t networkInfo(v8::Handle<v8::Array>& retVal);

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

protected:
	static v8::Handle<v8::Value> s_hostname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_type(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_release(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_arch(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUInfo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUs(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_networkInfo(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& os_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"hostname", s_hostname, true},
			{"type", s_type, true},
			{"release", s_release, true},
			{"arch", s_arch, true},
			{"CPUInfo", s_CPUInfo, true},
			{"CPUs", s_CPUs, true},
			{"networkInfo", s_networkInfo, true}
		};

		static ClassData s_cd = 
		{ 
			"os", NULL, 
			7, s_method, 0, NULL, 0, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> os_base::s_hostname(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = hostname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_type(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_release(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = release(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_arch(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = arch(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUInfo(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = CPUInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUs(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_ENTER(0, 0);

		hr = CPUs(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_networkInfo(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = networkInfo(vr);

		METHOD_RETURN();
	}

}

#endif

