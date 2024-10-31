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

result_t gui_base::alert(exlib::string message, AsyncEvent* ac)
{
    return alert("", message, ac);
}

result_t gui_base::alert(exlib::string title, exlib::string message, AsyncEvent* ac)
{
    result_t hr = check_gui(ac);
    if (hr < 0)
        return hr;

    exlib::Event ev;
    async([&message, &title, &ev]() {
        MessageBoxW(NULL, (LPCWSTR)utf8to16String(message).c_str(), (LPCWSTR)utf8to16String(title).c_str(), MB_OK | MB_ICONINFORMATION);
        ev.set();
    },
        CALL_E_LONGSYNC);

    ev.wait();

    return 0;
}

result_t gui_base::confirm(exlib::string message, bool& retVal, AsyncEvent* ac)
{
    return confirm("", message, retVal, ac);
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_gui(ac);
    if (hr < 0)
        return hr;

    exlib::Event ev;
    async([&message, &title, &retVal, &ev]() {
        int result = MessageBoxW(NULL, (LPCWSTR)utf8to16String(message).c_str(), (LPCWSTR)utf8to16String(title).c_str(), MB_OKCANCEL | MB_ICONQUESTION);
        retVal = result == IDOK;
        ev.set();
    },
        CALL_E_LONGSYNC);

    ev.wait();

    return 0;
}

}

#endif
