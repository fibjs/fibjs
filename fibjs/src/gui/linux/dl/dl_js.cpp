
/*
 * dl_js.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#if defined(OS_DESKTOP) && !defined(_WIN32) && !defined(__APPLE__)

#include <exlib/include/dl_func.h>

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <jsc/jsc.h>

static void* js_handle = NULL;
#define js_func(func) dl_def_func(js_handle, func, "libjavascriptcoregtk-4.0.so", "libjavascriptcoregtk-4.1.so")

char* jsc_value_to_string(JSCValue* value)
{
    js_func(jsc_value_to_string);
    return s_jsc_value_to_string(value);
}

#endif
