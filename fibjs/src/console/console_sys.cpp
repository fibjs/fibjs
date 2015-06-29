/*
 * console_sys.cpp
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

        ::syslog(p1->m_priority, "%s", p1->full(false).c_str());

        pn = (logger::item *) p1->m_next;
        delete p1;
    }
}

}

#endif
