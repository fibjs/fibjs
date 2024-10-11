
/*
 * dl_gobj.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include <exlib/include/dl_func.h>
#include "ldconfig.h"

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib-object.h>

const char* gobj_str()
{
    static std::string str = ldconfig("gobject-2.");
    return str.c_str();
}

static void* gobj_handle = NULL;
#define gobj_func(func) dl_def_func(gobj_handle, func, gobj_str())

void dl_gobj_init()
{
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

void* g_malloc(gsize n_bytes)
{
    gobj_func(g_malloc);
    return s_g_malloc(n_bytes);
}

gulong g_signal_connect_data(gpointer instance, const gchar* detailed_signal, GCallback c_handler,
    gpointer data, GClosureNotify destroy_data, GConnectFlags connect_flags)
{
    gobj_func(g_signal_connect_data);
    return s_g_signal_connect_data(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}

void g_object_unref(gpointer object)
{
    gobj_func(g_object_unref);
    s_g_object_unref(object);
}

GError* g_error_new_valist(GQuark domain, gint code, const gchar* format, va_list args)
{
    gobj_func(g_error_new_valist);
    return s_g_error_new_valist(domain, code, format, args);
}

GError* g_error_new(GQuark domain, gint code, const gchar* format, ...)
{
    gobj_func(g_error_new);
    va_list args;
    va_start(args, format);
    GError* ret = g_error_new_valist(domain, code, format, args);
    va_end(args);
    return ret;
}

void g_error_free(GError* error)
{
    gobj_func(g_error_free);
    s_g_error_free(error);
}

#endif
