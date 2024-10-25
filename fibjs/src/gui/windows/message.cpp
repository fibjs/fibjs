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

result_t gui_base::alert(exlib::string message, exlib::string title, AsyncEvent* ac)
{
    if (ac->isSync()) {
        start_gui();
        return CHECK_ERROR(CALL_E_GUICALL);
    }

    exlib::Event ev;
    async([&message, &title, &ev]() {
        MessageBoxW(NULL, (LPCWSTR)utf8to16String(message).c_str(), (LPCWSTR)utf8to16String(title).c_str(), MB_OK | MB_ICONINFORMATION);
        ev.set();
    },
        CALL_E_LONGSYNC);

    ev.wait();

    return 0;
}

result_t gui_base::confirm(exlib::string message, exlib::string title, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        start_gui();
        return CHECK_ERROR(CALL_E_GUICALL);
    }

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
