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

namespace fibjs {

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView(NObject* opt)
        : m_opt(opt)
    {
        m_ready = new Event();
        isolate_ref();
    }

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadURL(exlib::string url, AsyncEvent* ac);

    virtual result_t loadFile(exlib::string file, AsyncEvent* ac)
    {
        result_t hr = check_status(ac);
        if (hr < 0)
            return hr;

        obj_ptr<UrlObject_base> u;
        hr = url_base::pathToFileURL(file, u);
        if (hr < 0)
            return hr;

        exlib::string url;
        u->get_href(url);

        return loadURL(url, ac);
    }

    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t eval(exlib::string code, AsyncEvent* ac);
    virtual result_t setTitle(exlib::string title, AsyncEvent* ac);
    virtual result_t getTitle(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);

public:
    result_t open();
    void config();

public:
    static result_t open(NObject* opt, obj_ptr<WebView_base>& retVal);
    static void run_os_gui(exlib::Event& gui_ready);

public:
    void internal_close();

    result_t check_status(AsyncEvent* ac)
    {
        if (ac->isSync()) {
            m_ready->ac_wait();
            return CHECK_ERROR(CALL_E_GUICALL);
        }

        if (!m_window)
            return Runtime::setError("WebView: webview is closed");

        return 0;
    }

    void release()
    {
        if (m_webview) {
            m_webview = nullptr;
            m_window = nullptr;

            _emit("close");

            isolate_unref();
            Unref();
        }
    }

public:
    EVENT_FUNC(open);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(close);
    EVENT_FUNC(message);

public:
    obj_ptr<NObject> m_opt;

    void* m_window = nullptr;
    void* m_webview = nullptr;

    obj_ptr<Event_base> m_ready;

public:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

}
