/*
 * WebView.cpp
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/url.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"
#include <nlohmann/json.hpp>

namespace fibjs {

WebView::WebView(NObject* opt)
    : m_opt(opt)
{
    isolate_ref();
}

result_t WebView::open()
{
    Variant v;
    bool _debug = false;

    if (m_opt->get("debug", v) == 0 && v.boolVal())
        _debug = true;

    m_webview = new webview::webview(_debug, nullptr);
    m_webview->bind("postMessage", [this](std::string arg) -> std::string {
        try {
            nlohmann::json data = nlohmann::json::parse(arg);
            if (!data.is_array())
                return "";
            if (data.size() != 1)
                return "";

            data = data[0];
            if (!data.is_string())
                return "";

            arg = data.get<std::string>();
        } catch (const nlohmann::json::exception&) {
        }

        obj_ptr<EventInfo> ei = new EventInfo(this, "message");
        ei->add("data", arg);
        _emit("message", ei);

        return "";
    });
    m_webview->bind("close", [this](std::string arg) -> std::string {
        Ref();
        asyncCall([](WebView* w) {
            if (!w->m_closed)
                w->internal_close();
            w->Unref();
        },
            this, CALL_E_GUICALL);
        return "";
    });
    m_webview->init("window.__webview__.emitMessage = function(msg) { var e = new Event('message'); e.data = msg; window.dispatchEvent(e); }");

    if (m_opt->get("url", v) == 0)
        m_webview->navigate(v.string());
    else if (m_opt->get("file", v) == 0) {
        obj_ptr<UrlObject_base> u;
        result_t hr = url_base::pathToFileURL(v.string(), u);
        if (hr < 0)
            return hr;

        exlib::string url;
        u->get_href(url);

        m_webview->navigate(url);
    } else
        m_webview->navigate("about:blank");

    config();

    return 0;
}

result_t WebView::loadURL(exlib::string url, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_closed)
        return Runtime::setError("WebView: webview is closed");

    m_webview->navigate(url);

    return 0;
}

result_t WebView::loadFile(exlib::string file, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_closed)
        return Runtime::setError("WebView: webview is closed");

    obj_ptr<UrlObject_base> u;
    result_t hr = url_base::pathToFileURL(file, u);
    if (hr < 0)
        return hr;

    exlib::string url;
    u->get_href(url);

    m_webview->navigate(url);

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_closed)
        return Runtime::setError("WebView: webview is closed");

    m_webview->set_html(html);

    return 0;
}

result_t WebView::eval(exlib::string code, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_closed)
        return Runtime::setError("WebView: webview is closed");

    m_webview->eval(code);

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_closed)
        return Runtime::setError("WebView: webview is closed");

    internal_close();

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    if (m_closed)
        return Runtime::setError("WebView: webview is closed");

    encoding_base::jsstr(msg, false, msg);
    m_webview->eval("window.__webview__.emitMessage('" + msg + "')");

    return 0;
}

}

#endif
