
/*
 * dl_glib.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP) && !defined(_WIN32) && !defined(__APPLE__)

#include <exlib/include/dl_func.h>

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <glib.h>

static void* glib_handle = NULL;
#define glib_func(func) dl_def_func(glib_handle, "libglib-2.0.so", func)

guint g_idle_add_full(gint priority, GSourceFunc function, gpointer data, GDestroyNotify notify)
{
    glib_func(g_idle_add_full);
    return s_g_idle_add_full(priority, function, data, notify);
}

#endif
