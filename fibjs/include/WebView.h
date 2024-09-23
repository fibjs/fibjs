/*
 * WebView.h
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/WebView.h"
#include "webview/include/webview.h"

namespace fibjs {

class WebView : public WebView_base {
    FIBER_FREE();

public:
    WebView(exlib::string url, NObject* opt);

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadURL(exlib::string url, AsyncEvent* ac);
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t print(int32_t mode, AsyncEvent* ac);
    virtual result_t eval(exlib::string code, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);

public:
    result_t open();
    void config();

public:
    static result_t open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static void run_os_gui();

public:
    void internal_close();
    void release()
    {
        if (!m_closed) {
            m_closed = true;
            webview_destroy(m_webview);
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
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;

private:
    exlib::string m_url;
    obj_ptr<NObject> m_opt;

    webview::webview* m_webview = nullptr;

    bool m_closed = false;
};

}
