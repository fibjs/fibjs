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
    return result_t::make(CALL_E_EXCEPTION).with_message("Webview not supported in this platform");
}

result_t gui_base::openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Webview not supported in this platform");
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Webview not supported in this platform");
}

result_t gui_base::createMenu(std::vector<v8::Local<v8::Object>>& items, obj_ptr<Menu_base>& retVal)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Menu not supported in this platform");
}

result_t gui_base::createTray(v8::Local<v8::Object> opt, obj_ptr<Tray_base>& retVal)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Tray not supported in this platform");
}

result_t gui_base::alert(exlib::string message, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Alert not supported in this platform");
}

result_t gui_base::alert(exlib::string title, exlib::string message, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Alert not supported in this platform");
}

result_t gui_base::input(exlib::string message, bool password, exlib::string& retVal, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Input not supported in this platform");
}

result_t gui_base::input(exlib::string title, exlib::string message, bool password, exlib::string& retVal, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Input not supported in this platform");
}
result_t gui_base::confirm(exlib::string message, bool& retVal, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Confirm not supported in this platform");
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("Confirm not supported in this platform");
}

result_t gui_base::chooseFile(v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    return result_t::make(CALL_E_EXCEPTION).with_message("ChooseFile not supported in this platform");
}

}

#endif
