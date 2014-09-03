/*
 * NullHandler.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "NullHandler.h"

namespace fibjs
{

result_t NullHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                             exlib::AsyncEvent *ac)
{
    return CALL_RETURN_NULL;
}

} /* namespace fibjs */
