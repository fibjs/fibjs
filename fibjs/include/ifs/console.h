/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class console_base : public object_base {
    DECLARE_CLASS(console_base);

public:
    enum {
        C_FATAL = 0,
        C_ALERT = 1,
        C_CRIT = 2,
        C_ERROR = 3,
        C_WARN = 4,
        C_NOTICE = 5,
        C_INFO = 6,
        C_DEBUG = 7,
        C_PRINT = 9,
        C_NOTSET = 10
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
    static result_t trace(exlib::string fmt, OptArgs args);
    static result_t trace(OptArgs args);
    static result_t dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options);
    static result_t table(v8::Local<v8::Value> obj);
    static result_t table(v8::Local<v8::Value> obj, v8::Local<v8::Array> fields);
    static result_t time(exlib::string label);
    static result_t timeElapse(exlib::string label);
    static result_t timeEnd(exlib::string label);
    static result_t _assert(v8::Local<v8::Value> value, exlib::string msg);
    static result_t print(exlib::string fmt, OptArgs args);
    static result_t print(OptArgs args);
    static result_t moveTo(int32_t row, int32_t column);
    static result_t hideCursor();
    static result_t showCursor();
    static result_t clear();
    static result_t readLine(exlib::string msg, exlib::string& retVal, AsyncEvent* ac);
    static result_t getpass(exlib::string msg, exlib::string& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_width(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_height(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_table(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_timeElapse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static__assert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_moveTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hideCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_showCursor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        { "add", s_static_add, true, ClassData::ASYNC_SYNC },
        { "reset", s_static_reset, true, ClassData::ASYNC_SYNC },
        { "log", s_static_log, true, ClassData::ASYNC_SYNC },
        { "debug", s_static_debug, true, ClassData::ASYNC_SYNC },
        { "info", s_static_info, true, ClassData::ASYNC_SYNC },
        { "notice", s_static_notice, true, ClassData::ASYNC_SYNC },
        { "warn", s_static_warn, true, ClassData::ASYNC_SYNC },
        { "error", s_static_error, true, ClassData::ASYNC_SYNC },
        { "crit", s_static_crit, true, ClassData::ASYNC_SYNC },
        { "alert", s_static_alert, true, ClassData::ASYNC_SYNC },
        { "trace", s_static_trace, true, ClassData::ASYNC_SYNC },
        { "dir", s_static_dir, true, ClassData::ASYNC_SYNC },
        { "table", s_static_table, true, ClassData::ASYNC_SYNC },
        { "time", s_static_time, true, ClassData::ASYNC_SYNC },
        { "timeElapse", s_static_timeElapse, true, ClassData::ASYNC_SYNC },
        { "timeEnd", s_static_timeEnd, true, ClassData::ASYNC_SYNC },
        { "assert", s_static__assert, true, ClassData::ASYNC_SYNC },
        { "print", s_static_print, true, ClassData::ASYNC_SYNC },
        { "moveTo", s_static_moveTo, true, ClassData::ASYNC_SYNC },
        { "hideCursor", s_static_hideCursor, true, ClassData::ASYNC_SYNC },
        { "showCursor", s_static_showCursor, true, ClassData::ASYNC_SYNC },
        { "clear", s_static_clear, true, ClassData::ASYNC_SYNC },
        { "readLine", s_static_readLine, true, ClassData::ASYNC_ASYNC },
        { "getpass", s_static_getpass, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "loglevel", s_static_get_loglevel, s_static_set_loglevel, true },
        { "width", s_static_get_width, block_set, true },
        { "height", s_static_get_height, block_set, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "FATAL", C_FATAL },
        { "ALERT", C_ALERT },
        { "CRIT", C_CRIT },
        { "ERROR", C_ERROR },
        { "WARN", C_WARN },
        { "NOTICE", C_NOTICE },
        { "INFO", C_INFO },
        { "DEBUG", C_DEBUG },
        { "PRINT", C_PRINT },
        { "NOTSET", C_NOTSET }
    };

    static ClassData s_cd = {
        "console", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void console_base::s_static_get_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_loglevel(vr);

    METHOD_RETURN();
}

inline void console_base::s_static_set_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_loglevel(v0);

    METHOD_VOID();
}

inline void console_base::s_static_get_width(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_width(vr);

    METHOD_RETURN();
}

inline void console_base::s_static_get_height(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_height(vr);

    METHOD_RETURN();
}

inline void console_base::s_static_add(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void console_base::s_static_reset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = reset();

    METHOD_VOID();
}

inline void console_base::s_static_log(const v8::FunctionCallbackInfo<v8::Value>& args)
{
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

inline void console_base::s_static_trace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = trace(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = trace(v0);

    METHOD_VOID();
}

inline void console_base::s_static_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = dir(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_table(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = table(v0);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = table(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = time(v0);

    METHOD_VOID();
}

inline void console_base::s_static_timeElapse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = timeElapse(v0);

    METHOD_VOID();
}

inline void console_base::s_static_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "time");

    hr = timeEnd(v0);

    METHOD_VOID();
}

inline void console_base::s_static__assert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _assert(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
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
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = moveTo(v0, v1);

    METHOD_VOID();
}

inline void console_base::s_static_hideCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = hideCursor();

    METHOD_VOID();
}

inline void console_base::s_static_showCursor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = showCursor();

    METHOD_VOID();
}

inline void console_base::s_static_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = clear();

    METHOD_VOID();
}

inline void console_base::s_static_readLine(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = acb_readLine(v0, cb, args);
    else
        hr = ac_readLine(v0, vr);

    METHOD_RETURN();
}

inline void console_base::s_static_getpass(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = acb_getpass(v0, cb, args);
    else
        hr = ac_getpass(v0, vr);

    METHOD_RETURN();
}
}
