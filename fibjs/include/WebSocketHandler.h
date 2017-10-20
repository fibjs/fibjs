/*
 * WebSocketHandler.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "ifs/Handler.h"
#include "Stats.h"

#ifndef WEBSOCKETHANDLER_H_
#define WEBSOCKETHANDLER_H_

namespace fibjs {

class WebSocketHandler : public Handler_base {
    FIBER_FREE();

public:
    WebSocketHandler(v8::Local<v8::Function> accept);

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);
};

} /* namespace fibjs */
#endif /* WEBSOCKETHANDLER_H_ */
