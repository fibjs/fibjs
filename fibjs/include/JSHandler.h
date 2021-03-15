/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Handler.h"
#include "ifs/vm.h"
#include "Chain.h"
#include "Routing.h"

namespace fibjs {

class JSHandler : public Handler_base {

public:
    JSHandler(v8::Local<v8::Value> proc, bool async = false);

    FIBER_FREE();

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

private:
    bool m_async;
};

} /* namespace fibjs */
