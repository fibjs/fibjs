/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _io_base_H_
#define _io_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class MemoryStream_base;
class BufferedStream_base;

class io_base : public module_base
{
	DECLARE_CLASSINFO(io_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:

};

}

#include "MemoryStream.h"
#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& io_base::class_info()
	{
		static ClassData::ClassObject s_object[] = 
		{
			{"MemoryStream", MemoryStream_base::class_info},
			{"BufferedStream", BufferedStream_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"io", NULL, 
			0, NULL, 2, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

