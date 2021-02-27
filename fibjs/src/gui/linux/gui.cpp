/*
 * gui.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#ifndef _WIN32
#if !defined(__APPLE__) || defined(FIBJS_DISABLE_GUI)

#include "object.h"
#include "ifs/gui.h"
#include "../os_gui.h"

namespace fibjs {

void run_os_gui()
{
    static exlib::Event s_gui;
    s_gui.wait();
}

void os_putGuiPool(AsyncEvent* ac)
{
}

result_t os_gui_setVersion(int32_t ver)
{
    return 0;
}

result_t os_gui_open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}
}
#endif /* __APPLE__ */
#endif /* _WIN32 */
