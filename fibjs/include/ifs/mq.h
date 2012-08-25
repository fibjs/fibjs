/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _mq_base_H_
#define _mq_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class JSHandler_base;

class mq_base : public module_base
{
	DECLARE_CLASSINFO(mq_base);

public:

};

}

#include "JSHandler.h"

namespace fibjs
{
	inline ClassInfo& mq_base::class_info()
	{
		static ClassData::ClassObject s_object[] = 
		{
			{"JSHandler", JSHandler_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"mq", NULL, 
			0, NULL, 1, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

