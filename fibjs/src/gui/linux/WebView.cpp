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
#include "Fiber.h"
#include "ifs/gui.h"
#include "ifs/encoding.h"
#include "WebView.h"
#include "EventInfo.h"
#include "StringBuffer.h"

#include <chrono>
#include <stdint.h>
#include <unistd.h>

namespace fibjs {

static int64_t steady_ms()
{
    return (int64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

// X11 applies a geometry request asynchronously: the widget keeps its previous
// allocation until the resulting configure event has been dispatched, and the
// host loop only runs the GLib context between JS turns. A blocking setter
// therefore has to let the loop run before it answers, otherwise the window
// still reports the old rectangle when the call returns.
static void settle_size(void** pwindow, int width, int height)
{
    GtkWindow* window = (GtkWindow*)*pwindow;
    int before_w = 0, before_h = 0;
    gtk_window_get_size(window, &before_w, &before_h);

    if (before_w == width && before_h == height)
        return;

    int w = before_w, h = before_h;
    int64_t start = steady_ms();

    for (;;) {
        // Sleeping the fiber hands the thread back to the GLib loop, which is
        // what dispatches the configure event. Pumping g_main_context_iteration
        // here instead would run the pending JS queue from inside a JS call.
        if (exlib::Thread_base::current())
            exlib::Fiber::sleep(1);
        else
            usleep(1000);

        // The window can be closed while this fiber sleeps.
        window = (GtkWindow*)*pwindow;
        if (!window)
            return;

        gtk_window_get_size(window, &w, &h);
        if (w != before_w || h != before_h)
            break;

        if (steady_ms() - start >= 500)
            break;
    }
}

void WebView::internal_close()
{
    // close() ignores hideOnClose and always destroys the window.
    // gtk_window_close() silently does nothing for a window that was never
    // realized (delete-event is only emitted for realized windows), which used
    // to leave the object open and the window behind, so destroy that case
    // directly.
    GtkWidget* window = GTK_WIDGET(m_window);
    if (gtk_widget_get_realized(window))
        gtk_window_close(GTK_WINDOW(window));
    else
        gtk_widget_destroy(window);
}

void WebView::internal_minimize()
{
    GtkWindow* window = (GtkWindow*)m_window;
    gtk_window_iconify(window);
}

void WebView::internal_maximize()
{
    GtkWindow* window = (GtkWindow*)m_window;

    if (gtk_window_is_maximized(window))
        gtk_window_unmaximize(window);
    else
        gtk_window_maximize(window);
}

void WebView::internal_eval(exlib::string code)
{
    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), code.c_str(), nullptr, nullptr, nullptr);
}

exlib::string WebView::internal_getUrl()
{
    const gchar* uri = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(m_webview));
    return uri ? uri : "";
}

result_t WebView::loadUrl(exlib::string url)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    begin_load();
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_webview), url.c_str());

    return finish_load();
}

result_t WebView::getUrl(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = internal_getUrl();

    return result_t();
}

result_t WebView::setHtml(exlib::string html)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    begin_load();
    webkit_web_view_load_html(WEBKIT_WEB_VIEW(m_webview), html.c_str(), nullptr);

    return finish_load();
}

struct gethtml_callback_data {
public:
    exlib::string& m_retVal;
    exlib::string m_err;
    exlib::Event m_ev;
};

void gethtml_cb(GObject* object, GAsyncResult* result, gpointer user_data)
{
    WebKitJavascriptResult* js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, nullptr);
    gethtml_callback_data* data = static_cast<gethtml_callback_data*>(user_data);

    if (js_result) {
        JSCValue* value = webkit_javascript_result_get_js_value(js_result);
        if (jsc_value_is_string(value)) {
            gchar* str_value = jsc_value_to_string(value);
            data->m_retVal = str_value;
            g_free(str_value);
        }
        webkit_javascript_result_unref(js_result);
    }

    // The completion runs on this thread while the fiber is parked, so the
    // stack references stay valid.
    data->m_ev.set();
}

result_t WebView::getHtml(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    gethtml_callback_data data = { retVal, "", exlib::Event() };

    WebKitWebView* webView = WEBKIT_WEB_VIEW(m_webview);
    webkit_web_view_run_javascript(webView, "document.documentElement.outerHTML.toString()", nullptr, gethtml_cb, &data);

    data.m_ev.wait();

    if (!data.m_err.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(data.m_err);

    return result_t();
}

result_t WebView::reload()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    begin_load();
    webkit_web_view_reload(WEBKIT_WEB_VIEW(m_webview));

    return finish_load();
}

result_t WebView::goBack(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WebKitWebView* webView = WEBKIT_WEB_VIEW(m_webview);

    retVal = webkit_web_view_can_go_back(webView) == TRUE;
    if (!retVal)
        return result_t();

    begin_load();
    webkit_web_view_go_back(webView);

    return finish_load();
}

result_t WebView::goForward(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    WebKitWebView* webView = WEBKIT_WEB_VIEW(m_webview);

    retVal = webkit_web_view_can_go_forward(webView) == TRUE;
    if (!retVal)
        return result_t();

    begin_load();
    webkit_web_view_go_forward(webView);

    return finish_load();
}

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

struct eval_callback_data {
public:
    Variant& m_retVal;
    exlib::string m_err;
    exlib::Event m_ev;
};

void eval_cb(GObject* object, GAsyncResult* result, gpointer user_data)
{
    eval_callback_data* data = (eval_callback_data*)user_data;
    GError* error = NULL;
    WebKitJavascriptResult* js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);

    if (error) {
        // Check if this is the specific "Unsupported result type" error from WebKit (error code 601)
        // This error occurs when JavaScript code returns a Promise, which WebKit cannot serialize
        // We should ignore this error and return undefined, as the Promise result will be handled asynchronously
        if (g_error_matches(error, WEBKIT_JAVASCRIPT_ERROR, 601)) {
            // Ignore Promise return values
            g_clear_error(&error);
            data->m_ev.set();
            return;
        }

        if (!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED) && (!g_error_matches(error, WEBKIT_JAVASCRIPT_ERROR, WEBKIT_JAVASCRIPT_ERROR_SCRIPT_FAILED) || (error->message && *(error->message)))) {
            data->m_err = error->message ? error->message : "script failed";
            g_clear_error(&error);
            data->m_ev.set();
            return;
        }
        g_clear_error(&error);
    }

    if (js_result) {
        JSCValue* value = webkit_javascript_result_get_js_value(js_result);
        JSCException* exception = jsc_context_get_exception(jsc_value_get_context(value));

        if (exception) {
            data->m_err = jsc_exception_get_message(exception);
            jsc_context_clear_exception(jsc_value_get_context(value));
        } else {
            jsc2Variant(value, data->m_retVal);
        }

        webkit_javascript_result_unref(js_result);
    }

    data->m_ev.set();
}

result_t WebView::eval(exlib::string code, Variant& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    eval_callback_data data = { retVal, "", exlib::Event() };

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), code.c_str(), nullptr, eval_cb, &data);

    data.m_ev.wait();

    if (!data.m_err.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_GUI_ERROR").with_message(data.m_err);

    return result_t();
}

result_t WebView::setTitle(exlib::string title)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    gtk_window_set_title(GTK_WINDOW(m_window), title.c_str());

    return result_t();
}

result_t WebView::getTitle(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    const char* title = gtk_window_get_title(GTK_WINDOW(m_window));
    retVal = title ? title : "";

    return result_t();
}

result_t WebView::isVisible(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = gtk_widget_get_visible(GTK_WIDGET(m_window)) != 0;

    return result_t();
}

result_t WebView::show()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (!gtk_widget_get_visible(GTK_WIDGET(m_window)))
        gtk_widget_show_all(GTK_WIDGET(m_window));

    return result_t();
}

result_t WebView::hide()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    if (gtk_widget_get_visible(GTK_WIDGET(m_window)))
        gtk_widget_hide(GTK_WIDGET(m_window));

    return result_t();
}

result_t WebView::setSize(int32_t width, int32_t height)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    gtk_window_resize(GTK_WINDOW(m_window), width, height);
    settle_size(&m_window, width, height);

    return result_t();
}

result_t WebView::getSize(std::vector<double>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    int width, height;
    gtk_window_get_size(GTK_WINDOW(m_window), &width, &height);

    retVal.push_back(width);
    retVal.push_back(height);

    return result_t();
}

result_t WebView::setPosition(int32_t left, int32_t top)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    gtk_window_move(GTK_WINDOW(m_window), left, top);

    return result_t();
}

result_t WebView::getPosition(std::vector<double>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    int left, top;
    gtk_window_get_position(GTK_WINDOW(m_window), &left, &top);

    retVal.push_back(left);
    retVal.push_back(top);

    return result_t();
}

result_t WebView::isActived(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = (gtk_window_is_active(GTK_WINDOW(m_window)) != 0);

    return result_t();
}

result_t WebView::active()
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    gtk_window_present(GTK_WINDOW(m_window));

    return result_t();
}

result_t WebView::get_focused(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = (gtk_window_is_active(GTK_WINDOW(m_window)) != 0);
    return result_t();
}

static exlib::string window_state(GtkWindow* window)
{
    GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    if (gdk_window) {
        GdkWindowState state = gdk_window_get_state(gdk_window);

        if (state & GDK_WINDOW_STATE_FULLSCREEN)
            return "fullscreen";

        if (state & GDK_WINDOW_STATE_ICONIFIED)
            return "minimized";
    }

    if (gtk_window_is_maximized(window))
        return "maximized";

    return "normal";
}

result_t WebView::get_state(exlib::string& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    retVal = window_state(GTK_WINDOW(m_window));
    return result_t();
}

result_t WebView::set_state(exlib::string newVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    GtkWindow* window = GTK_WINDOW(m_window);
    exlib::string cur = window_state(window);

    if (newVal == "normal") {
        if (cur == "fullscreen")
            gtk_window_unfullscreen(window);
        else if (cur == "minimized")
            gtk_window_deiconify(window);
        else if (cur == "maximized")
            gtk_window_unmaximize(window);
    } else if (newVal == "minimized") {
        if (cur != "minimized")
            gtk_window_iconify(window);
    } else if (newVal == "maximized") {
        if (cur == "fullscreen")
            gtk_window_unfullscreen(window);
        if (cur == "minimized")
            gtk_window_deiconify(window);
        if (!gtk_window_is_maximized(window))
            gtk_window_maximize(window);
    } else if (newVal == "fullscreen") {
        if (cur != "fullscreen")
            gtk_window_fullscreen(window);
    } else
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("WebView: state must be 'normal', 'minimized', 'maximized' or 'fullscreen', but got '" + newVal + "'");

    return result_t();
}

result_t WebView::getBounds(obj_ptr<GetBoundsType>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    int left, top, width, height;
    gtk_window_get_position(GTK_WINDOW(m_window), &left, &top);
    gtk_window_get_size(GTK_WINDOW(m_window), &width, &height);

    obj_ptr<GetBoundsType> bounds = new GetBoundsType();
    bounds->x = left;
    bounds->y = top;
    bounds->width = width;
    bounds->height = height;

    retVal = bounds;
    return result_t();
}

result_t WebView::setBounds(WindowBounds* bounds)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    GtkWindow* window = GTK_WINDOW(m_window);

    if (bounds->x.has_value() || bounds->y.has_value()) {
        int left, top;
        gtk_window_get_position(window, &left, &top);

        if (bounds->x.has_value())
            left = (int32_t)bounds->x.value();
        if (bounds->y.has_value())
            top = (int32_t)bounds->y.value();

        gtk_window_move(window, left, top);
    }

    if (bounds->width.has_value() || bounds->height.has_value()) {
        int width, height;
        gtk_window_get_size(window, &width, &height);

        if (bounds->width.has_value())
            width = (int32_t)bounds->width.value();
        if (bounds->height.has_value())
            height = (int32_t)bounds->height.value();

        gtk_window_resize(window, width, height);
        settle_size(&m_window, width, height);
    }

    return result_t();
}

result_t WebView::focus(bool& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    GtkWindow* window = GTK_WINDOW(m_window);
    gtk_window_present(window);

    retVal = (gtk_window_is_active(window) != 0);
    return result_t();
}

struct capture_callback_data {
public:
    StringBuffer m_strs;

    obj_ptr<Buffer_base>& m_retVal;
    exlib::string m_err;
    exlib::string m_message;
    exlib::Event m_ev;
};

void capture_cb(GObject* source_object, GAsyncResult* res, gpointer user_data)
{
    GError* error = NULL;
    cairo_surface_t* surface = webkit_web_view_get_snapshot_finish(WEBKIT_WEB_VIEW(source_object), res, &error);
    capture_callback_data* cb_data = static_cast<capture_callback_data*>(user_data);

    if (error) {
        cb_data->m_err = "ERR_GUI_ERROR";
        cb_data->m_message = error->message ? error->message : "snapshot failed";
        cb_data->m_ev.set();

        g_error_free(error);
        return;
    }

    if (surface) {
        cairo_surface_write_to_png_stream(surface, [](void* closure, const unsigned char* data, unsigned int length) -> cairo_status_t {
            capture_callback_data* cb_data = static_cast<capture_callback_data*>(closure);
            cb_data->m_strs.append((const char*)data, length);
            return CAIRO_STATUS_SUCCESS; }, user_data);

        cairo_surface_destroy(surface);

        cb_data->m_retVal = cb_data->m_strs.buffer();
    } else {
        cb_data->m_err = "ERR_NOT_SUPPORTED";
        cb_data->m_message = "Failed to capture screenshot";
    }

    cb_data->m_ev.set();
}

result_t WebView::takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    capture_callback_data data = { StringBuffer(), retVal, "", "", exlib::Event() };

    WebKitWebView* webView = WEBKIT_WEB_VIEW(m_webview);
    WebKitSnapshotRegion region = fullPage ? WEBKIT_SNAPSHOT_REGION_FULL_DOCUMENT : WEBKIT_SNAPSHOT_REGION_VISIBLE;
    WebKitSnapshotOptions options = WEBKIT_SNAPSHOT_OPTIONS_NONE;

    webkit_web_view_get_snapshot(webView, region, options, NULL, capture_cb, &data);

    data.m_ev.wait();

    if (!data.m_err.empty())
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code(data.m_err).with_message(data.m_message);

    return result_t();
}

result_t WebView::postMessage(exlib::string msg)
{
    result_t hr = ensure_open();
    if (hr.failed())
        return hr;

    encoding_base::jsstr(msg, false, msg);
    exlib::string jsCode = "window.dispatchEvent(new MessageEvent('message', { data: '" + msg + "' }))";

    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), jsCode.c_str(), nullptr, nullptr, nullptr);

    return result_t();
}

}

#endif
