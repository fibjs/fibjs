
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

OpaqueJSString* JSValueToStringCopy(const OpaqueJSContext* ctx, const OpaqueJSValue* value, const OpaqueJSValue** exception)
{
    js_func(JSValueToStringCopy);
    return s_JSValueToStringCopy(ctx, value, exception);
}

size_t JSStringGetMaximumUTF8CStringSize(OpaqueJSString* string)
{
    js_func(JSStringGetMaximumUTF8CStringSize);
    return s_JSStringGetMaximumUTF8CStringSize(string);
}

size_t JSStringGetUTF8CString(OpaqueJSString* string, char* buffer, size_t bufferSize)
{
    js_func(JSStringGetUTF8CString);
    return s_JSStringGetUTF8CString(string, buffer, bufferSize);
}

void JSStringRelease(OpaqueJSString* string)
{
    js_func(JSStringRelease);
    s_JSStringRelease(string);
}

int jsc_value_is_boolean(JSCValue* value)
{
    js_func(jsc_value_is_boolean);
    return s_jsc_value_is_boolean(value);
}

int jsc_value_to_boolean(JSCValue* value)
{
    js_func(jsc_value_to_boolean);
    return s_jsc_value_to_boolean(value);
}

int jsc_value_is_number(JSCValue* value)
{
    js_func(jsc_value_is_number);
    return s_jsc_value_is_number(value);
}

double jsc_value_to_double(JSCValue* value)
{
    js_func(jsc_value_to_double);
    return s_jsc_value_to_double(value);
}

int jsc_value_is_string(JSCValue* value)
{
    js_func(jsc_value_is_string);
    return s_jsc_value_is_string(value);
}

int jsc_value_is_null(JSCValue* value)
{
    js_func(jsc_value_is_null);
    return s_jsc_value_is_null(value);
}

int jsc_value_is_array(JSCValue* value)
{
    js_func(jsc_value_is_array);
    return s_jsc_value_is_array(value);
}

JSCValue* jsc_value_object_get_property(JSCValue* object, const char* propertyName)
{
    js_func(jsc_value_object_get_property);
    return s_jsc_value_object_get_property(object, propertyName);
}

int32_t jsc_value_to_int32(JSCValue* value)
{
    js_func(jsc_value_to_int32);
    return s_jsc_value_to_int32(value);
}

int jsc_value_is_object(JSCValue* value)
{
    js_func(jsc_value_is_object);
    return s_jsc_value_is_object(value);
}

char** jsc_value_object_enumerate_properties(JSCValue* object)
{
    js_func(jsc_value_object_enumerate_properties);
    return s_jsc_value_object_enumerate_properties(object);
}

JSCValue* jsc_value_object_get_property_at_index(JSCValue* object, unsigned index)
{
    js_func(jsc_value_object_get_property_at_index);
    return s_jsc_value_object_get_property_at_index(object, index);
}

JSCContext* jsc_value_get_context(JSCValue* value)
{
    js_func(jsc_value_get_context);
    return s_jsc_value_get_context(value);
}

JSCException* jsc_context_get_exception(JSCContext* ctx)
{
    js_func(jsc_context_get_exception);
    return s_jsc_context_get_exception(ctx);
}

const char* jsc_exception_get_message(JSCException* exception)
{
    js_func(jsc_exception_get_message);
    return s_jsc_exception_get_message(exception);
}

void jsc_context_clear_exception(JSCContext* ctx)
{
    js_func(jsc_context_clear_exception);
    s_jsc_context_clear_exception(ctx);
}

#endif
