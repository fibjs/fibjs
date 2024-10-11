/*
 * Menu.cpp
 *
 *  Created on: Oct 11, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>
#include <gtk/gtk.h>

#include "object.h"
#include "Menu.h"
#include "EventInfo.h"

namespace fibjs {

static void on_menu_item_click(GtkWidget* widget, gpointer data)
{
    MenuItem* item = static_cast<MenuItem*>(data);
    obj_ptr<EventInfo> ei = new EventInfo(item, "click");
    item->_emit("click", ei);
}

static void on_menu_item_toggled(GtkCheckMenuItem* checkMenuItem, gpointer data)
{
    MenuItem* item = static_cast<MenuItem*>(data);
    gboolean active = gtk_check_menu_item_get_active(checkMenuItem);
    item->checked = active;
}

void MenuItem::update_os_menu_item()
{
    GtkWidget* gtkMenu = (GtkWidget*)m_menu_item;

    GtkWidget* box = gtk_bin_get_child(GTK_BIN(gtkMenu));
    GList* children = gtk_container_get_children(GTK_CONTAINER(box));
    for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
        if (GTK_IS_LABEL(iter->data)) {
            gtk_label_set_text(GTK_LABEL(iter->data), label->c_str());
            break;
        }
    }
    g_list_free(children);

    gtk_widget_set_tooltip_text(gtkMenu, tooltip->c_str());
    gtk_widget_set_sensitive(gtkMenu, enabled.value());

    if (checked.has_value())
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtkMenu), checked.value());
}

static void* create_os_menu_recursive(Menu* menu, const char* title, bool is_popup)
{
    GtkWidget* gtkMenu = is_popup ? gtk_menu_new() : gtk_menu_bar_new();

    for (MenuItem* item : menu->m_items) {
        GtkWidget* gtkMenuItem = nullptr;
        if (item->type.value() == "separator") {
            gtkMenuItem = gtk_separator_menu_item_new();
        } else {
            if (item->checked.has_value()) {
                gtkMenuItem = gtk_check_menu_item_new();
                gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtkMenuItem), item->checked.value());
                g_signal_connect(gtkMenuItem, "toggled", G_CALLBACK(on_menu_item_toggled), item);
            } else {
                gtkMenuItem = gtk_menu_item_new();
            }

            GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

            GtkWidget* label = gtk_label_new(item->label->c_str());

            GtkStyleContext* style_context = gtk_widget_get_style_context(gtkMenuItem);
            const PangoFontDescription* font_description = gtk_style_context_get_font(style_context, GTK_STATE_FLAG_NORMAL);

            PangoFontDescription* new_font_description = pango_font_description_copy(font_description);
            int font_size = pango_font_description_get_size(font_description) * 1.2;
            pango_font_description_set_size(new_font_description, font_size);

            gtk_widget_override_font(label, new_font_description);
            pango_font_description_free(new_font_description);

            font_size /= PANGO_SCALE;
            gtk_widget_set_margin_top(box, font_size / 3);
            gtk_widget_set_margin_bottom(box, font_size / 3);

            if (item->m_icon) {
                GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
                gdk_pixbuf_loader_write(loader, item->m_icon->data(), item->m_icon->length(), NULL);
                gdk_pixbuf_loader_close(loader, NULL);
                GdkPixbuf* pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
                if (pixbuf) {
                    int width = gdk_pixbuf_get_width(pixbuf);
                    int height = gdk_pixbuf_get_height(pixbuf);
                    int new_height = font_size * 1.6;
                    int new_width = width * new_height / height;

                    GdkPixbuf* scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
                    if (scaled_pixbuf) {
                        GtkWidget* image = gtk_image_new_from_pixbuf(scaled_pixbuf);
                        if (image)
                            gtk_container_add(GTK_CONTAINER(box), image);

                        g_object_unref(scaled_pixbuf);
                    }
                }
                g_object_unref(loader);
            }

            gtk_container_add(GTK_CONTAINER(box), label);

            gtk_container_add(GTK_CONTAINER(gtkMenuItem), box);

            if (item->tooltip.has_value())
                gtk_widget_set_tooltip_text(gtkMenuItem, item->tooltip->c_str());
            gtk_widget_set_sensitive(gtkMenuItem, item->enabled.value());

            if (item->submenu.has_value()) {
                GtkWidget* subMenu = (GtkWidget*)create_os_menu_recursive(item->submenu.value(), item->label->c_str(), true);
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(gtkMenuItem), subMenu);
            }

            g_signal_connect(gtkMenuItem, "activate", G_CALLBACK(on_menu_item_click), item);
        }

        gtk_menu_shell_append(GTK_MENU_SHELL(gtkMenu), gtkMenuItem);
        gtk_widget_show(gtkMenuItem);

        g_object_ref(gtkMenuItem);
        item->m_menu_item = gtkMenuItem;
    }

    g_object_ref(gtkMenu);
    menu->m_menu = gtkMenu;

    return gtkMenu;
}

void* Menu::create_os_menu(bool is_popup)
{
    if (m_menu)
        return m_menu;

    return create_os_menu_recursive(this, "main", is_popup);
}

static void release_os_menu_recursive(Menu* menu)
{
    for (MenuItem* item : menu->m_items) {
        if (item->submenu.has_value()) {
            release_os_menu_recursive(item->submenu.value());
        }
        if (item->m_menu_item) {
            g_object_unref(item->m_menu_item);
            item->m_menu_item = nullptr;
        }
    }

    if (menu->m_menu) {
        g_object_unref(menu->m_menu);
        menu->m_menu = nullptr;
    }
}

void Menu::release_os_menu()
{
    release_os_menu_recursive(this);
}

}

#endif
