/*
 * EventInfo.h
 *
 *  Created on: Mar 23, 2016
 *      Author: lion
 */

#include "SimpleObject.h"

#ifndef EVENTINFO_H_
#define EVENTINFO_H_

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
#endif /* EVENTINFO_H_ */
