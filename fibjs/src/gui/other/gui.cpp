/*
 * gui.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#ifndef _WIN32
#ifndef __APPLE__

#include "object.h"

namespace fibjs {

void run_gui()
{
    exlib::OSThread th;

    th.bindCurrent();
    th.suspend();
}

void putGuiPool(AsyncEvent* ac)
{
}
}
#endif /* __APPLE__ */
#endif /* _WIN32 */
