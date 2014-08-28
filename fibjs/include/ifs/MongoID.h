/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _MongoID_base_H_
#define _MongoID_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MongoID_base : public object_base
{
	DECLARE_CLASSINFO(MongoID_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:

};

}

namespace fibjs
{
	inline ClassInfo& MongoID_base::class_info()
	{
		static ClassData s_cd = 
		{ 
			"MongoID", NULL, 
			0, NULL, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

