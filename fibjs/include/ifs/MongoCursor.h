/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoCursor_base_H_
#define _MongoCursor_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoCursor_base : public object_base
{
	DECLARE_CLASSINFO(MongoCursor_base);

public:

};

}

namespace fibjs
{
	inline ClassInfo& MongoCursor_base::class_info()
	{
		static ClassData s_cd = 
		{ 
			"MongoCursor", NULL, 
			0, NULL, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

