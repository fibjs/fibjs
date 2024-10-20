
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
#include <JavaScriptCore/JavaScript.h>

const char* js_str()
{
    static std::string str = ldconfig("javascriptcoregtk-4.");
    return str.c_str();
}

static void* js_handle = NULL;
#define js_func(func) dl_def_func(js_handle, func, js_str())

void dl_js_init()
{
}

char* jsc_value_to_string(JSCValue* value)
{
    js_func(jsc_value_to_string);
    return s_jsc_value_to_string(value);
}

OpaqueJSString * JSValueToStringCopy(const OpaqueJSContext* ctx, const OpaqueJSValue* value, const OpaqueJSValue** exception)
{
    js_func(JSValueToStringCopy);
    return s_JSValueToStringCopy(ctx, value, exception);
}

size_t JSStringGetMaximumUTF8CStringSize(OpaqueJSString * string)
{
    js_func(JSStringGetMaximumUTF8CStringSize);
    return s_JSStringGetMaximumUTF8CStringSize(string);
}

size_t JSStringGetUTF8CString(OpaqueJSString * string, char* buffer, size_t bufferSize)
{
    js_func(JSStringGetUTF8CString);
    return s_JSStringGetUTF8CString(string, buffer, bufferSize);
}

void JSStringRelease(OpaqueJSString * string)
{
    js_func(JSStringRelease);
    s_JSStringRelease(string);
}

#endif
