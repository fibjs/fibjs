/*
 * message.cpp
 *
 *  Created on: Oct 25, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"

namespace fibjs {

result_t gui_base::alert(exlib::string message)
{
    return alert("", message);
}

result_t gui_base::alert(exlib::string title, exlib::string message)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    gui_prepare_call();

    // The modal runs inline on the host (GUI) thread: a nested native loop is
    // allowed there, a fiber-level wait is not (gui-main-thread-plan §4.3).
    MessageBoxW(NULL, (LPCWSTR)utf8to16String(message).c_str(),
        (LPCWSTR)utf8to16String(title).c_str(), MB_OK | MB_ICONINFORMATION);

    return result_t();
}

result_t gui_base::confirm(exlib::string message, bool& retVal)
{
    return confirm("", message, retVal);
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    gui_prepare_call();

    int result = MessageBoxW(NULL, (LPCWSTR)utf8to16String(message).c_str(),
        (LPCWSTR)utf8to16String(title).c_str(), MB_OKCANCEL | MB_ICONQUESTION);
    retVal = result == IDOK;

    return result_t();
}

}

#endif
