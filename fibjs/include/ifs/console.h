/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _console_base_H_
#define _console_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class TextColor_base;

class console_base : public object_base {
    DECLARE_CLASS(console_base);

public:
    enum {
        _FATAL = 0,
        _ALERT = 1,
        _CRIT = 2,
        _ERROR = 3,
        _WARN = 4,
        _NOTICE = 5,
        _INFO = 6,
        _DEBUG = 7,
        _PRINT = 9,
        _NOTSET = 10
    };

public:
    // console_base
    static result_t get_loglevel(int32_t& retVal);
    static result_t set_loglevel(int32_t newVal);
    static result_t get_colors(obj_ptr<TextColor_base>& retVal);
    static result_t get_width(int32_t& retVal);
    static result_t get_height(int32_t& retVal);
    static result_t add(exlib::string type);
    static result_t add(v8::Local<v8::Object> cfg);
    static result_t add(v8::Local<v8::Array> cfg);
    static result_t reset();
    static result_t log(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t log(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t debug(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t info(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t notice(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t notice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t warn(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t warn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t error(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t error(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t crit(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t crit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t alert(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t dir(v8::Local<v8::Value> obj);
    static result_t time(exlib::string label);
    static result_t timeEnd(exlib::string label);
    static result_t trace(exlib::string label);
    static result_t _assert(v8::Local<v8::Value> value, exlib::string msg);
    static result_t print(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t moveTo(int32_t row, int32_t column);
    static result_t hideCursor();
    static result_t showCursor();
    static result_t clear();
    static result_t keyDown(exlib::string key, exlib::string modifier);
    static result_t keyDown(exlib::string key, v8::Local<v8::Array> modifier);
    static result_t keyUp(exlib::string key, exlib::string modifier);
    static result_t keyUp(exlib::string key, v8::Local<v8::Array> modifier);
    static result_t keyTap(exlib::string key, exlib::string modifier);
    static result_t keyTap(exlib::string key, v8::Local<v8::Array> modifier);
    static result_t typeString(exlib::string text);
    static result_t moveMouse(int32_t x, int32_t y);
    static result_t mouseUp(exlib::string button);
    static result_t mouseDown(exlib::string button);
    static result_t clickMouse(exlib::string button, bool dbclick);
    static result_t readLine(exlib::string msg, exlib::string& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_FATAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ALERT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CRIT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ERROR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_WARN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_NOTICE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_INFO(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_DEBUG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PRINT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_NOTSET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_loglevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_colors(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_width(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_height(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_log(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_notice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_warn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_error(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_crit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_moveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hideCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_showCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keyDown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keyUp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keyTap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_typeString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_moveMouse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mouseUp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mouseDown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clickMouse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readLine(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(console_base, readLine, exlib::string, exlib::string);
};
}

#include "TextColor.h"

namespace fibjs {
inline ClassInfo& console_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "add", s_add, true },
        { "reset", s_reset, true },
        { "log", s_log, true },
        { "debug", s_debug, true },
        { "info", s_info, true },
        { "notice", s_notice, true },
        { "warn", s_warn, true },
        { "error", s_error, true },
        { "crit", s_crit, true },
        { "alert", s_alert, true },
        { "dir", s_dir, true },
        { "time", s_time, true },
        { "timeEnd", s_timeEnd, true },
        { "trace", s_trace, true },
        { "assert", s_assert, true },
        { "print", s_print, true },
        { "moveTo", s_moveTo, true },
        { "hideCursor", s_hideCursor, true },
        { "showCursor", s_showCursor, true },
        { "clear", s_clear, true },
        { "keyDown", s_keyDown, true },
        { "keyUp", s_keyUp, true },
        { "keyTap", s_keyTap, true },
        { "typeString", s_typeString, true },
        { "moveMouse", s_moveMouse, true },
        { "mouseUp", s_mouseUp, true },
        { "mouseDown", s_mouseDown, true },
        { "clickMouse", s_clickMouse, true },
        { "readLine", s_readLine, true },
        { "readLineSync", s_readLine, true }
    };

    static ClassData::ClassProperty s_property[] = {
        { "FATAL", s_get_FATAL, block_set, true },
        { "ALERT", s_get_ALERT, block_set, true },
        { "CRIT", s_get_CRIT, block_set, true },
        { "ERROR", s_get_ERROR, block_set, true },
        { "WARN", s_get_WARN, block_set, true },
        { "NOTICE", s_get_NOTICE, block_set, true },
        { "INFO", s_get_INFO, block_set, true },
        { "DEBUG", s_get_DEBUG, block_set, true },
        { "PRINT", s_get_PRINT, block_set, true },
        { "NOTSET", s_get_NOTSET, block_set, true },
        { "loglevel", s_get_loglevel, s_set_loglevel, true },
        { "colors", s_get_colors, block_set, true },
        { "width", s_get_width, block_set, true },
        { "height", s_get_height, block_set, true }
    };

    static ClassData s_cd = {
        "console", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void console_base::s_get_FATAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _FATAL;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_ALERT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _ALERT;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_CRIT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CRIT;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_ERROR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _ERROR;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_WARN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _WARN;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_NOTICE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _NOTICE;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_INFO(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _INFO;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_DEBUG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _DEBUG;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_PRINT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _PRINT;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_NOTSET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _NOTSET;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void console_base::s_get_loglevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_loglevel(vr);

    METHOD_RETURN();
}

inline void console_base::s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_loglevel(v0);

    PROPERTY_SET_LEAVE();
}

inline void console_base::s_get_colors(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<TextColor_base> vr;

    PROPERTY_ENTER();

    hr = get_colors(vr);

    METHOD_RETURN();
}

inline void console_base::s_get_width(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_width(vr);

    METHOD_RETURN();
}

inline void console_base::s_get_height(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_height(vr);

    METHOD_RETURN();
}

inline void console_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = add(v0);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = add(v0);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = add(v0);

    METHOD_VOID();
}

inline void console_base::s_reset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = reset();

    METHOD_VOID();
}

inline void console_base::s_log(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = log(v0, args);

    METHOD_OVER(-1, 0);

    hr = log(args);

    METHOD_VOID();
}

inline void console_base::s_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = debug(v0, args);

    METHOD_OVER(-1, 0);

    hr = debug(args);

    METHOD_VOID();
}

inline void console_base::s_info(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = info(v0, args);

    METHOD_OVER(-1, 0);

    hr = info(args);

    METHOD_VOID();
}

inline void console_base::s_notice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = notice(v0, args);

    METHOD_OVER(-1, 0);

    hr = notice(args);

    METHOD_VOID();
}

inline void console_base::s_warn(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = warn(v0, args);

    METHOD_OVER(-1, 0);

    hr = warn(args);

    METHOD_VOID();
}

inline void console_base::s_error(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = error(v0, args);

    METHOD_OVER(-1, 0);

    hr = error(args);

    METHOD_VOID();
}

inline void console_base::s_crit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = crit(v0, args);

    METHOD_OVER(-1, 0);

    hr = crit(args);

    METHOD_VOID();
}

inline void console_base::s_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = alert(v0, args);

    METHOD_OVER(-1, 0);

    hr = alert(args);

    METHOD_VOID();
}

inline void console_base::s_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = dir(v0);

    METHOD_VOID();
}

inline void console_base::s_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = time(v0);

    METHOD_VOID();
}

inline void console_base::s_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = timeEnd(v0);

    METHOD_VOID();
}

inline void console_base::s_trace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "trace");

    hr = trace(v0);

    METHOD_VOID();
}

inline void console_base::s_assert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _assert(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = print(v0, args);

    METHOD_OVER(-1, 0);

    hr = print(args);

    METHOD_VOID();
}

inline void console_base::s_moveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = moveTo(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_hideCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = hideCursor();

    METHOD_VOID();
}

inline void console_base::s_showCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = showCursor();

    METHOD_VOID();
}

inline void console_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = clear();

    METHOD_VOID();
}

inline void console_base::s_keyDown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = keyDown(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = keyDown(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_keyUp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = keyUp(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = keyUp(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_keyTap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = keyTap(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = keyTap(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_typeString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = typeString(v0);

    METHOD_VOID();
}

inline void console_base::s_moveMouse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = moveMouse(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_mouseUp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = mouseUp(v0);

    METHOD_VOID();
}

inline void console_base::s_mouseDown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = mouseDown(v0);

    METHOD_VOID();
}

inline void console_base::s_clickMouse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    hr = clickMouse(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_readLine(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty()) {
        acb_readLine(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_readLine(v0, vr);

    METHOD_RETURN();
}
}

#endif
