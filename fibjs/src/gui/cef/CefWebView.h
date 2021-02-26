/*
 * CefWebView.h
 *
 *  Created on: Feb 24, 2021
 *      Author: lion
 */

#include "ifs/WebView.h"
#include "include/cef_client.h"

#ifndef WEBVIEW_CEF_H_
#define WEBVIEW_CEF_H_

namespace fibjs {

class CefWebView : public WebView_base {
public:
    CefWebView(exlib::string url, NObject* opt);
    ~CefWebView();

    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url, AsyncEvent* ac);
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac);
    virtual result_t reload(AsyncEvent* ac);
    virtual result_t goBack(AsyncEvent* ac);
    virtual result_t goForward(AsyncEvent* ac);
    virtual result_t print(int32_t mode, AsyncEvent* ac);
    virtual result_t printToPDF(exlib::string file, AsyncEvent* ac);
    virtual result_t executeJavaScript(exlib::string code, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac);

    void open();
    void config_window();

public:
    EVENT_FUNC(open);
    EVENT_FUNC(load);
    EVENT_FUNC(move);
    EVENT_FUNC(resize);
    EVENT_FUNC(closed);
    EVENT_FUNC(message);

public:
    CefRefPtr<CefBrowser> m_browser;
    obj_ptr<NObject> m_opt;

private:
    exlib::string m_url;
};
}

#endif