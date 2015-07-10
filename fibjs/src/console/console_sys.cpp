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

void sys_logger::write(exlib::List<item> &logs)
{
	item *p1;

	while ((p1 = logs.getHead()) != 0)
	{
		::syslog(p1->m_priority, "%s", p1->full(false).c_str());
		delete p1;
	}
}

}

#endif
