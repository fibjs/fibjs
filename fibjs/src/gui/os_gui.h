/*
 * os_gui.h
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifndef OS_GUI_H_
#define OS_GUI_H_

namespace fibjs {
void run_os_gui();
void os_putGuiPool(AsyncEvent* ac);
result_t os_gui_setVersion(int32_t ver);
result_t os_gui_open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
void os_config_window(object_base* webview, void* window, NObject* opt);
}

#endif