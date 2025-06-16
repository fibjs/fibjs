/*
 * EventSourceHandler.h
 *
 *  Created on: Jun 17, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/Handler.h"

namespace fibjs {

class EventSourceHandler : public Handler_base {
    FIBER_FREE();

public:
    EventSourceHandler(v8::Local<v8::Function> accept);

public:
    // Handler_base
    virtual result_t isRouting(bool& retVal);
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);
};

} /* namespace fibjs */
