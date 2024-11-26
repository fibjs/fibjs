/*
 * input.cpp
 *
 *  Created on: Nov 22, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <gtk/gtk.h>

extern "C" GtkWidget* gtk_message_dialog_new_(GtkWindow* parent, GtkDialogFlags flags, GtkMessageType type,
    GtkButtonsType buttons, const gchar* message);

namespace fibjs {

result_t gui_base::input(exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    return input("", message, password, retVal, ac);
}

result_t gui_base::input(exlib::string title, exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    GtkWidget* dialog = gtk_message_dialog_new_(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_OK_CANCEL,
        message.c_str());
    gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog), message.c_str());

    GtkWidget* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry), password ? FALSE : TRUE);
    gtk_widget_set_margin_start(entry, 12);
    gtk_widget_set_margin_end(entry, 12);
    gtk_container_add(GTK_CONTAINER(content), entry);
    
    g_signal_connect(G_OBJECT(entry), "activate",
        G_CALLBACK(+[](GtkEntry* entry, gpointer dialog) {
            gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
        }),
        dialog);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 200);
    gtk_window_set_modal(GTK_WINDOW(dialog), true);
    gtk_window_set_resizable(GTK_WINDOW(dialog), false);
    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        retVal = gtk_entry_get_text(GTK_ENTRY(entry));
    }

    gtk_widget_destroy(dialog);

    return response == GTK_RESPONSE_OK ? 0 : CALL_RETURN_UNDEFINED;
}

}

#endif