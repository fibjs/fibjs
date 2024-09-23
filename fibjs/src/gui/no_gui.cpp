/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifndef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
namespace fibjs {

DECLARE_MODULE(gui);

static exlib::Event s_gui;
static exlib::Event s_gui_ready;

void run_gui(int argc, char* argv[])
{
    exlib::OSThread th;
    th.bindCurrent();

    th.suspend();
}

void putGuiPool(AsyncEvent* ac)
{
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return Runtime::setError("Webview not supported in this platform");
}

result_t gui_base::openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return Runtime::setError("Webview not supported in this platform");
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return Runtime::setError("Webview not supported in this platform");
}

}

#endif
