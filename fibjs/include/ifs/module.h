/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _module_base_H_
#define _module_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class module_base : public object_base
{
	DECLARE_CLASSINFO(module_base);

public:

};

}

namespace fibjs
{
	inline ClassInfo& module_base::class_info()
	{
		static ClassData s_cd = 
		{ 
			"module", NULL, 
			0, NULL, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

