/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
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
public:
	static ClassInfo& class_info()
	{
		static ClassData s_cd = 
		{ 
			"module", NULL, 
			0, NULL, 0, NULL, NULL,
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

};

}

namespace fibjs
{

}

#endif

