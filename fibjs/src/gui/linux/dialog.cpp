/*
 * dialog.mm
 *
 *  Created on: Oct 29, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>
#include <gtk/gtk.h>

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"

extern "C" GtkWidget* gtk_file_chooser_dialog_new_(const char* title, GtkWindow* parent,
    GtkFileChooserAction action, const char* b1, GtkResponseType t1, const char* b2, GtkResponseType t2);

namespace fibjs {

result_t gui_base::chooseFile(v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);

        obj_ptr<DialogOptions> opts;
        result_t hr = DialogOptions::load(isolate, options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_GUICALL);
    }

    DialogOptions* opts = (DialogOptions*)ac->m_ctx[0].object();

    bool isSaveDialog = opts->type.value() == "saveFile";

    GtkWidget* dialog;
    if (isSaveDialog)
        dialog = gtk_file_chooser_dialog_new_("Save File",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Save", GTK_RESPONSE_ACCEPT);
    else if (opts->type.value() == "openDirectory")
        dialog = gtk_file_chooser_dialog_new_("Open Directory",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT);
    else if (opts->type.value() == "openFile")
        dialog = gtk_file_chooser_dialog_new_("Open File",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Open", GTK_RESPONSE_ACCEPT);
    else
        return Runtime::setError("Invalid dialog type: " + opts->type.value());

    if (opts->title.has_value())
        gtk_window_set_title(GTK_WINDOW(dialog), opts->title.value().c_str());

    if (opts->defaultPath.has_value())
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), opts->defaultPath.value().c_str());

    if (opts->multiSelections.value())
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    if (opts->filters.has_value()) {
        for (auto& filterItem : opts->filters.value()) {
            GtkFileFilter* filter = gtk_file_filter_new();
            gtk_file_filter_set_name(filter, filterItem->name.c_str());
            for (auto& extension : filterItem->extensions) {
                std::string pattern = "*." + extension;
                gtk_file_filter_add_pattern(filter, pattern.c_str());
            }
            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        }
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        retVal = new NArray();

        if (opts->multiSelections.value()) {
            GSList* filenames = gtk_file_chooser_get_filenames(chooser);
            for (GSList* iter = filenames; iter != NULL; iter = iter->next) {
                retVal->append((char*)iter->data);
                g_free(iter->data);
            }
            g_slist_free(filenames);
        } else {
            char* filename = gtk_file_chooser_get_filename(chooser);
            retVal->append(filename);
            g_free(filename);
        }

        gtk_widget_destroy(dialog);
        return 0;
    }

    gtk_widget_destroy(dialog);
    return CALL_RETURN_UNDEFINED;
}

}

#endif
