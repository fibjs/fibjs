/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifndef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "gui.h"

namespace fibjs {

DECLARE_MODULE(gui);

// A build without a desktop backend still parses --use-thread, and the mode is
// rejected before the platform check so every platform reports the same
// ERR_GUI_UNAVAILABLE there (the desktop entry points gate on it the same way).
// Without --use-thread the call keeps its historical platform error.
static result_t no_gui_error(const char* message)
{
    result_t gui_hr = check_gui_thread();
    if (gui_hr.failed())
        return gui_hr;

    return result_t::make(CALL_E_INVALID_CALL)
        .with_type_name(errtype::kError)
        .with_code("ERR_NOT_SUPPORTED")
        .with_message(message);
}

static const char* platform_name()
{
#if defined(__APPLE__)
    return "darwin";
#elif defined(_WIN32)
    return "win32";
#else
    return "linux";
#endif
}

void gui_prepare_main_service()
{
}

void gui_prepare_call()
{
}

bool gui_has_pending()
{
    return false;
}

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
    return no_gui_error("Webview not supported in this platform");
}

result_t gui_base::openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return no_gui_error("Webview not supported in this platform");
}

result_t gui_base::open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    return no_gui_error("Webview not supported in this platform");
}

result_t gui_base::createMenu(std::vector<v8::Local<v8::Object>>& items, obj_ptr<Menu_base>& retVal)
{
    return no_gui_error("Menu not supported in this platform");
}

result_t gui_base::createTray(TrayOptions* opt, obj_ptr<Tray_base>& retVal)
{
    return no_gui_error("Tray not supported in this platform");
}

result_t gui_base::alert(exlib::string message)
{
    return no_gui_error("Alert not supported in this platform");
}

result_t gui_base::alert(exlib::string title, exlib::string message)
{
    return no_gui_error("Alert not supported in this platform");
}

result_t gui_base::input(exlib::string message, bool password, exlib::string& retVal)
{
    return no_gui_error("Input not supported in this platform");
}

result_t gui_base::input(exlib::string title, exlib::string message, bool password, exlib::string& retVal)
{
    return no_gui_error("Input not supported in this platform");
}
result_t gui_base::confirm(exlib::string message, bool& retVal)
{
    return no_gui_error("Confirm not supported in this platform");
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal)
{
    return no_gui_error("Confirm not supported in this platform");
}

result_t gui_base::chooseFile(ChooseFileOptions* options, std::vector<exlib::string>& retVal)
{
    return no_gui_error("ChooseFile not supported in this platform");
}

result_t gui_base::get_capabilities(v8::Local<v8::Object>& retVal)
{
    obj_ptr<NObject> window = new NObject();
    window->add("position", false);
    window->add("focusRequest", false);
    window->add("state", false);
    window->add("alwaysOnTop", false);
    window->add("transparency", false);
    window->add("clientFrame", false);

    obj_ptr<NObject> webview = new NObject();
    webview->add("screenshot", false);
    webview->add("devTools", false);

    obj_ptr<NObject> tray = new NObject();
    tray->add("title", false);
    tray->add("tooltip", false);
    tray->add("click", false);

    obj_ptr<NObject> dialog = new NObject();
    dialog->add("parent", false);
    dialog->add("sheet", false);

    obj_ptr<NObject> menu = new NObject();
    menu->add("mutable", false);

    obj_ptr<NObject> caps = new NObject();
    caps->add("platform", exlib::string(platform_name()));

    Variant null_session;
    null_session.setNull();
    caps->add("session", null_session);

    caps->add("window", (object_base*)window);
    caps->add("webview", (object_base*)webview);
    caps->add("tray", (object_base*)tray);
    caps->add("dialog", (object_base*)dialog);
    caps->add("menu", (object_base*)menu);

    caps->valueOf(retVal);
    return result_t();
}

result_t gui_base::quit()
{
    return no_gui_error("gui is not supported in this platform");
}

}

#endif
