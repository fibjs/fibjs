/*
 * WebView.cpp
 *
 *  Created on: Sep 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"

namespace fibjs {

static void handle_script_message(WebKitUserContentManager* manager, WebKitJavascriptResult* js_result,
    gpointer user_data)
{
    JSCValue* value = webkit_javascript_result_get_js_value(js_result);
    gchar* value_str = jsc_value_to_string(value);

    WebView* _webView = (WebView*)user_data;
    fibjs::obj_ptr<fibjs::EventInfo> ei = new fibjs::EventInfo(_webView, "message");
    ei->add("data", (char*)value_str);
    _webView->_emit("message", ei);

    g_free(value_str);
}

static void handle_close(WebKitUserContentManager* manager, WebKitJavascriptResult* js_result,
    gpointer user_data)
{
    WebView* _webView = (WebView*)user_data;
    _webView->internal_close();
}

result_t WebView::open()
{
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    m_window = window;

    WebKitUserContentManager* manager = webkit_user_content_manager_new();

    webkit_user_content_manager_register_script_message_handler(manager, "fibjs");
    g_signal_connect(manager, "script-message-received::fibjs", G_CALLBACK(handle_script_message), this);

    webkit_user_content_manager_register_script_message_handler(manager, "close");
    g_signal_connect(manager, "script-message-received::close", G_CALLBACK(handle_close), this);

    const gchar* custom_js = "window.postMessage = function(message) { window.webkit.messageHandlers.fibjs.postMessage(message); };"
                             "window.close = function() { window.webkit.messageHandlers.close.postMessage(''); };";
    webkit_user_content_manager_add_script(manager, webkit_user_script_new(custom_js, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES, WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, NULL, NULL));

    GtkWidget* webview = webkit_web_view_new_with_user_content_manager(manager);
    m_webview = webview;

    gtk_container_add(GTK_CONTAINER(window), webview);

    exlib::string url;
    Variant v;

    if (m_opt->get("url", v) == 0)
        url = v.string();
    else if (m_opt->get("file", v) == 0) {
        obj_ptr<UrlObject_base> u;
        result_t hr = url_base::pathToFileURL(v.string(), u);
        if (hr < 0)
            return hr;

        u->get_href(url);
    } else
        url = "about:blank";

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url.c_str());

    config();

    return 0;
}

result_t WebView::loadURL(exlib::string url, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_webview), url.c_str());

    return 0;
}

result_t WebView::getUrl(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    const gchar* uri = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(m_webview));
    retVal = uri;
    g_free((gchar*)uri);

    return 0;
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_load_html(WEBKIT_WEB_VIEW(m_webview), html.c_str(), nullptr);

    return 0;
}

result_t WebView::reload(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_reload(WEBKIT_WEB_VIEW(m_webview));

    return 0;
}

result_t WebView::goBack(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_go_back(WEBKIT_WEB_VIEW(m_webview));

    return 0;
}

result_t WebView::goForward(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_go_forward(WEBKIT_WEB_VIEW(m_webview));

    return 0;
}

result_t WebView::eval(exlib::string code, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), code.c_str(), nullptr, nullptr, nullptr);

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    gtk_window_close(GTK_WINDOW(m_window));

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string jsCode = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }))";

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), jsCode.c_str(), nullptr, nullptr, nullptr);

    return 0;
}

}

#endif
