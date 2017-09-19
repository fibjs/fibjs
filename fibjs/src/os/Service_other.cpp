/*
 * Service_other.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: lion
 */

#ifndef _WIN32

#include "object.h"
#include "ifs/Service.h"

namespace fibjs {

result_t Service_base::_new(exlib::string name, v8::Local<v8::Function> worker,
    v8::Local<v8::Object> event, obj_ptr<Service_base>& retVal,
    v8::Local<v8::Object> This)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::install(exlib::string name, exlib::string cmd, exlib::string displayName, exlib::string description)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::remove(exlib::string name)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::start(exlib::string name)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::stop(exlib::string name)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::restart(exlib::string name)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::isInstalled(exlib::string name, bool& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t Service_base::isRunning(exlib::string name, bool& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}
}

#endif