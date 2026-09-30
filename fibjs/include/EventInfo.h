/*
 * EventInfo.h
 *
 *  Created on: Mar 23, 2016
 *      Author: lion
 */

#pragma once

#include "SimpleObject.h"
#include "utils.h"

namespace fibjs {

class EventInfo : public NObject {
public:
    EventInfo(obj_ptr<object_base> target, exlib::string type,
        int32_t code = 0, exlib::string reason = "")
        : m_target(target)
        , m_type(type)
    {
        add("target", target);
        add("type", type);

        if (type == "error" && code) {
            ErrorPayload payload = takeErrorPayload();

            if (!payload.code.empty())
                add("code", payload.code);
            else
                add("code", code);

            if (payload.has_errno)
                add("errno", payload.errno_value);
            if (!payload.syscall.empty())
                add("syscall", payload.syscall);
            if (!payload.path.empty())
                add("path", payload.path);
            if (!payload.hostname.empty())
                add("hostname", payload.hostname);

            // Parameter summaries travel with the event just like the scalar
            // fields above: the payload is consumed on the completing thread
            // and the object is converted on the JS thread later on.
            if (!payload.args.empty()) {
                obj_ptr<NObject> args = new NObject();

                for (const ErrorArg& arg : payload.args)
                    args->add(arg.name, arg.value);

                add("args", (obj_base*)args);
            }

            error_payload_delivered(payload);
        } else if (code)
            add("code", code);

        if (!reason.empty())
            add("reason", reason);
    }

    result_t emit()
    {
        return m_target->_emit(m_type, this);
    }

private:
    obj_ptr<object_base> m_target;
    exlib::string m_type;
};

} /* namespace fibjs */
