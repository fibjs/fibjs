/*
 * WebView.h
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/WebView.h"
#include "ifs/url.h"
#include "Event.h"
#include "Buffer.h"
#include "Menu.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

class WebView : public WebView_base {
public:
    class TitlebarOptions : public obj_base {
    public:
        LOAD_OPTIONS(TitlebarOptions, (style)(height));

    public:
        std::optional<exlib::string> style = "show";
        std::optional<exlib::string> height = "normal";
    };

    class OpenOptions : public obj_base {
    public:
        LOAD_OPTIONS(OpenOptions, (url)(file)(icon)(left)(top)(width)(height)(visible)(hideOnClose)(minWidth)(minHeight)(maxWidth)(maxHeight)(frame)(titlebar)(resizable)(fullscreen)(maximize)(menu)(app)(devtools)(onloading)(onload)(onclose)(onmove)(onresize)(onfocus)(onblur)(onmessage));

    public:
        std::optional<exlib::string> url;
        std::optional<exlib::string> file;
        std::optional<exlib::string> icon;

        std::optional<int32_t> left;
        std::optional<int32_t> top;
        std::optional<int32_t> width;
        std::optional<int32_t> height;

        std::optional<bool> visible = true;
        std::optional<bool> hideOnClose = false;

        std::optional<int32_t> minWidth = 0;
        std::optional<int32_t> minHeight = 0;
        std::optional<int32_t> maxWidth;
        std::optional<int32_t> maxHeight;

        std::optional<bool> frame = true;
        std::optional<std::variant<exlib::string, obj_ptr<TitlebarOptions>>> titlebar = "show";
        std::optional<bool> resizable = true;
        std::optional<bool> fullscreen = false;
        std::optional<bool> maximize = false;

        std::optional<obj_ptr<Menu>> menu;
        std::optional<v8::Local<v8::Object>> app;

        std::optional<bool> devtools = false;

        std::optional<v8::Local<v8::Function>> onloading;
        std::optional<v8::Local<v8::Function>> onload;
        std::optional<v8::Local<v8::Function>> onclose;
        std::optional<v8::Local<v8::Function>> onmove;
        std::optional<v8::Local<v8::Function>> onresize;
        std::optional<v8::Local<v8::Function>> onfocus;
        std::optional<v8::Local<v8::Function>> onblur;
        std::optional<v8::Local<v8::Function>> onmessage;
    };

public:
    WebView()
    {
        m_ready = new Event();
        m_load = new Event();
        m_closed_ev = new Event();
    }

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url);
    virtual result_t loadFile(exlib::string file);
    virtual result_t getUrl(exlib::string& retVal);
    virtual result_t setHtml(exlib::string html);
    virtual result_t getHtml(exlib::string& retVal);
    virtual result_t get_loading(bool& retVal);
    virtual result_t waitForLoad(exlib::string url);
    virtual result_t reload();
    virtual result_t goBack(bool& retVal);
    virtual result_t goForward(bool& retVal);
    virtual result_t eval(exlib::string code, Variant& retVal);
    virtual result_t setTitle(exlib::string title);
    virtual result_t getTitle(exlib::string& retVal);
    virtual result_t isVisible(bool& retVal);
    virtual result_t show();
    virtual result_t hide();
    virtual result_t setSize(int32_t width, int32_t height);
    virtual result_t getSize(std::vector<double>& retVal);
    virtual result_t setPosition(int32_t left, int32_t top);
    virtual result_t getPosition(std::vector<double>& retVal);
    virtual result_t isActived(bool& retVal);
    virtual result_t active();
    virtual result_t get_focused(bool& retVal);
    virtual result_t get_state(exlib::string& retVal);
    virtual result_t set_state(exlib::string newVal);
    virtual result_t getBounds(obj_ptr<GetBoundsType>& retVal);
    virtual result_t setBounds(WindowBounds* bounds);
    virtual result_t focus(bool& retVal);
    virtual result_t waitForClose();
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal);
    virtual result_t takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal);
    virtual result_t close(bool& retVal);
    virtual result_t destroy();
    virtual result_t postMessage(exlib::string msg);
    virtual result_t ref(obj_ptr<WebView_base>& retVal);
    virtual result_t unref(obj_ptr<WebView_base>& retVal);

public:
    // Navigation bookkeeping. A navigation clears the event when it starts and
    // sets it when it finishes, so a blocking member issues its command and
    // then waits: the page is loaded (or failed) when the call returns.
    void postLoading(exlib::string url);
    void postLoad(exlib::string url, result_t hr = result_t(), exlib::string message = "");
    void begin_load();
    result_t finish_load();

    // True while a fiber is parked for the next completion. WebKitGTK follows a
    // failed navigation with an error page it loads itself; when the caller is
    // waiting for that navigation, the failure is the answer and the engine
    // must not replace it with the error page.
    bool handles_load_failure() const
    {
        return m_waiting && !m_load_done;
    }

    // The window must exist: every member either blocks until it does (open)
    // or reports that it is gone.
    result_t ensure_open();

    // Asks the close listeners whether the window may close. Runs on the JS
    // thread: inline when the caller is JS, posted when the platform asked
    // (window-manager button), in which case the close is cancelled now and
    // retried after the listeners returned.
    bool should_cancel_close();
    bool emit_close_requested();

    // Blocks until the native window is gone; returns at once when it already
    // is. Called by close/destroy/waitForClose.
    void wait_close();

    // Marks the window as gone and wakes every waiter. Called once by the
    // platform when the native window was destroyed.
    void postClose();

public:
    result_t createWebView();
    void config();

public:
    result_t setup(v8::Local<v8::Object> opt);
    result_t open(exlib::string url, v8::Local<v8::Object> opt);
    result_t open(v8::Local<v8::Object> opt);
    result_t openFile(exlib::string file, v8::Local<v8::Object> opt);
    result_t async_open();

    void app_rpc(exlib::string json);

public:
    void internal_close();
    void internal_minimize();
    void internal_maximize();
    void internal_eval(exlib::string code);
    exlib::string internal_getUrl();

    void release();

public:
    obj_ptr<OpenOptions> m_options;

    obj_ptr<ValueHolder> m_self;
    obj_ptr<ValueHolder> m_menu;

    obj_ptr<Buffer> m_icon;

    void* m_window = nullptr;
    void* m_webview = nullptr;

    obj_ptr<Event_base> m_ready;
    obj_ptr<Event_base> m_load;
    obj_ptr<Event_base> m_closed_ev;

    exlib::string m_load_url;
    exlib::string m_loading_url;
    result_t m_load_hr;

    // One navigation reports exactly one outcome: WebKitGTK emits load-failed
    // for a connection error and then finishes the error page, which would
    // otherwise overwrite the failure with a success.
    bool m_load_done = false;

    // A fiber is parked in finish_load()/waitForLoad(); the reported outcome
    // stays untouched until it wakes, so a follow-up navigation started by the
    // engine cannot rewrite the answer it is about to read.
    bool m_waiting = false;

    // Navigation generations: begin_load() expects the next navigation, so a
    // completion belonging to an older one (the about:blank of a fresh window,
    // for example) cannot satisfy the wait.
    int32_t m_load_gen = 0;
    int32_t m_wait_gen = 0;

    bool m_loading = false;
    bool m_closed = false;
    bool m_allowClose = false;

#ifdef _WIN32
    bool m_fullscreen = false;
    // WINDOWPLACEMENT*, allocated when the window first goes fullscreen and
    // released when it leaves it; kept as void* so this header does not depend
    // on <windows.h>.
    void* m_placement = nullptr;
#endif

public:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

}
