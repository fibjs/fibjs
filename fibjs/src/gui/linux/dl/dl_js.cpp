
/*
 * dl_js.cpp
 *
 *  Created on: Sep 21, 2024
 *      Author: lion
 */

#include <exlib/include/osconfig.h>
#if defined(Linux) && defined(OS_DESKTOP)

#include <exlib/include/dl_func.h>
#include "ldconfig.h"

#define _GLIB_TEST_OVERFLOW_FALLBACK
#include <jsc/jsc.h>

static std::string js_lib = ldconfig("javascriptcoregtk-4.");
static void* js_handle = NULL;
#define js_func(func) dl_def_func(js_handle, func, js_lib.c_str())

char* jsc_value_to_string(JSCValue* value)
{
    js_func(jsc_value_to_string);
    return s_jsc_value_to_string(value);
}

#endif
