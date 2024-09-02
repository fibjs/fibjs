/*
 * AbortController.h
 *
 *  Created on: Jul 13, 2024
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "ifs/AbortController.h"

namespace fibjs {

class AbortSignal : public AbortSignal_base {
    EVENT_SUPPORT();

public:
    // AbortSignal_base
    virtual result_t abort(exlib::string reason);
    virtual result_t get_aborted(bool& retVal);

public:
    EVENT_FUNC(abort);

public:
    // object_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        if (ev == "abort")
            m_aborted = true;
        return 0;
    }

private:
    std::atomic_bool m_aborted = false;
};

class AbortController : public AbortController_base {
public:
    AbortController()
    {
        m_signal = new AbortSignal();
    }

public:
    // AbortController_base
    virtual result_t get_signal(obj_ptr<AbortSignal_base>& retVal);
    virtual result_t abort(exlib::string reason);

private:
    obj_ptr<AbortSignal> m_signal;
};

}
