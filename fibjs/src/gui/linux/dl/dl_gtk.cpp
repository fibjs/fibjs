
/*
 * dl_gtk.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP) && !defined(_WIN32) && !defined(__APPLE__)

#include <exlib/include/dl_func.h>

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <gtk/gtk.h>

static void* gtk_handle = NULL;
#define gtk_func(func) dl_def_func(gtk_handle, "libgtk-3.so", func)

gboolean gtk_init_check(int* argc, char*** argv)
{
    gtk_func(gtk_init_check);
    return s_gtk_init_check(argc, argv);
}

void gtk_window_set_resizable(GtkWindow* window, gboolean resizable)
{
    gtk_func(gtk_window_set_resizable);
    s_gtk_window_set_resizable(window, resizable);
}

const char* gtk_window_get_title(GtkWindow* window)
{
    gtk_func(gtk_window_get_title);
    return s_gtk_window_get_title(window);
}

void gtk_window_set_title(GtkWindow* window, const char* title)
{
    gtk_func(gtk_window_set_title);
    s_gtk_window_set_title(window, title);
}

void gtk_window_get_position(GtkWindow* window, gint* root_x, gint* root_y)
{
    gtk_func(gtk_window_get_position);
    s_gtk_window_get_position(window, root_x, root_y);
}

void gtk_window_get_size(GtkWindow* window, gint* width, gint* height)
{
    gtk_func(gtk_window_get_size);
    s_gtk_window_get_size(window, width, height);
}

void gtk_container_add(GtkContainer* container, GtkWidget* widget)
{
    gtk_func(gtk_container_add);
    s_gtk_container_add(container, widget);
}

GType gtk_container_get_type(void)
{
    gtk_func(gtk_container_get_type);
    return s_gtk_container_get_type();
}

GType gtk_widget_get_type(void)
{
    gtk_func(gtk_widget_get_type);
    return s_gtk_widget_get_type();
}

GtkWidget* gtk_window_new(GtkWindowType type)
{
    gtk_func(gtk_window_new);
    return s_gtk_window_new(type);
}

void gtk_window_close(GtkWindow* window)
{
    gtk_func(gtk_window_close);
    s_gtk_window_close(window);
}

GType gtk_window_get_type(void)
{
    gtk_func(gtk_window_get_type);
    return s_gtk_window_get_type();
}

void gtk_window_resize(GtkWindow* window, gint width, gint height)
{
    gtk_func(gtk_window_resize);
    s_gtk_window_resize(window, width, height);
}

void gtk_window_move(GtkWindow* window, gint x, gint y)
{
    gtk_func(gtk_window_move);
    s_gtk_window_move(window, x, y);
}

void gtk_window_set_default_size(GtkWindow* window, int width, int height)
{
    gtk_func(gtk_window_set_default_size);
    s_gtk_window_set_default_size(window, width, height);
}

void gtk_window_maximize(GtkWindow* window)
{
    gtk_func(gtk_window_maximize);
    s_gtk_window_maximize(window);
}

void gtk_window_set_decorated(GtkWindow* window, gboolean setting)
{
    gtk_func(gtk_window_set_decorated);
    s_gtk_window_set_decorated(window, setting);
}

void gtk_window_fullscreen(GtkWindow* window)
{
    gtk_func(gtk_window_fullscreen);
    s_gtk_window_fullscreen(window);
}

void gtk_widget_show_all(GtkWidget* widget)
{
    gtk_func(gtk_widget_show_all);
    s_gtk_widget_show_all(widget);
}

GdkDisplay* gdk_display_get_default(void)
{
    gtk_func(gdk_display_get_default);
    return s_gdk_display_get_default();
}

GdkSeat* gdk_display_get_default_seat(GdkDisplay* display)
{
    gtk_func(gdk_display_get_default_seat);
    return s_gdk_display_get_default_seat(display);
}

GdkDevice* gdk_seat_get_pointer(GdkSeat* seat)
{
    gtk_func(gdk_seat_get_pointer);
    return s_gdk_seat_get_pointer(seat);
}

void gdk_device_get_position(GdkDevice* device, GdkScreen** screen, gint* x, gint* y)
{
    gtk_func(gdk_device_get_position);
    s_gdk_device_get_position(device, screen, x, y);
}

void gtk_window_begin_move_drag(GtkWindow* window, gint button, gint root_x, gint root_y, guint32 timestamp)
{
    gtk_func(gtk_window_begin_move_drag);
    s_gtk_window_begin_move_drag(window, button, root_x, root_y, timestamp);
}

GdkScreen* gtk_window_get_screen(GtkWindow* window)
{
    gtk_func(gtk_window_get_screen);
    return s_gtk_window_get_screen(window);
}

gint gdk_screen_get_width(GdkScreen* screen)
{
    gtk_func(gdk_screen_get_width);
    return s_gdk_screen_get_width(screen);
}

gint gdk_screen_get_height(GdkScreen* screen)
{
    gtk_func(gdk_screen_get_height);
    return s_gdk_screen_get_height(screen);
}

GtkWidget* gtk_header_bar_new(void)
{
    gtk_func(gtk_header_bar_new);
    return s_gtk_header_bar_new();
}

void gtk_header_bar_set_show_close_button(GtkHeaderBar* header_bar, gboolean setting)
{
    gtk_func(gtk_header_bar_set_show_close_button);
    s_gtk_header_bar_set_show_close_button(header_bar, setting);
}

GtkWidget* gtk_box_new(GtkOrientation orientation, gint spacing)
{
    gtk_func(gtk_box_new);
    return s_gtk_box_new(orientation, spacing);
}

void gtk_widget_hide(GtkWidget* widget)
{
    gtk_func(gtk_widget_hide);
    s_gtk_widget_hide(widget);
}

void gtk_window_set_titlebar(GtkWindow* window, GtkWidget* titlebar)
{
    gtk_func(gtk_window_set_titlebar);
    s_gtk_window_set_titlebar(window, titlebar);
}

void gtk_widget_destroy(GtkWidget* widget)
{
    gtk_func(gtk_widget_destroy);
    s_gtk_widget_destroy(widget);
}

GMainLoop* g_main_loop_new(GMainContext* context, gboolean is_running)
{
    gtk_func(g_main_loop_new);
    return s_g_main_loop_new(context, is_running);
}

void g_main_loop_run(GMainLoop* loop)
{
    gtk_func(g_main_loop_run);
    s_g_main_loop_run(loop);
}

guint g_idle_add(GSourceFunc function, gpointer data)
{
    gtk_func(g_idle_add);
    return s_g_idle_add(function, data);
}

GInputStream* g_memory_input_stream_new_from_data(const void* data, gssize len, GDestroyNotify destroy)
{
    gtk_func(g_memory_input_stream_new_from_data);
    return s_g_memory_input_stream_new_from_data(data, len, destroy);
}

GQuark g_io_error_quark(void)
{
    gtk_func(g_io_error_quark);
    return s_g_io_error_quark();
}

#endif
