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

class Logger_base : public object_base {
    DECLARE_CLASS(Logger_base);

public:
    // Logger_base
    virtual result_t _function(exlib::string fmt, OptArgs args) = 0;
    virtual result_t _function(OptArgs args) = 0;
    virtual result_t get_section(exlib::string& retVal) = 0;
    virtual result_t get_enabled(bool& retVal) = 0;
    virtual result_t log(exlib::string fmt, OptArgs args) = 0;
    virtual result_t log(OptArgs args) = 0;
    virtual result_t debug(exlib::string fmt, OptArgs args) = 0;
    virtual result_t debug(OptArgs args) = 0;
    virtual result_t info(exlib::string fmt, OptArgs args) = 0;
    virtual result_t info(OptArgs args) = 0;
    virtual result_t notice(exlib::string fmt, OptArgs args) = 0;
    virtual result_t notice(OptArgs args) = 0;
    virtual result_t warn(exlib::string fmt, OptArgs args) = 0;
    virtual result_t warn(OptArgs args) = 0;
    virtual result_t warning(exlib::string fmt, OptArgs args) = 0;
    virtual result_t warning(OptArgs args) = 0;
    virtual result_t error(exlib::string fmt, OptArgs args) = 0;
    virtual result_t error(OptArgs args) = 0;
    virtual result_t crit(exlib::string fmt, OptArgs args) = 0;
    virtual result_t crit(OptArgs args) = 0;
    virtual result_t critical(exlib::string fmt, OptArgs args) = 0;
    virtual result_t critical(OptArgs args) = 0;
    virtual result_t alert(exlib::string fmt, OptArgs args) = 0;
    virtual result_t alert(OptArgs args) = 0;
    virtual result_t trace(exlib::string fmt, OptArgs args) = 0;
    virtual result_t trace(OptArgs args) = 0;
    virtual result_t dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s__function(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_section(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_enabled(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_log(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_notice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_warn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_warning(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_error(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_crit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_critical(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Logger_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "log", s_log, false, ClassData::ASYNC_SYNC },
        { "debug", s_debug, false, ClassData::ASYNC_SYNC },
        { "info", s_info, false, ClassData::ASYNC_SYNC },
        { "notice", s_notice, false, ClassData::ASYNC_SYNC },
        { "warn", s_warn, false, ClassData::ASYNC_SYNC },
        { "warning", s_warning, false, ClassData::ASYNC_SYNC },
        { "error", s_error, false, ClassData::ASYNC_SYNC },
        { "crit", s_crit, false, ClassData::ASYNC_SYNC },
        { "critical", s_critical, false, ClassData::ASYNC_SYNC },
        { "alert", s_alert, false, ClassData::ASYNC_SYNC },
        { "trace", s_trace, false, ClassData::ASYNC_SYNC },
        { "dir", s_dir, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "section", s_get_section, block_set, false },
        { "enabled", s_get_enabled, block_set, false }
    };

    static ClassData s_cd = {
        "Logger", false, s__new, s__function,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Logger_base::s__function(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->_function(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->_function(v0);

    METHOD_VOID();
}

inline void Logger_base::s_get_section(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_section(vr);

    METHOD_RETURN();
}

inline void Logger_base::s_get_enabled(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_enabled(vr);

    METHOD_RETURN();
}

inline void Logger_base::s_log(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->log(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->log(v0);

    METHOD_VOID();
}

inline void Logger_base::s_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->debug(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->debug(v0);

    METHOD_VOID();
}

inline void Logger_base::s_info(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->info(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->info(v0);

    METHOD_VOID();
}

inline void Logger_base::s_notice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->notice(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->notice(v0);

    METHOD_VOID();
}

inline void Logger_base::s_warn(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->warn(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->warn(v0);

    METHOD_VOID();
}

inline void Logger_base::s_warning(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->warning(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->warning(v0);

    METHOD_VOID();
}

inline void Logger_base::s_error(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->error(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->error(v0);

    METHOD_VOID();
}

inline void Logger_base::s_crit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->crit(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->crit(v0);

    METHOD_VOID();
}

inline void Logger_base::s_critical(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->critical(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->critical(v0);

    METHOD_VOID();
}

inline void Logger_base::s_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->alert(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->alert(v0);

    METHOD_VOID();
}

inline void Logger_base::s_trace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->trace(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->trace(v0);

    METHOD_VOID();
}

inline void Logger_base::s_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Logger_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->dir(v0, v1);

    METHOD_VOID();
}
}
