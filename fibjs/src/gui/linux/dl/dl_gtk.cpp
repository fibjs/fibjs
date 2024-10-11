
/*
 * dl_gtk.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include <exlib/include/dl_func.h>
#include "ldconfig.h"

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <gtk/gtk.h>

const char* gtk_str()
{
    static std::string str = ldconfig("gtk-3");
    return str.c_str();
}

static void* gtk_handle = NULL;
#define gtk_func(func) dl_def_func(gtk_handle, func, gtk_str())

void dl_gtk_init()
{
}

gboolean gtk_init_check(int* argc, char*** argv)
{
    gtk_func(gtk_init_check);
    return s_gtk_init_check(argc, argv);
}

void gtk_window_set_icon(GtkWindow* window, GdkPixbuf* icon)
{
    gtk_func(gtk_window_set_icon);
    s_gtk_window_set_icon(window, icon);
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

void gtk_window_iconify(GtkWindow* window)
{
    gtk_func(gtk_window_iconify);
    s_gtk_window_iconify(window);
}

void gtk_window_maximize(GtkWindow* window)
{
    gtk_func(gtk_window_maximize);
    s_gtk_window_maximize(window);
}

void gtk_window_unmaximize(GtkWindow* window)
{
    gtk_func(gtk_window_unmaximize);
    s_gtk_window_unmaximize(window);
}

gboolean gtk_window_is_maximized(GtkWindow* window)
{
    gtk_func(gtk_window_is_maximized);
    return s_gtk_window_is_maximized(window);
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

GtkCssProvider* gtk_css_provider_new(void)
{
    gtk_func(gtk_css_provider_new);
    return s_gtk_css_provider_new();
}

gboolean gtk_css_provider_load_from_data(GtkCssProvider* provider, const gchar* data, gssize length, GError** error)
{
    gtk_func(gtk_css_provider_load_from_data);
    return s_gtk_css_provider_load_from_data(provider, data, length, error);
}

GType gtk_style_provider_get_type(void)
{
    gtk_func(gtk_style_provider_get_type);
    return s_gtk_style_provider_get_type();
}

void gtk_style_context_add_provider(GtkStyleContext* context, GtkStyleProvider* provider, guint priority)
{
    gtk_func(gtk_style_context_add_provider);
    s_gtk_style_context_add_provider(context, provider, priority);
}

GtkStyleContext* gtk_widget_get_style_context(GtkWidget* widget)
{
    gtk_func(gtk_widget_get_style_context);
    return s_gtk_widget_get_style_context(widget);
}

GType gtk_box_get_type(void)
{
    gtk_func(gtk_box_get_type);
    return s_gtk_box_get_type();
}

void g_list_free(GList* list)
{
    gtk_func(g_list_free);
    s_g_list_free(list);
}

#undef g_object_ref
gpointer g_object_ref(gpointer object)
{
    gtk_func(g_object_ref);
    return s_g_object_ref(object);
}

gboolean g_type_check_instance_is_a(GTypeInstance* instance, GType iface_type)
{
    gtk_func(g_type_check_instance_is_a);
    return s_g_type_check_instance_is_a(instance, iface_type);
}

int gdk_pixbuf_get_height(const GdkPixbuf* pixbuf)
{
    gtk_func(gdk_pixbuf_get_height);
    return s_gdk_pixbuf_get_height(pixbuf);
}

int gdk_pixbuf_get_width(const GdkPixbuf* pixbuf)
{
    gtk_func(gdk_pixbuf_get_width);
    return s_gdk_pixbuf_get_width(pixbuf);
}

gboolean gdk_pixbuf_loader_close(GdkPixbufLoader* loader, GError** error)
{
    gtk_func(gdk_pixbuf_loader_close);
    return s_gdk_pixbuf_loader_close(loader, error);
}

GdkPixbuf* gdk_pixbuf_loader_get_pixbuf(GdkPixbufLoader* loader)
{
    gtk_func(gdk_pixbuf_loader_get_pixbuf);
    return s_gdk_pixbuf_loader_get_pixbuf(loader);
}

GdkPixbufLoader* gdk_pixbuf_loader_new(void)
{
    gtk_func(gdk_pixbuf_loader_new);
    return s_gdk_pixbuf_loader_new();
}

gboolean gdk_pixbuf_loader_write(GdkPixbufLoader* loader, const guchar* buf, gsize count, GError** error)
{
    gtk_func(gdk_pixbuf_loader_write);
    return s_gdk_pixbuf_loader_write(loader, buf, count, error);
}

GdkPixbuf* gdk_pixbuf_scale_simple(const GdkPixbuf* src, int dest_width, int dest_height, GdkInterpType interp_type)
{
    gtk_func(gdk_pixbuf_scale_simple);
    return s_gdk_pixbuf_scale_simple(src, dest_width, dest_height, interp_type);
}

GtkWidget* gtk_bin_get_child(GtkBin* bin)
{
    gtk_func(gtk_bin_get_child);
    return s_gtk_bin_get_child(bin);
}

GType gtk_bin_get_type(void)
{
    gtk_func(gtk_bin_get_type);
    return s_gtk_bin_get_type();
}

void gtk_box_pack_start(GtkBox* box, GtkWidget* child, gboolean expand, gboolean fill, guint padding)
{
    gtk_func(gtk_box_pack_start);
    s_gtk_box_pack_start(box, child, expand, fill, padding);
}

void gtk_box_reorder_child(GtkBox* box, GtkWidget* child, gint position)
{
    gtk_func(gtk_box_reorder_child);
    s_gtk_box_reorder_child(box, child, position);
}

gboolean gtk_check_menu_item_get_active(GtkCheckMenuItem* check_menu_item)
{
    gtk_func(gtk_check_menu_item_get_active);
    return s_gtk_check_menu_item_get_active(check_menu_item);
}

GType gtk_check_menu_item_get_type(void)
{
    gtk_func(gtk_check_menu_item_get_type);
    return s_gtk_check_menu_item_get_type();
}

GtkWidget* gtk_check_menu_item_new(void)
{
    gtk_func(gtk_check_menu_item_new);
    return s_gtk_check_menu_item_new();
}

void gtk_check_menu_item_set_active(GtkCheckMenuItem* check_menu_item, gboolean is_active)
{
    gtk_func(gtk_check_menu_item_set_active);
    s_gtk_check_menu_item_set_active(check_menu_item, is_active);
}

GList* gtk_container_get_children(GtkContainer* container)
{
    gtk_func(gtk_container_get_children);
    return s_gtk_container_get_children(container);
}

GtkWidget* gtk_image_new_from_pixbuf(GdkPixbuf* pixbuf)
{
    gtk_func(gtk_image_new_from_pixbuf);
    return s_gtk_image_new_from_pixbuf(pixbuf);
}

GType gtk_label_get_type(void)
{
    gtk_func(gtk_label_get_type);
    return s_gtk_label_get_type();
}

GtkWidget* gtk_label_new(const gchar* str)
{
    gtk_func(gtk_label_new);
    return s_gtk_label_new(str);
}

void gtk_label_set_text(GtkLabel* label, const gchar* str)
{
    gtk_func(gtk_label_set_text);
    s_gtk_label_set_text(label, str);
}

GtkWidget* gtk_menu_bar_new(void)
{
    gtk_func(gtk_menu_bar_new);
    return s_gtk_menu_bar_new();
}

GType gtk_menu_item_get_type(void)
{
    gtk_func(gtk_menu_item_get_type);
    return s_gtk_menu_item_get_type();
}

GtkWidget* gtk_menu_item_new(void)
{
    gtk_func(gtk_menu_item_new);
    return s_gtk_menu_item_new();
}

void gtk_menu_item_set_submenu(GtkMenuItem* menu_item, GtkWidget* submenu)
{
    gtk_func(gtk_menu_item_set_submenu);
    s_gtk_menu_item_set_submenu(menu_item, submenu);
}

GtkWidget* gtk_menu_new(void)
{
    gtk_func(gtk_menu_new);
    return s_gtk_menu_new();
}

void gtk_menu_shell_append(GtkMenuShell* menu_shell, GtkWidget* child)
{
    gtk_func(gtk_menu_shell_append);
    s_gtk_menu_shell_append(menu_shell, child);
}

GType gtk_menu_shell_get_type(void)
{
    gtk_func(gtk_menu_shell_get_type);
    return s_gtk_menu_shell_get_type();
}

GType gtk_menu_get_type(void)
{
    gtk_func(gtk_menu_get_type);
    return s_gtk_menu_get_type();
}

GtkWidget* gtk_separator_menu_item_new(void)
{
    gtk_func(gtk_separator_menu_item_new);
    return s_gtk_separator_menu_item_new();
}

const PangoFontDescription* gtk_style_context_get_font(GtkStyleContext* context, GtkStateFlags state)
{
    gtk_func(gtk_style_context_get_font);
    return s_gtk_style_context_get_font(context, state);
}

void gtk_widget_override_font(GtkWidget* widget, const PangoFontDescription* font_desc)
{
    gtk_func(gtk_widget_override_font);
    s_gtk_widget_override_font(widget, font_desc);
}

void gtk_widget_set_margin_bottom(GtkWidget* widget, gint margin)
{
    gtk_func(gtk_widget_set_margin_bottom);
    s_gtk_widget_set_margin_bottom(widget, margin);
}

void gtk_widget_set_margin_top(GtkWidget* widget, gint margin)
{
    gtk_func(gtk_widget_set_margin_top);
    s_gtk_widget_set_margin_top(widget, margin);
}

void gtk_widget_set_sensitive(GtkWidget* widget, gboolean sensitive)
{
    gtk_func(gtk_widget_set_sensitive);
    s_gtk_widget_set_sensitive(widget, sensitive);
}

void gtk_widget_set_tooltip_text(GtkWidget* widget, const gchar* text)
{
    gtk_func(gtk_widget_set_tooltip_text);
    s_gtk_widget_set_tooltip_text(widget, text);
}

void gtk_widget_show(GtkWidget* widget)
{
    gtk_func(gtk_widget_show);
    s_gtk_widget_show(widget);
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
