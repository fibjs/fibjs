/*
 * gui.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "options.h"
#include "gui.h"

#include "WebView.h"
#include "Tray.h"

namespace fibjs {

DECLARE_MODULE(gui);

void os_quit();

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

result_t gui_base::get_capabilities(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    bool position = true;
    bool focus_request = true;
    bool transparency = false;
    const char* session = NULL;

#if defined(__APPLE__)
    session = "quartz";
    transparency = true;
#elif defined(_WIN32)
    session = "win32";
#else
    // The session decides what the window manager honours: placement and focus
    // requests are a compositor matter on Wayland.
    const char* xdg = getenv("XDG_SESSION_TYPE");
    const char* wayland = getenv("WAYLAND_DISPLAY");
    const char* display = getenv("DISPLAY");

    if (xdg && !strcmp(xdg, "wayland")) {
        session = "wayland";
        position = false;
        focus_request = false;
    } else if (xdg && !strcmp(xdg, "x11")) {
        session = "x11";
    } else if (wayland && *wayland) {
        session = "wayland";
        position = false;
        focus_request = false;
    } else if (display && *display) {
        session = "x11";
    } else {
        position = false;
        focus_request = false;
    }
#endif

#if defined(__APPLE__)
    bool tray_title = true, tray_tooltip = true;
#elif defined(_WIN32)
    bool tray_title = false, tray_tooltip = false;
#else
    bool tray_title = true, tray_tooltip = true;
#endif

    obj_ptr<NObject> window = new NObject();
    window->add("position", position);
    window->add("focusRequest", focus_request);
    window->add("state", true);
    window->add("alwaysOnTop", false);
    window->add("transparency", transparency);
    window->add("clientFrame", true);

    obj_ptr<NObject> webview = new NObject();
    webview->add("screenshot", true);
    webview->add("devTools", true);

    obj_ptr<NObject> tray = new NObject();
    tray->add("title", tray_title);
    tray->add("tooltip", tray_tooltip);
    tray->add("click", false);

    obj_ptr<NObject> dialog = new NObject();
    dialog->add("parent", false);
    dialog->add("sheet", false);

    obj_ptr<NObject> menu = new NObject();
    menu->add("mutable", false);

    obj_ptr<NObject> caps = new NObject();
    caps->add("platform", exlib::string(platform_name()));

    if (session) {
        caps->add("session", exlib::string(session));
    } else {
        Variant null_session;
        null_session.setNull();
        caps->add("session", null_session);
    }

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
    result_t hr = check_gui_available();
    if (hr.failed())
        return hr;

    os_quit();
    return result_t();
}

// GUI events are queued in one place and executed by the native host loop.
// The queue is the only entry point, which is what lets the eventual fast path
// decide "nothing is queued, safe to run inline" (gui-main-thread-plan §4.8).
static exlib::LockedList<AsyncEvent> s_gui_pending;

void gui_prepare_main_service()
{
    if (!g_main_js_service)
        g_main_js_service = exlib::Service::createDedicatedUnstarted();
}

void putGuiPool(AsyncEvent* ac)
{
    if (exlib::Service::use_thread) {
        // GUI entry points already reject this mode; this is the last line of
        // defense so that a missed entry never parks a fiber forever.
        ac->post(result_t::make(CALL_E_INVALID_CALL)
                     .with_type_name(errtype::kError)
                     .with_code("ERR_GUI_UNAVAILABLE")
                     .with_message("gui is not available in --use-thread mode; rerun without --use-thread"));
        return;
    }

    s_gui_pending.putTail(ac);
    os_wake_host();
}

void gui_dispatch_pending()
{
    // A nested native loop (for example an inline modal dialog started from the
    // fast path) re-enters the host while JS is still running. The outer drain
    // owns the queue; running GUI events from the JS stack would let them
    // overtake JS-side state.
    if (g_main_js_service && g_main_js_service->is_guest_draining())
        return;

    // Every fiber post wakes the host, so most callbacks have nothing to do.
    // Checking first also keeps the native backend from being initialized on a
    // plain scheduler wakeup (macOS starts AppKit lazily, see
    // ensure_os_gui_initialized).
    if (s_gui_pending.empty())
        return;

    ensure_os_gui_initialized();

    AsyncEvent* ac;
    while ((ac = s_gui_pending.getHead()) != nullptr)
        ac->invoke();
}

bool gui_has_pending()
{
    return !s_gui_pending.empty();
}

void gui_prepare_call()
{
    // A GUI entry point runs inline in the JS fiber (the main isolate lives on
    // the GUI host thread), so it never reaches gui_dispatch_pending() and the
    // lazy backend initialization (AppKit on macOS) has to happen here.
    ensure_os_gui_initialized();
}

// Wake the native loop whenever a fiber is posted to the main isolate.
class HostWake : public exlib::Service::WaitHook {
public:
    virtual void wake(exlib::Service*)
    {
        os_wake_host();
    }
};

static HostWake s_host_wake;

void run_gui(int argc, char* argv[])
{
    if (exlib::Service::use_thread || !g_main_js_service) {
        // Test mode / no host: keep the process alive, GUI is unavailable.
        exlib::OSThread th;
        th.bindCurrent();
        th.suspend();
        return;
    }

    os_prepare_host();
    g_main_js_service->attachCurrentThread();

    // The host-context Runtime (isolate == NULL) is fiber-local, so it has to
    // be registered after the service turned this thread's main context into
    // its m_main fiber.
    Runtime rt(NULL);

    g_main_js_service->setWaitHook(&s_host_wake);

    // First drain: pick up fibers posted before attach (the isolate may have
    // been created while the main thread was still starting). This cannot rely
    // on a wakeup, because posts before setWaitHook() only bumped the unused
    // service semaphore.
    if (g_main_js_service->runPending(64))
        os_wake_host();

    os_run_host();
}

}

#endif
