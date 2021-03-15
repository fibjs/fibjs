/*
 * Service.h
 *
 *  Created on: Oct 15, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/Service.h"

namespace fibjs {

class Service : public Service_base {
public:
    Service(exlib::string name)
        : m_name(name)
    {
    }

public:
    EVENT_SUPPORT();

public:
    // Service_base
    virtual result_t run(AsyncEvent* ac);
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t set_name(exlib::string newVal);

public:
    EVENT_FUNC(stop);
    EVENT_FUNC(pause);
    EVENT_FUNC(continue);

private:
    exlib::string m_name;
};

inline result_t Service::get_name(exlib::string& retVal)
{
    retVal = m_name;
    return 0;
}

inline result_t Service::set_name(exlib::string newVal)
{
    m_name = newVal;
    return 0;
}

} /* namespace fibjs */
