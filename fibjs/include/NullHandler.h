/*
 * NullHandler.h
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Handler.h"

namespace fibjs {

class NullHandler : public Handler_base {
    FIBER_FREE();

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal, AsyncEvent* ac);
};

} /* namespace fibjs */
