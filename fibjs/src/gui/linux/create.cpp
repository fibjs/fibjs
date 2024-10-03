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
    obj_ptr<EventInfo> ei = new EventInfo(_webView, "message");
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

static void handle_title_change(WebKitWebView* webview, GParamSpec* pspec, gpointer user_data)
{
    WebView* _webView = (WebView*)user_data;
    const char* title = webkit_web_view_get_title(webview);
    gtk_window_set_title(GTK_WINDOW(_webView->m_window), title);
}

result_t WebView::createWebView()
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

    g_signal_connect(webview, "notify::title", G_CALLBACK(handle_title_change), this);

    gtk_container_add(GTK_CONTAINER(window), webview);

    if (m_options->devtools.has_value() && m_options->devtools.value()) {
        WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(webview));
        webkit_settings_set_enable_developer_extras(settings, TRUE);
    }

    exlib::string url;
    if (m_options->url.has_value())
        url = m_options->url.value();
    else if (m_options->file.has_value()) {
        obj_ptr<UrlObject_base> u;
        result_t hr = url_base::pathToFileURL(m_options->file.value(), u);
        if (hr < 0)
            return hr;

        u->set_protocol("fs:");
        u->set_slashes(true);

        u->get_href(url);
    } else
        url = "about:blank";

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url.c_str());

    config();

    return 0;
}

}

#endif
