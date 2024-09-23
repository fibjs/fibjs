/*
 * WebView.cpp
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#ifdef OS_DESKTOP

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"
#include <nlohmann/json.hpp>

namespace fibjs {

WebView::WebView(exlib::string url, NObject* opt)
    : m_url(url)
    , m_opt(opt)
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

    m_webview->navigate(m_url);

    config();

    return 0;
}

result_t WebView::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<NObject> o = new NObject();
    o->add(opt);

    obj_ptr<WebView> w = new WebView(url, o);
    w->wrap();

    w->Ref();
    asyncCall([](WebView* w) {
        w->open();
        w->Unref();
    },
        w, CALL_E_GUICALL);
    retVal = w;

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

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
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

result_t WebView::reload(AsyncEvent* ac)
{
    return 0;
}

result_t WebView::goBack(AsyncEvent* ac)
{
    return 0;
}

result_t WebView::goForward(AsyncEvent* ac)
{
    return 0;
}

result_t WebView::print(int32_t mode, AsyncEvent* ac)
{
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
