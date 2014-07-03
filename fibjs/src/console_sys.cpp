/*
 * console_std.cpp
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#include "console.h"

#ifndef _WIN32

#include <syslog.h>

namespace fibjs
{

void sys_logger::write(item *pn)
{
    item *p1;

    while (pn)
    {
        p1 = pn;
        pn = (logger::item *) pn->m_next;

        ::syslog(p1->m_priority, "%s", p1->m_msg.c_str());
        delete p1;
    }
}

}

#endif
