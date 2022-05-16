/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/gui.h"
#include "os_gui.h"

namespace fibjs {

DECLARE_MODULE(gui);

static exlib::Event s_gui;
static exlib::Event s_gui_ready;

void run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    Runtime rt(NULL);

    s_gui_ready.set();
    run_os_gui();

    th.suspend();
}

void putGuiPool(AsyncEvent* ac)
{
    os_putGuiPool(ac);
}

result_t gui_base::setVersion(int32_t ver)
{
    return os_gui_setVersion(ver);
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    Isolate* isolate = Isolate::current();

    s_gui.set();
    s_gui_ready.wait();

    return os_gui_open(url, opt, retVal);
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return open("about:blank", opt, retVal);
}

}
