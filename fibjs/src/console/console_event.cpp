/*
 * console_event.cpp
 *
 *  Created on: Sep 13, 2016
 *      Author: lion
 */

#include "console.h"

#ifdef _WIN32

namespace fibjs {

event_logger::event_logger()
{
    m_event = RegisterEventSource(NULL, "fibjs");
}

event_logger::~event_logger()
{
    DeregisterEventSource(m_event);
}

result_t event_logger::write(AsyncEvent* ac)
{
    static WORD s_levels[] = {
        EVENTLOG_ERROR_TYPE,
        EVENTLOG_ERROR_TYPE,
        EVENTLOG_ERROR_TYPE,
        EVENTLOG_ERROR_TYPE,
        EVENTLOG_WARNING_TYPE,
        EVENTLOG_WARNING_TYPE,
        EVENTLOG_INFORMATION_TYPE,
        EVENTLOG_INFORMATION_TYPE,
        EVENTLOG_INFORMATION_TYPE,
        EVENTLOG_INFORMATION_TYPE,
        EVENTLOG_INFORMATION_TYPE
    };
    item* p1;

    while ((p1 = m_workinglogs.getHead()) != 0) {
        if (p1->m_priority != console_base::_PRINT) {
            exlib::string str = p1->full(false);
            const char* ptr = str.c_str();
            ReportEvent(m_event, s_levels[p1->m_priority], 0, 0,
                NULL, 1, 0, &ptr, NULL);
        }

        delete p1;
    }

    return 0;
}
}

#endif
