
/*
 * dl_webkit.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP) && !defined(_WIN32) && !defined(__APPLE__)

#include <exlib/include/dl_func.h>

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <webkit2/webkit2.h>

static void* webkit_handle = NULL;
#define webkit_func(func) dl_def_func(webkit_handle, "libwebkit2gtk-4.0.so", func)

GtkWidget* webkit_web_view_new()
{
    webkit_func(webkit_web_view_new);
    return s_webkit_web_view_new();
}

GtkWidget* webkit_web_view_new_with_user_content_manager(WebKitUserContentManager* user_content_manager)
{
    webkit_func(webkit_web_view_new_with_user_content_manager);
    return s_webkit_web_view_new_with_user_content_manager(user_content_manager);
}

WebKitUserContentManager* webkit_user_content_manager_new(void)
{
    webkit_func(webkit_user_content_manager_new);
    return s_webkit_user_content_manager_new();
}

WebKitUserScript* webkit_user_script_new(const gchar* source, WebKitUserContentInjectedFrames injected_frames,
    WebKitUserScriptInjectionTime injection_time, const gchar* const* whitelist, const gchar* const* blacklist)
{
    webkit_func(webkit_user_script_new);
    return s_webkit_user_script_new(source, injected_frames, injection_time, whitelist, blacklist);
}

void webkit_user_content_manager_add_script(WebKitUserContentManager* manager, WebKitUserScript* script)
{
    webkit_func(webkit_user_content_manager_add_script);
    s_webkit_user_content_manager_add_script(manager, script);
}

gboolean webkit_user_content_manager_register_script_message_handler(WebKitUserContentManager* manager, const gchar* name)
{
    webkit_func(webkit_user_content_manager_register_script_message_handler);
    return s_webkit_user_content_manager_register_script_message_handler(manager, name);
}

void webkit_web_view_run_javascript(WebKitWebView* web_view, const gchar* script, GCancellable* cancellable,
    GAsyncReadyCallback callback, gpointer user_data)
{
    webkit_func(webkit_web_view_run_javascript);
    s_webkit_web_view_run_javascript(web_view, script, cancellable, callback, user_data);
}

JSCValue* webkit_javascript_result_get_js_value(WebKitJavascriptResult* js_result)
{
    webkit_func(webkit_javascript_result_get_js_value);
    return s_webkit_javascript_result_get_js_value(js_result);
}

void webkit_web_view_load_uri(WebKitWebView* web_view, const gchar* uri)
{
    webkit_func(webkit_web_view_load_uri);
    s_webkit_web_view_load_uri(web_view, uri);
}

void webkit_web_view_load_html(WebKitWebView* web_view, const gchar* content, const gchar* base_uri)
{
    webkit_func(webkit_web_view_load_html);
    s_webkit_web_view_load_html(web_view, content, base_uri);
}

GType webkit_web_view_get_type(void)
{
    webkit_func(webkit_web_view_get_type);
    return s_webkit_web_view_get_type();
}

const gchar* webkit_web_view_get_uri(WebKitWebView* web_view)
{
    webkit_func(webkit_web_view_get_uri);
    return s_webkit_web_view_get_uri(web_view);
}

const gchar* webkit_web_view_get_title(WebKitWebView* web_view)
{
    webkit_func(webkit_web_view_get_title);
    return s_webkit_web_view_get_title(web_view);
}

void webkit_web_view_reload(WebKitWebView* web_view)
{
    webkit_func(webkit_web_view_reload);
    s_webkit_web_view_reload(web_view);
}

void webkit_web_view_go_back(WebKitWebView* web_view)
{
    webkit_func(webkit_web_view_go_back);
    s_webkit_web_view_go_back(web_view);
}

void webkit_web_view_go_forward(WebKitWebView* web_view)
{
    webkit_func(webkit_web_view_go_forward);
    s_webkit_web_view_go_forward(web_view);
}

#endif
