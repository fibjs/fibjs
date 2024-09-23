
/*
 * dl_gobj.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP) && !defined(_WIN32) && !defined(__APPLE__)

#include <exlib/include/dl_func.h>

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib-object.h>

static void* gobj_handle = NULL;
#define gobj_func(func) dl_def_func(gobj_handle, "libgobject-2.0.so", func)

gpointer g_object_ref_sink(gpointer object)
{
    gobj_func(g_object_ref_sink);
    return s_g_object_ref_sink(object);
}

void g_object_unref(gpointer object)
{
    gobj_func(g_object_unref);
    s_g_object_unref(object);
}

guint g_signal_handlers_disconnect_matched(gpointer instance, GSignalMatchType mask,
    guint signal_id, GQuark detail, GClosure* closure, gpointer func, gpointer data)
{
    gobj_func(g_signal_handlers_disconnect_matched);
    return s_g_signal_handlers_disconnect_matched(instance, mask, signal_id, detail, closure, func, data);
}

GTypeInstance* g_type_check_instance_cast(GTypeInstance* instance, GType iface_type)
{
    gobj_func(g_type_check_instance_cast);
    return s_g_type_check_instance_cast(instance, iface_type);
}

void g_free(gpointer mem)
{
    gobj_func(g_free);
    s_g_free(mem);
}

gulong g_signal_connect_data(gpointer instance, const gchar* detailed_signal, GCallback c_handler,
    gpointer data, GClosureNotify destroy_data, GConnectFlags connect_flags)
{
    gobj_func(g_signal_connect_data);
    return s_g_signal_connect_data(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}

#endif
