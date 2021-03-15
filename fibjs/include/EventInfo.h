/*
 * EventInfo.h
 *
 *  Created on: Mar 23, 2016
 *      Author: lion
 */

#pragma once

#include "SimpleObject.h"

namespace fibjs {

class EventInfo : public NObject {
public:
    EventInfo(obj_ptr<object_base> target, exlib::string type,
        int32_t code = 0, exlib::string reason = "")
    {
        add("target", target);
        add("type", type);

        if (code)
            add("code", code);

        if (!reason.empty())
            add("reason", reason);
    }
};

} /* namespace fibjs */
