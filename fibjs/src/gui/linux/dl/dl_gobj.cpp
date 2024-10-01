
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
