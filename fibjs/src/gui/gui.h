/*
 * gui.h
 *
 *  Created on: Oct 25, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/DialogFilter.h"

#include <stdlib.h>

namespace fibjs {

// Native host loop hooks. os_prepare_host() must create the wakeup source used
// by os_wake_host() before the main JS service is attached; the caller then
// enters os_run_host() (normally never returns). Backend-dependent resources
// (AppKit on macOS) are created lazily by ensure_os_gui_initialized(), i.e. on
// the first GUI operation or when the loop starts, so a native addon can claim
// the platform application object first and a GUI-less script never pays for it.
void os_prepare_host();
void os_wake_host();
void os_run_host();
void ensure_os_gui_initialized();

// Drain the platform-independent GUI event queue on the host thread.
void gui_dispatch_pending();

// True when a GUI event is queued and waiting for the host loop. Host loops use
// it to decide whether the platform toolkit is actually needed (macOS starts
// AppKit lazily so a native addon can own NSApplication first).
bool gui_has_pending();

void putGuiPool(AsyncEvent* ac);

// Whether the process has a display session to draw in. On Linux a missing
// DISPLAY/WAYLAND_DISPLAY means GTK cannot connect, and the same condition is
// reported as an absent `session` by gui.capabilities.
inline bool has_gui_session()
{
#if defined(Linux) && defined(OS_DESKTOP)
    const char* wayland = getenv("WAYLAND_DISPLAY");
    const char* display = getenv("DISPLAY");

    return (wayland && *wayland) || (display && *display);
#else
    return true;
#endif
}

// --use-thread has no single GUI thread: every JS fiber is its own OS thread
// there, so there is no thread that could own the native objects. Entry points
// that only build data (a menu template) need nothing else.
inline result_t check_gui_thread()
{
    if (!exlib::Service::use_thread)
        return result_t();

    return result_t::make(CALL_E_INVALID_CALL)
        .with_type_name(errtype::kError)
        .with_code("ERR_GUI_UNAVAILABLE")
        .with_message("gui is not available in --use-thread mode; rerun without --use-thread");
}

// Windows, tray icons and dialogs also need a desktop to draw in. The check runs
// before any toolkit library is loaded, so a CLI or server process on a headless
// machine never touches GTK.
inline result_t check_gui_available()
{
    result_t hr = check_gui_thread();
    if (hr.failed() || has_gui_session())
        return hr;

    return result_t::make(CALL_E_INVALID_CALL)
        .with_type_name(errtype::kError)
        .with_code("ERR_GUI_UNAVAILABLE")
        .with_message("gui is not available: no display server (DISPLAY and WAYLAND_DISPLAY are both unset)");
}

} // namespace fibjs
