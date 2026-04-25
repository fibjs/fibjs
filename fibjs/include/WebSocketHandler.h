/*
 * WebSocketHandler.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#pragma once

#include "ifs/Handler.h"
#include <vector>

namespace fibjs {

class WebSocketHandler : public Handler_base {
    FIBER_FREE();

public:
    WebSocketHandler(v8::Local<v8::Function> accept, bool enableCompress, int32_t maxSize,
        const std::vector<exlib::string>& protocols = std::vector<exlib::string>());

public:
    // Handler_base
    virtual result_t isRouting(bool& retVal);
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    bool m_enableCompress;
    int32_t m_maxSize;
    std::vector<exlib::string> m_protocols;
};

} /* namespace fibjs */
