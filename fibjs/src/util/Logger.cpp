/*
 * Logger.cpp
 *
 *  Created on: Sep 25, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/util.h"
#include "ifs/process.h"
#include "Logger.h"
#include "console.h"
#include "../util/util.h"
#include <uv/include/uv.h>

namespace fibjs {

result_t util_base::debuglog(exlib::string section, obj_ptr<Logger_base>& retVal)
{
    if (section.empty())
        return Runtime::setError("Logger: section is required.");

    retVal = new Logger(section);
    return 0;
}

result_t util_base::debuglog(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<Logger_base>& retVal)
{
    if (section.empty())
        return Runtime::setError("Logger: section is required.");

    retVal = new Logger(section, fn);
    return 0;
}

result_t util_base::debug(exlib::string section, obj_ptr<Logger_base>& retVal)
{
    return debuglog(section, retVal);
}

result_t util_base::debug(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<Logger_base>& retVal)
{
    return debuglog(section, fn, retVal);
}

bool Logger::check_env()
{
    char buf[4096];

    size_t sz = sizeof(buf);
    if (uv_os_getenv("NODE_DEBUG", buf, &sz) == 0) {
        const char* start = buf;
        while (true) {
            if (!qstricmp(start, m_section.c_str(), m_section.length())) {
                char ch = start[m_section.length()];
                if (ch == 0 || ch == ',')
                    return true;
            }

            const char* pos = qstrchr(start, ',');
            if (pos == NULL)
                return false;

            start = pos + 1;
        }
    }

    return false;
}

bool Logger::first_call()
{
    bool enabled = check_env();

    if (m_first) {
        m_first = false;
        fill_prefix();

        if (!m_fn.IsEmpty()) {
            Isolate* isolate = holder();
            v8::Local<v8::Function> fn = m_fn.Get(isolate->m_isolate);
            v8::Local<v8::Value> v;

            if (enabled)
                v = wrap();
            else {
                v8::Local<v8::Object> o = isolate->NewFunction("",
                    [](const v8::FunctionCallbackInfo<v8::Value>&) {});

                ClassInfo& ci = Classinfo();
                o->SetPrototype(isolate->context(), ci.GetPrototype(isolate)).IsJust();

                ClassData& cd = ci.data();
                for (int32_t i = 0; i < cd.mc; i++)
                    o->Set(isolate->context(), isolate->NewString(cd.cms[i].name), o).IsJust();

                v = o;
            }

            fn->Call(isolate->context(), v8::Undefined(isolate->m_isolate), 1, &v);
        }
    }

    return enabled;
}

static bool colors(Isolate* isolate, int32_t type)
{
    if (!isolate->m_console_colored)
        return false;

    if (type <= console_base::C_NOTICE)
        return false;

    return true;
}

void Logger::fill_prefix()
{
    int32_t pid;
    process_base::get_pid(pid);

    exlib::string _section(m_section);
    qstrupr(_section);

    Isolate* isolate = holder();

    std::vector<v8::Local<v8::Value>> args;
    args.push_back(isolate->NewString(_section));
    args.push_back(v8::Number::New(isolate->m_isolate, pid));

    util_format(isolate, "", args, false, m_prefix);
    m_prefix.append(": ", 2);

    util_format(isolate, "", args, true, m_prefix_color);
    m_prefix_color.append(": ", 2);
}

void Logger::_log(int32_t type, exlib::string fmt, OptArgs args)
{
    if (!first_call())
        return;

    int32_t level;

    console_base::get_loglevel(level);

    if (type <= level) {
        Isolate* isolate = holder();
        exlib::string str;
        bool is_color = colors(isolate, type);

        util_format(isolate, fmt, args, is_color, str);
        outLog(type, (is_color ? m_prefix_color : m_prefix) + str);
    }
}

result_t Logger::get_section(exlib::string& retVal)
{
    retVal = m_section;
    return 0;
}

result_t Logger::get_enabled(bool& retVal)
{
    retVal = check_env();
    return 0;
}

result_t Logger::_function(exlib::string fmt, OptArgs args)
{
    return debug(fmt, args);
}

result_t Logger::_function(OptArgs args)
{
    return debug(args);
}

result_t Logger::log(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_INFO, fmt, args);
    return 0;
}

result_t Logger::log(OptArgs args)
{
    _log(console_base::C_INFO, "", args);
    return 0;
}

result_t Logger::debug(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_DEBUG, fmt, args);
    return 0;
}

result_t Logger::debug(OptArgs args)
{
    _log(console_base::C_DEBUG, "", args);
    return 0;
}

result_t Logger::info(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_INFO, fmt, args);
    return 0;
}

result_t Logger::info(OptArgs args)
{
    _log(console_base::C_INFO, "", args);
    return 0;
}

result_t Logger::notice(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_NOTICE, fmt, args);
    return 0;
}

result_t Logger::notice(OptArgs args)
{
    _log(console_base::C_NOTICE, "", args);
    return 0;
}

result_t Logger::warn(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_WARN, fmt, args);
    return 0;
}

result_t Logger::warn(OptArgs args)
{
    _log(console_base::C_WARN, "", args);
    return 0;
}

result_t Logger::warning(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_WARN, fmt, args);
    return 0;
}

result_t Logger::warning(OptArgs args)
{
    _log(console_base::C_WARN, "", args);
    return 0;
}

result_t Logger::error(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_ERROR, fmt, args);
    return 0;
}

result_t Logger::error(OptArgs args)
{
    _log(console_base::C_ERROR, "", args);
    return 0;
}

result_t Logger::crit(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_CRIT, fmt, args);
    return 0;
}

result_t Logger::crit(OptArgs args)
{
    _log(console_base::C_CRIT, "", args);
    return 0;
}

result_t Logger::critical(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_CRIT, fmt, args);
    return 0;
}

result_t Logger::critical(OptArgs args)
{
    _log(console_base::C_CRIT, "", args);
    return 0;
}

result_t Logger::alert(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_ALERT, fmt, args);
    return 0;
}

result_t Logger::alert(OptArgs args)
{
    _log(console_base::C_ALERT, "", args);
    return 0;
}

result_t Logger::trace(exlib::string fmt, OptArgs args)
{
    if (!first_call())
        return 0;

    int32_t type = console_base::C_WARN;
    int32_t level;

    console_base::get_loglevel(level);

    if (type <= level) {
        exlib::string str;
        Isolate* isolate = holder();

        util_format(isolate, fmt, args, colors(isolate, type), str);
        if (str.empty())
            str = "Trace";
        else
            str = "Trace: " + str;

        str.append(1, '\n');
        str.append(traceInfo(isolate->m_isolate, 10));
        outLog(type, m_prefix + str);
    }

    return 0;
}

result_t Logger::trace(OptArgs args)
{
    return trace("", args);
}

result_t Logger::dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options)
{
    if (!first_call())
        return 0;

    exlib::string strBuffer;
    util_base::inspect(obj, options, strBuffer);

    outLog(console_base::C_INFO, m_prefix_color + strBuffer);
    return 0;
}

}
