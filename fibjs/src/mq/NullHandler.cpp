/*
 * NullHandler.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "object.h"
#include "NullHandler.h"

namespace fibjs {

result_t NullHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    return CALL_RETURN_UNDEFINED;
}

} /* namespace fibjs */
