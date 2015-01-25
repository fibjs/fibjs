/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlText_base_H_
#define _XmlText_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlCharacterData.h"

namespace fibjs
{

class XmlCharacterData_base;

class XmlText_base : public XmlCharacterData_base
{
	DECLARE_CLASS(XmlText_base);

public:
	// XmlText_base
	virtual result_t splitText(int32_t offset, obj_ptr<XmlText_base>& retVal) = 0;

public:
	static void s_splitText(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& XmlText_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"splitText", s_splitText, false}
		};

		static ClassData s_cd = 
		{ 
			"XmlText", NULL, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&XmlCharacterData_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void XmlText_base::s_splitText(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlText_base> vr;

		METHOD_INSTANCE(XmlText_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->splitText(v0, vr);

		METHOD_RETURN();
	}

}

#endif

