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
#include <jsc/jsc.h>

#include "object.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"

namespace fibjs {

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

struct eval_callback_data {
public:
    eval_callback_data(Variant& retVal, AsyncEvent* ac)
        : m_retVal(retVal)
        , m_ac(ac)
    {
    }

    Variant& m_retVal;
    AsyncEvent* m_ac;
};

void jsc2Variant(JSCValue* value, Variant& retVal)
{
    if (jsc_value_is_boolean(value)) {
        retVal = jsc_value_to_boolean(value) ? true : false;
    } else if (jsc_value_is_number(value)) {
        retVal = jsc_value_to_double(value);
    } else if (jsc_value_is_string(value)) {
        retVal = jsc_value_to_string(value);
    } else if (jsc_value_is_null(value)) {
        retVal.setNull();
    } else if (jsc_value_is_array(value)) {
        JSCValue* length = jsc_value_object_get_property(value, "length");
        int len = jsc_value_to_int32(length);
        obj_ptr<NArray> array = new NArray();
        for (int i = 0; i < len; i++) {
            JSCValue* item = jsc_value_object_get_property_at_index(value, i);
            Variant v;
            jsc2Variant(item, v);
            array->append(v);
        }
        retVal = array;
    } else if (jsc_value_is_object(value)) {
        obj_ptr<NObject> obj = new NObject();
        char** keys = jsc_value_object_enumerate_properties(value);
        if (keys) {
            for (int i = 0; keys[i]; i++) {
                JSCValue* item = jsc_value_object_get_property(value, keys[i]);
                Variant v;
                jsc2Variant(item, v);
                obj->add(keys[i], v);
            }
        }
        retVal = obj;
    }
}

void eval_cb(GObject* object, GAsyncResult* result, gpointer user_data)
{
    eval_callback_data* data = (eval_callback_data*)user_data;
    GError* error = NULL;
    WebKitJavascriptResult* js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);
    if (error) {
        if (!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED) && (!g_error_matches(error, WEBKIT_JAVASCRIPT_ERROR, WEBKIT_JAVASCRIPT_ERROR_SCRIPT_FAILED) || (error->message && *(error->message)))) {
            data->m_ac->post(Runtime::setError(error->message));
            g_clear_error(&error);
            delete data;
            return;
        }
    }

    if (js_result) {
        JSCValue* value = webkit_javascript_result_get_js_value(js_result);
        JSCException* exception = jsc_context_get_exception(jsc_value_get_context(value));

        if (exception) {
            data->m_ac->post(Runtime::setError(jsc_exception_get_message(exception)));
            jsc_context_clear_exception(jsc_value_get_context(value));
        } else {
            jsc2Variant(value, data->m_retVal);
            data->m_ac->post(0);
        }

        webkit_javascript_result_unref(js_result);
    } else
        data->m_ac->post(0);

    delete data;
}

result_t WebView::eval(exlib::string code, Variant& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), code.c_str(), nullptr, eval_cb, new eval_callback_data(retVal, ac));
    return CALL_E_PENDDING;
}

result_t WebView::setTitle(exlib::string title, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    gtk_window_set_title(GTK_WINDOW(m_window), title.c_str());

    return 0;
}

result_t WebView::getTitle(exlib::string& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    const char* title = gtk_window_get_title(GTK_WINDOW(m_window));
    if (title) {
        retVal = title;
    } else {
        retVal = "";
    }

    return 0;
}

result_t WebView::setSize(int32_t width, int32_t height, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    gtk_window_resize(GTK_WINDOW(m_window), width, height);

    return 0;
}

result_t WebView::getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    int width, height;
    gtk_window_get_size(GTK_WINDOW(m_window), &width, &height);

    retVal = new NArray();
    retVal->append(width);
    retVal->append(height);

    return 0;
}

result_t WebView::setPosition(int32_t left, int32_t top, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    gtk_window_move(GTK_WINDOW(m_window), left, top);

    return 0;
}

result_t WebView::getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    int left, top;
    gtk_window_get_position(GTK_WINDOW(m_window), &left, &top);

    retVal = new NArray();
    retVal->append(left);
    retVal->append(top);

    return 0;
}

result_t WebView::isActived(bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    GtkWindow* window = GTK_WINDOW(m_window);
    retVal = (gtk_window_is_active(window) != 0);

    return 0;
}

result_t WebView::active(AsyncEvent* ac)
{
    result_t hr = check_status(ac);
    if (hr < 0)
        return hr;

    GtkWindow* window = GTK_WINDOW(m_window);
    gtk_window_present(window);

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
