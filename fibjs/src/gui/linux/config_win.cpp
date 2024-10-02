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

#include "object.h"
#include "ifs/gui.h"
#include "EventInfo.h"
#include "WebView.h"

namespace fibjs {

static GMainLoop* main_loop = nullptr;

void putGuiPool(AsyncEvent* ac)
{
    g_idle_add([](void* _p) -> gboolean {
        AsyncEvent* p = (AsyncEvent*)_p;
        p->invoke();

        return G_SOURCE_REMOVE;
    },
        ac);
}

void WebView::run_os_gui(exlib::Event& gui_ready)
{
    gtk_init_check(nullptr, nullptr);
    main_loop = g_main_loop_new(NULL, FALSE);

    gui_ready.set();

    g_main_loop_run(main_loop);
}

static gboolean on_close(GObject* object, GParamSpec* pspec, gpointer data)
{
    WebView* webview = (WebView*)data;
    webview->release();
    return FALSE;
}

static gboolean on_configure_event(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
    WebView* webview = (WebView*)data;

    int32_t x = event->x;
    int32_t y = event->y;
    int32_t width = event->width;
    int32_t height = event->height;

    if (x != webview->m_x || y != webview->m_y) {
        webview->m_x = x;
        webview->m_y = y;

        obj_ptr<EventInfo> ei = new EventInfo(webview, "move");
        ei->add("left", x);
        ei->add("top", y);

        webview->_emit("move", ei);
    }

    if (width != webview->m_width || height != webview->m_height) {
        webview->m_width = width;
        webview->m_height = height;

        obj_ptr<EventInfo> ei = new EventInfo(webview, "resize");
        ei->add("width", width);
        ei->add("height", height);

        webview->_emit("resize", ei);
    }

    return FALSE;
}

void WebView::internal_close()
{
    GtkWindow* window = (GtkWindow*)m_window;
    gtk_window_close(window);
}

#define CW_USEDEFAULT -1
void WebView::config()
{
    GtkWindow* window = (GtkWindow*)m_window;
    int32_t x = CW_USEDEFAULT;
    int32_t y = CW_USEDEFAULT;
    int32_t nWidth = CW_USEDEFAULT;
    int32_t nHeight = CW_USEDEFAULT;
    bool _maximize = false;
    bool _fullscreen = false;

    if (m_options->left.has_value())
        x = m_options->left.value();
    if (m_options->top.has_value())
        y = m_options->top.value();
    if (m_options->width.has_value())
        nWidth = m_options->width.value();
    if (m_options->height.has_value())
        nHeight = m_options->height.value();

    if (m_options->frame.has_value() && !m_options->frame.value())
        gtk_window_set_decorated(window, FALSE);
    else {
        if (m_options->caption.has_value() && !m_options->caption.value())
            gtk_window_set_titlebar(window, gtk_header_bar_new());

        if (m_options->resizable.has_value() && !m_options->resizable.value())
            gtk_window_set_resizable(window, FALSE);
    }

    if (m_options->maximize.has_value())
        _maximize = m_options->maximize.value();

    if (m_options->fullscreen.has_value())
        _fullscreen = m_options->fullscreen.value();

    GdkScreen* screen = gtk_window_get_screen(window);
    int screen_width = gdk_screen_get_width(screen);
    int screen_height = gdk_screen_get_height(screen);

    if (nWidth == CW_USEDEFAULT)
        nWidth = screen_width * 3 / 4;

    if (nHeight == CW_USEDEFAULT)
        nHeight = screen_height * 3 / 4;

    if (x == CW_USEDEFAULT)
        x = (screen_width - nWidth) / 2;

    if (y == CW_USEDEFAULT)
        y = (screen_height - nHeight) / 2;

    gtk_window_move(window, x, y);
    gtk_window_set_default_size(window, nWidth, nHeight);
    if (_fullscreen)
        gtk_window_fullscreen(window);
    else if (_maximize)
        gtk_window_maximize(window);
    else
        gtk_window_resize(window, nWidth, nHeight);

    gtk_window_get_position(window, &m_x, &m_y);
    gtk_window_get_size(window, &m_width, &m_height);

    gtk_widget_show_all(GTK_WIDGET(window));

    g_signal_connect(window, "delete-event", G_CALLBACK(on_close), this);
    g_signal_connect(window, "configure-event", G_CALLBACK(on_configure_event), this);

    Ref();
    m_ready->set();
    _emit("open");
}

}

#endif /* Darwin */
