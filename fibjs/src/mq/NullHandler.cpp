/*
 * NullHandler.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "object.h"
#include "NullHandler.h"

namespace fibjs {

result_t NullHandler::isRouting(bool& retVal)
{
    retVal = false;
    return result_t();
}

result_t NullHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncHandle ac)
{
    return result_t::return_null();
}

} /* namespace fibjs */
