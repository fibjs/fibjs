/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _vm_base_H_
#define _vm_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class SandBox_base;

class vm_base : public module_base
{
public:
	// vm_base
	static result_t create(v8::Handle<v8::Object> mods, obj_ptr<SandBox_base>& retVal);
	static result_t create(v8::Handle<v8::Object> mods, v8::Handle<v8::Function> require, obj_ptr<SandBox_base>& retVal);
	static result_t current(obj_ptr<SandBox_base>& retVal);

	DECLARE_CLASSINFO(vm_base);

public:
	static void s_create(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_current(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "SandBox.h"

namespace fibjs
{
	inline ClassInfo& vm_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"create", s_create, true},
			{"current", s_current, true}
		};

		static ClassData s_cd = 
		{ 
			"vm", NULL, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void vm_base::s_create(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SandBox_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = create(v0, vr);

		METHOD_OVER(2, 2);

		ARG(v8::Handle<v8::Object>, 0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = create(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void vm_base::s_current(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SandBox_base> vr;

		METHOD_ENTER(0, 0);

		hr = current(vr);

		METHOD_RETURN();
	}

}

#endif

