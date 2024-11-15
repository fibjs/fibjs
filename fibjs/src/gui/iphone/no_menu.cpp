/*
 * no_menu.cpp
 *
 *  Created on: Nov 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#ifdef iPhone

#include "object.h"
#include "ifs/gui.h"
#include "Menu.h"

namespace fibjs {

void MenuItem::update_os_menu_item()
{
}

void Menu::release_os_menu()
{
}

result_t gui_base::createTray(v8::Local<v8::Object> opt, obj_ptr<Tray_base>& retVal)
{
    return Runtime::setError("Tray not supported in this platform");
}

result_t gui_base::alert(exlib::string message, AsyncEvent* ac)
{
    return Runtime::setError("Alert not supported in this platform");
}

result_t gui_base::alert(exlib::string title, exlib::string message, AsyncEvent* ac)
{
    return Runtime::setError("Alert not supported in this platform");
}

result_t gui_base::confirm(exlib::string message, bool& retVal, AsyncEvent* ac)
{
    return Runtime::setError("Confirm not supported in this platform");
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal, AsyncEvent* ac)
{
    return Runtime::setError("Confirm not supported in this platform");
}

result_t gui_base::chooseFile(v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    return Runtime::setError("ChooseFile not supported in this platform");
}

}

#endif