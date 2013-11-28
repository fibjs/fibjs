/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _collection_base_H_
#define _collection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Map_base;
class List_base;
class Queue_base;

class collection_base : public module_base
{
	DECLARE_CLASSINFO(collection_base);

public:

};

}

#include "Map.h"
#include "List.h"
#include "Queue.h"

namespace fibjs
{
	inline ClassInfo& collection_base::class_info()
	{
		static ClassData::ClassObject s_object[] = 
		{
			{"Map", Map_base::class_info},
			{"List", List_base::class_info},
			{"Queue", Queue_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"collection", NULL, 
			0, NULL, 3, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

