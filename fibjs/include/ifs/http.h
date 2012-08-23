/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _http_base_H_
#define _http_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class HttpRequest_base;
class HttpResponse_base;

class http_base : public module_base
{
	DECLARE_CLASSINFO(http_base);

public:

};

}

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace fibjs
{
	inline ClassInfo& http_base::class_info()
	{
		static ClassData::ClassObject s_object[] = 
		{
			{"Request", HttpRequest_base::class_info},
			{"Response", HttpResponse_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"http", NULL, 
			0, NULL, 2, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


}

#endif

