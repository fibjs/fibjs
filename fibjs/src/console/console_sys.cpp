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

result_t sys_logger::write(AsyncEvent *ac)
{
	item *p1;

	while ((p1 = m_workinglogs.getHead()) != 0)
	{
		if (p1->m_priority != console_base::_PRINT)
			::syslog(p1->m_priority, "%s", p1->full(false).c_str());

		delete p1;
	}

	return 0;
}

}

#endif
