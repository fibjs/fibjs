/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoCollection_base_H_
#define _MongoCollection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoCollection_base : public object_base
{
	DECLARE_CLASSINFO(MongoCollection_base);

public:

};

}

namespace fibjs
{
	inline ClassInfo& MongoCollection_base::class_info()
	{
		static ClassData s_cd = 
		{ 
			"MongoCollection", NULL, 
			0, NULL, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

