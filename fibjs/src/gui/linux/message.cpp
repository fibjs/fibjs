/*
 * message.cpp
 *
 *  Created on: Oct 25, 2024
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

result_t gui_base::alert(exlib::string message, AsyncEvent* ac)
{
    return alert("", message, ac);
}

result_t gui_base::alert(exlib::string title, exlib::string message, AsyncEvent* ac)
{
    result_t hr = check_gui(ac);
    if (hr < 0)
        return hr;

    GtkWidget* dialog = gtk_message_dialog_new_(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        message.c_str());
    gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return 0;
}

result_t gui_base::confirm(exlib::string message, bool& retVal, AsyncEvent* ac)
{
    return confirm("", message, retVal, ac);
}

result_t gui_base::confirm(exlib::string title, exlib::string message, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_gui(ac);
    if (hr < 0)
        return hr;

    GtkWidget* dialog = gtk_message_dialog_new_(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_OK_CANCEL,
        message.c_str());
    gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    retVal = result == GTK_RESPONSE_OK;
    return 0;
}

}

#endif
