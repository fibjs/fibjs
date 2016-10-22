/*
 * Service_other.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: lion
 */

#ifndef _WIN32

#include "object.h"
#include "ifs/Service.h"

namespace fibjs
{

result_t Service_base::_new(exlib::string name, v8::Local<v8::Function> worker,
                            v8::Local<v8::Object> event, obj_ptr<Service_base>& retVal,
                            v8::Local<v8::Object> This)
{
	return CHECK_ERROR(CALL_E_INVALID_CALL);
}

}

#endif