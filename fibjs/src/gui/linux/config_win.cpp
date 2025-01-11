/*
 * gui.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "object.h"
#include "../gui.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/mime.h"
#include "ifs/encoding.h"
#include "EventInfo.h"
#include "WebView.h"
#include "Buffer.h"

extern void dl_init();

namespace fibjs {

static GMainLoop* main_loop = nullptr;

void os_putGuiPool(AsyncEvent* ac)
{
    g_idle_add([](void* _p) -> gboolean {
        AsyncEvent* p = (AsyncEvent*)_p;
        p->invoke();

        return G_SOURCE_REMOVE;
    },
        ac);
}

static void fs_scheme_request_callback(WebKitURISchemeRequest* request, gpointer user_data)
{
    exlib::string fname = webkit_uri_scheme_request_get_path(request);
    encoding_base::decodeURI(fname, fname);

    async([fname, request]() {
        Variant var;
        result_t hr = fs_base::cc_readFile(fname, "", var, Isolate::main());

        if (hr >= 0) {
            async([fname, request, var]() {
                Buffer* _buf = (Buffer*)var.object();
                GInputStream* stream = _buf ? g_memory_input_stream_new_from_data(_buf->data(), _buf->length(), NULL)
                                            : g_memory_input_stream_new_from_data("", 0, NULL);

                exlib::string mtype;
                mime_base::getType(fname, mtype);

                webkit_uri_scheme_request_finish(request, stream, _buf ? _buf->length() : 0, mtype.c_str());
                g_object_unref(stream);
            },
                CALL_E_GUICALL);
        } else {
            async([fname, request]() {
                exlib::string page = "<html><body><h1>Error</h1><p>File not found at path:<br>" + fname + "</p></body></html>";
                GError* error = g_error_new(G_IO_ERROR, G_IO_ERROR_NOT_FOUND, page.c_str());
                webkit_uri_scheme_request_finish_error(request, error);
                g_error_free(error);
            },
                CALL_E_GUICALL);
        }
    });
}

void setup_webkit()
{
    WebKitWebContext* context = webkit_web_context_get_default();

    webkit_web_context_register_uri_scheme(context, "fs", fs_scheme_request_callback, NULL, NULL);

    WebKitCookieManager* cookie_manager = webkit_web_context_get_cookie_manager(context);
    exlib::string cookie_storage_path = g_get_user_cache_dir();
    cookie_storage_path += "/fibjs/cookies";
    webkit_cookie_manager_set_persistent_storage(cookie_manager, cookie_storage_path.c_str(), WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
    webkit_cookie_manager_set_accept_policy(cookie_manager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);
}

void run_os_gui()
{
    dl_init();
    gtk_init_check(nullptr, nullptr);
    main_loop = g_main_loop_new(NULL, FALSE);
    setup_webkit();

    g_gui_ready.set();

    g_main_loop_run(main_loop);
}

static gboolean on_close(GObject* object, GParamSpec* pspec, gpointer data)
{
    WebView* webview = (WebView*)data;

    if (webview->m_options->hideOnClose.value()) {
        if (gtk_widget_get_visible(GTK_WIDGET(webview->m_window))) {
            gtk_widget_hide(GTK_WIDGET(webview->m_window));
        }
        return TRUE;
    }

    webview->release();
    return FALSE;
}

static gboolean on_configure_event(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
    WebView* webview = (WebView*)data;

    int32_t x, y, width, height;
    gtk_window_get_position(GTK_WINDOW(webview->m_window), &x, &y);
    gtk_window_get_size(GTK_WINDOW(webview->m_window), &width, &height);

    // int32_t x = event->x;
    // int32_t y = event->y;
    // int32_t width = event->width;
    // int32_t height = event->height;

    if (x != webview->m_x || y != webview->m_y) {
        webview->m_x = x;
        webview->m_y = y;

        obj_ptr<EventInfo> ei = new EventInfo(webview, "move");
        ei->add("left", x);
        ei->add("top", y);

        ei->emit();
    }

    if (width != webview->m_width || height != webview->m_height) {
        webview->m_width = width;
        webview->m_height = height;

        obj_ptr<EventInfo> ei = new EventInfo(webview, "resize");
        ei->add("width", width);
        ei->add("height", height);

        ei->emit();
    }

    return FALSE;
}

static gboolean on_focus_event(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    if (event->type == GDK_FOCUS_CHANGE) {
        GdkEventFocus* focus_event = (GdkEventFocus*)event;
        if (focus_event->in) {
            WebView* webview = (WebView*)data;
            (new EventInfo(webview, "focus"))->emit();
        }
    }
    return FALSE;
}

static gboolean on_blur_event(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    if (event->type == GDK_FOCUS_CHANGE) {
        GdkEventFocus* focus_event = (GdkEventFocus*)event;
        if (!focus_event->in) {
            WebView* webview = (WebView*)data;
            (new EventInfo(webview, "blur"))->emit();
        }
    }
    return FALSE;
}

#define CW_USEDEFAULT -1
void WebView::config()
{
    GtkWindow* window = (GtkWindow*)m_window;
    int32_t x = CW_USEDEFAULT;
    int32_t y = CW_USEDEFAULT;
    int32_t nWidth = CW_USEDEFAULT;
    int32_t nHeight = CW_USEDEFAULT;

    if (m_options->left.has_value())
        x = m_options->left.value();
    if (m_options->top.has_value())
        y = m_options->top.value();
    if (m_options->width.has_value())
        nWidth = m_options->width.value();
    if (m_options->height.has_value())
        nHeight = m_options->height.value();

    if (!m_options->fullscreen.value()) {
        if (!m_options->frame.value())
            gtk_window_set_decorated(window, FALSE);
        else {
            GtkWidget* titlebar = gtk_header_bar_new();
            gtk_header_bar_set_show_close_button((GtkHeaderBar*)titlebar, TRUE);
            gtk_window_set_titlebar(window, titlebar);

            obj_ptr<TitlebarOptions> titlebar_opt = std::get<obj_ptr<TitlebarOptions>>(m_options->titlebar.value());
            if (titlebar_opt->style.value() != "show") {
                gtk_widget_hide(titlebar);
                gtk_widget_destroy(titlebar);

                GtkCssProvider* provider = gtk_css_provider_new();
                gtk_css_provider_load_from_data(provider, "* { border-radius: 0px; }", -1, NULL);
                GtkStyleContext* context = gtk_widget_get_style_context(GTK_WIDGET(window));
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                g_object_unref(provider);
            }

            if (!m_options->resizable.value())
                gtk_window_set_resizable(window, FALSE);
        }
    }

    GdkScreen* screen = gtk_window_get_screen(window);
    int screen_width = gdk_screen_get_width(screen);
    int screen_height = gdk_screen_get_height(screen);

    if (nWidth == CW_USEDEFAULT)
        nWidth = screen_width * 3 / 4;

    if (nHeight == CW_USEDEFAULT)
        nHeight = screen_height * 3 / 4;

    if (m_options->minWidth.has_value())
        nWidth = std::max(nWidth, m_options->minWidth.value());

    if (m_options->minHeight.has_value())
        nHeight = std::max(nHeight, m_options->minHeight.value());

    if (m_options->maxWidth.has_value())
        nWidth = std::min(nWidth, m_options->maxWidth.value());

    if (m_options->maxHeight.has_value())
        nHeight = std::min(nHeight, m_options->maxHeight.value());

    if (x == CW_USEDEFAULT)
        x = (screen_width - nWidth) / 2;

    if (y == CW_USEDEFAULT)
        y = (screen_height - nHeight) / 2;

    GdkGeometry hints;
    hints.min_width = m_options->minWidth.value();
    hints.min_height = m_options->minHeight.value();
    hints.max_width = m_options->maxWidth.value_or(__INT32_MAX__);
    hints.max_height = m_options->maxHeight.value_or(__INT32_MAX__);
    gtk_window_set_geometry_hints(GTK_WINDOW(window), NULL, &hints, (GdkWindowHints)(GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE));

    gtk_window_move(window, x, y);
    gtk_window_set_default_size(window, nWidth, nHeight);
    if (m_options->fullscreen.value())
        gtk_window_fullscreen(window);
    else if (m_options->maximize.value())
        gtk_window_maximize(window);
    else
        gtk_window_resize(window, nWidth, nHeight);

    if (m_options->menu.has_value()) {
        GtkWidget* menu = (GtkWidget*)m_options->menu.value()->create_os_menu(false);
        GtkWidget* vbox = gtk_bin_get_child(GTK_BIN(window));
        gtk_box_pack_start(GTK_BOX(vbox), menu, FALSE, FALSE, 0);
        gtk_box_reorder_child(GTK_BOX(vbox), menu, 0);
    }

    if (m_icon) {
        GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
        gdk_pixbuf_loader_write(loader, m_icon->data(), m_icon->length(), NULL);
        gdk_pixbuf_loader_close(loader, NULL);
        GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
        if (pixbuf)
            gtk_window_set_icon(GTK_WINDOW(window), pixbuf);
        g_object_unref(loader);
    }

    if (m_options->visible.value())
        gtk_widget_show_all(GTK_WIDGET(window));

    gtk_window_get_position(window, &m_x, &m_y);
    gtk_window_get_size(window, &m_width, &m_height);

    g_signal_connect(window, "delete-event", G_CALLBACK(on_close), this);
    g_signal_connect(window, "configure-event", G_CALLBACK(on_configure_event), this);
    g_signal_connect(window, "focus-in-event", G_CALLBACK(on_focus_event), this);
    g_signal_connect(window, "focus-out-event", G_CALLBACK(on_blur_event), this);

    m_ready->set();
}

}

#endif /* Darwin */
