/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _uuidValue_base_H_
#define _uuidValue_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class uuidValue_base : public object_base
{
public:
	// uuidValue_base
	virtual result_t data(obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t detail(std::string& retVal) = 0;

	DECLARE_CLASSINFO(uuidValue_base);

public:
	static void s_data(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_detail(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& uuidValue_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"data", s_data},
			{"detail", s_detail}
		};

		static ClassData s_cd = 
		{ 
			"uuidValue", NULL, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void uuidValue_base::s_data(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(uuidValue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->data(vr);

		METHOD_RETURN();
	}

	inline void uuidValue_base::s_detail(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(uuidValue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->detail(vr);

		METHOD_RETURN();
	}

}

#endif

