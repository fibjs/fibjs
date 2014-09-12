/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlNamedNodeMap_base_H_
#define _XmlNamedNodeMap_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNodeList.h"

namespace fibjs
{

class XmlNodeList_base;
class XmlNode_base;

class XmlNamedNodeMap_base : public XmlNodeList_base
{
	DECLARE_CLASS(XmlNamedNodeMap_base);

public:
	// XmlNamedNodeMap_base
	virtual result_t getNamedItem(const char* name, obj_ptr<XmlNode_base>& retVal) = 0;
	virtual result_t removeNamedItem(const char* name, obj_ptr<XmlNode_base>& retVal) = 0;

public:
	static void s_getNamedItem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_removeNamedItem(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "XmlNode.h"

namespace fibjs
{
	inline ClassInfo& XmlNamedNodeMap_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"getNamedItem", s_getNamedItem},
			{"removeNamedItem", s_removeNamedItem}
		};

		static ClassData s_cd = 
		{ 
			"XmlNamedNodeMap", NULL, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&XmlNodeList_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void XmlNamedNodeMap_base::s_getNamedItem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlNode_base> vr;

		METHOD_INSTANCE(XmlNamedNodeMap_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->getNamedItem(v0, vr);

		METHOD_RETURN();
	}

	inline void XmlNamedNodeMap_base::s_removeNamedItem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<XmlNode_base> vr;

		METHOD_INSTANCE(XmlNamedNodeMap_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->removeNamedItem(v0, vr);

		METHOD_RETURN();
	}

}

#endif

