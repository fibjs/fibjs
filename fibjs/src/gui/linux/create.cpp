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
#include <JavaScriptCore/JavaScript.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"

namespace fibjs {

static char* get_string_from_js_result(WebKitJavascriptResult* r)
{
    char* s;

    // JSCValue* value = webkit_javascript_result_get_js_value(r);
    // s = jsc_value_to_string(value);

    JSGlobalContextRef ctx = webkit_javascript_result_get_global_context(r);
    JSValueRef value = webkit_javascript_result_get_value(r);
    JSStringRef js = JSValueToStringCopy(ctx, value, nullptr);
    size_t n = JSStringGetMaximumUTF8CStringSize(js);
    s = g_new(char, n);
    JSStringGetUTF8CString(js, s, n);
    JSStringRelease(js);

    return s;
}

static void handle_message(WebKitUserContentManager* manager, WebKitJavascriptResult* js_result,
    gpointer user_data)
{
    gchar* value_str = get_string_from_js_result(js_result);

    WebView* _webView = (WebView*)user_data;
    obj_ptr<EventInfo> ei = new EventInfo(_webView, "message");
    ei->add("data", (char*)value_str);
    _webView->_emit("message", ei);

    g_free(value_str);
}

static void handle_command(WebKitUserContentManager* manager, WebKitJavascriptResult* js_result,
    gpointer user_data)
{
    gchar* value_str = get_string_from_js_result(js_result);

    if (strcmp(value_str, "close") == 0) {
        WebView* _webView = (WebView*)user_data;
        _webView->internal_close();
    } else if (strcmp(value_str, "minimize") == 0) {
        WebView* _webView = (WebView*)user_data;
        _webView->internal_minimize();
    } else if (strcmp(value_str, "maximize") == 0) {
        WebView* _webView = (WebView*)user_data;
        _webView->internal_maximize();
    } else if (strcmp(value_str, "drag") == 0) {
        GdkDisplay* display = gdk_display_get_default();
        GdkSeat* seat = gdk_display_get_default_seat(display);
        GdkDevice* device = gdk_seat_get_pointer(seat);

        gint x, y;
        gdk_device_get_position(device, NULL, &x, &y);

        WebView* _webView = (WebView*)user_data;
        gtk_window_begin_move_drag(GTK_WINDOW(_webView->m_window), 1, x, y, 0);
    }

    g_free(value_str);
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

    webkit_user_content_manager_register_script_message_handler(manager, "message");
    g_signal_connect(manager, "script-message-received::message", G_CALLBACK(handle_message), this);

    webkit_user_content_manager_register_script_message_handler(manager, "command");
    g_signal_connect(manager, "script-message-received::command", G_CALLBACK(handle_command), this);

    const gchar* custom_js = "window.postMessage = function(message) { window.webkit.messageHandlers.message.postMessage(message); };"
                             "window.close = function() { window.webkit.messageHandlers.command.postMessage('close'); };"
                             "window.minimize = function() { window.webkit.messageHandlers.command.postMessage('minimize'); };"
                             "window.maximize = function() { window.webkit.messageHandlers.command.postMessage('maximize'); };"
                             "window.drag = function() { window.webkit.messageHandlers.command.postMessage('drag'); };";
    webkit_user_content_manager_add_script(manager, webkit_user_script_new(custom_js, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES, WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, NULL, NULL));

    GtkWidget* webview = webkit_web_view_new_with_user_content_manager(manager);
    m_webview = webview;

    g_signal_connect(webview, "notify::title", G_CALLBACK(handle_title_change), this);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), webview, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    if (m_options->devtools.value()) {
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
