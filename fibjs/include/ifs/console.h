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
    static result_t get_width(int32_t& retVal);
    static result_t get_height(int32_t& retVal);
    static result_t add(exlib::string type);
    static result_t add(v8::Local<v8::Object> cfg);
    static result_t add(v8::Local<v8::Array> cfg);
    static result_t reset();
    static result_t log(exlib::string fmt, OptArgs args);
    static result_t log(OptArgs args);
    static result_t debug(exlib::string fmt, OptArgs args);
    static result_t debug(OptArgs args);
    static result_t info(exlib::string fmt, OptArgs args);
    static result_t info(OptArgs args);
    static result_t notice(exlib::string fmt, OptArgs args);
    static result_t notice(OptArgs args);
    static result_t warn(exlib::string fmt, OptArgs args);
    static result_t warn(OptArgs args);
    static result_t error(exlib::string fmt, OptArgs args);
    static result_t error(OptArgs args);
    static result_t crit(exlib::string fmt, OptArgs args);
    static result_t crit(OptArgs args);
    static result_t alert(exlib::string fmt, OptArgs args);
    static result_t alert(OptArgs args);
    static result_t dir(v8::Local<v8::Value> obj);
    static result_t time(exlib::string label);
    static result_t timeElapse(exlib::string label);
    static result_t timeEnd(exlib::string label);
    static result_t trace(exlib::string label);
    static result_t _assert(v8::Local<v8::Value> value, exlib::string msg);
    static result_t print(exlib::string fmt, OptArgs args);
    static result_t print(OptArgs args);
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
    static result_t getpass(exlib::string msg, exlib::string& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_loglevel(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_loglevel(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_width(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_height(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_reset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_log(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_warn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_crit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_timeElapse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static__assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_moveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hideCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_showCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keyDown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keyUp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keyTap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_typeString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_moveMouse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mouseUp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mouseDown(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clickMouse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_readLine(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getpass(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(console_base, readLine, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(console_base, getpass, exlib::string, exlib::string);
};
}

namespace fibjs {
inline ClassInfo& console_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "add", s_static_add, true },
        { "reset", s_static_reset, true },
        { "log", s_static_log, true },
        { "debug", s_static_debug, true },
        { "info", s_static_info, true },
        { "notice", s_static_notice, true },
        { "warn", s_static_warn, true },
        { "error", s_static_error, true },
        { "crit", s_static_crit, true },
        { "alert", s_static_alert, true },
        { "dir", s_static_dir, true },
        { "time", s_static_time, true },
        { "timeElapse", s_static_timeElapse, true },
        { "timeEnd", s_static_timeEnd, true },
        { "trace", s_static_trace, true },
        { "assert", s_static__assert, true },
        { "print", s_static_print, true },
        { "moveTo", s_static_moveTo, true },
        { "hideCursor", s_static_hideCursor, true },
        { "showCursor", s_static_showCursor, true },
        { "clear", s_static_clear, true },
        { "keyDown", s_static_keyDown, true },
        { "keyUp", s_static_keyUp, true },
        { "keyTap", s_static_keyTap, true },
        { "typeString", s_static_typeString, true },
        { "moveMouse", s_static_moveMouse, true },
        { "mouseUp", s_static_mouseUp, true },
        { "mouseDown", s_static_mouseDown, true },
        { "clickMouse", s_static_clickMouse, true },
        { "readLine", s_static_readLine, true },
        { "readLineSync", s_static_readLine, true },
        { "getpass", s_static_getpass, true },
        { "getpassSync", s_static_getpass, true }
    };

    static ClassData::ClassProperty s_property[] = {
        { "loglevel", s_static_get_loglevel, s_static_set_loglevel, true },
        { "width", s_static_get_width, block_set, true },
        { "height", s_static_get_height, block_set, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "FATAL", _FATAL },
        { "ALERT", _ALERT },
        { "CRIT", _CRIT },
        { "ERROR", _ERROR },
        { "WARN", _WARN },
        { "NOTICE", _NOTICE },
        { "INFO", _INFO },
        { "DEBUG", _DEBUG },
        { "PRINT", _PRINT },
        { "NOTSET", _NOTSET }
    };

    static ClassData s_cd = {
        "console", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void console_base::s_static_get_loglevel(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("console.loglevel");
    PROPERTY_ENTER();

    hr = get_loglevel(vr);

    METHOD_RETURN();
}

inline void console_base::s_static_set_loglevel(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("console.loglevel");
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_loglevel(v0);

    PROPERTY_SET_LEAVE();
}

inline void console_base::s_static_get_width(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("console.width");
    PROPERTY_ENTER();

    hr = get_width(vr);

    METHOD_RETURN();
}

inline void console_base::s_static_get_height(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("console.height");
    PROPERTY_ENTER();

    hr = get_height(vr);

    METHOD_RETURN();
}

inline void console_base::s_static_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.add");
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

inline void console_base::s_static_reset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.reset");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = reset();

    METHOD_VOID();
}

inline void console_base::s_static_log(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.log");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = log(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = log(v0);

    METHOD_VOID();
}

inline void console_base::s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.debug");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = debug(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = debug(v0);

    METHOD_VOID();
}

inline void console_base::s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.info");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = info(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = info(v0);

    METHOD_VOID();
}

inline void console_base::s_static_notice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.notice");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = notice(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = notice(v0);

    METHOD_VOID();
}

inline void console_base::s_static_warn(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.warn");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = warn(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = warn(v0);

    METHOD_VOID();
}

inline void console_base::s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.error");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = error(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = error(v0);

    METHOD_VOID();
}

inline void console_base::s_static_crit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.crit");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = crit(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = crit(v0);

    METHOD_VOID();
}

inline void console_base::s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.alert");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = alert(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = alert(v0);

    METHOD_VOID();
}

inline void console_base::s_static_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.dir");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = dir(v0);

    METHOD_VOID();
}

inline void console_base::s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.time");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = time(v0);

    METHOD_VOID();
}

inline void console_base::s_static_timeElapse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.timeElapse");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = timeElapse(v0);

    METHOD_VOID();
}

inline void console_base::s_static_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.timeEnd");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = timeEnd(v0);

    METHOD_VOID();
}

inline void console_base::s_static_trace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.trace");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "trace");

    hr = trace(v0);

    METHOD_VOID();
}

inline void console_base::s_static__assert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.assert");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _assert(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.print");
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = print(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = print(v0);

    METHOD_VOID();
}

inline void console_base::s_static_moveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.moveTo");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = moveTo(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_hideCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.hideCursor");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = hideCursor();

    METHOD_VOID();
}

inline void console_base::s_static_showCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.showCursor");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = showCursor();

    METHOD_VOID();
}

inline void console_base::s_static_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.clear");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = clear();

    METHOD_VOID();
}

inline void console_base::s_static_keyDown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.keyDown");
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

inline void console_base::s_static_keyUp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.keyUp");
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

inline void console_base::s_static_keyTap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.keyTap");
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

inline void console_base::s_static_typeString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.typeString");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = typeString(v0);

    METHOD_VOID();
}

inline void console_base::s_static_moveMouse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.moveMouse");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = moveMouse(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_mouseUp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.mouseUp");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = mouseUp(v0);

    METHOD_VOID();
}

inline void console_base::s_static_mouseDown(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.mouseDown");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = mouseDown(v0);

    METHOD_VOID();
}

inline void console_base::s_static_clickMouse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("console.clickMouse");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    hr = clickMouse(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_readLine(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("console.readLine");
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

inline void console_base::s_static_getpass(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("console.getpass");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty()) {
        acb_getpass(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_getpass(v0, vr);

    METHOD_RETURN();
}
}

#endif
