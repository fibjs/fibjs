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

result_t gui_base::createMenu(v8::Local<v8::Array> items, obj_ptr<Menu_base>& retVal)
{
    return Runtime::setError("Menu not supported in this platform");
}

result_t gui_base::createTray(v8::Local<v8::Object> opt, obj_ptr<Tray_base>& retVal)
{
    return Runtime::setError("Tray not supported in this platform");
}

}

#endif
