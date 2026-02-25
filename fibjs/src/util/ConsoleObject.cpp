/*
 * Logger.cpp
 *
 *  Created on: Sep 25, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/util.h"
#include "ifs/process.h"
#include "ConsoleObject.h"
#include "console.h"
#include "../util/util.h"
#include <uv/include/uv.h>

namespace fibjs {

static bool is_writable(Isolate* isolate, v8::Local<v8::Value> v, v8::Local<v8::Object>& obj)
{
    if (v.IsEmpty() || !v->IsObject())
        return false;

    obj = v.As<v8::Object>();
    v8::Local<v8::Value> write_fn;
    if (!obj->Get(isolate->context(), isolate->NewString("write")).ToLocal(&write_fn))
        return false;

    return write_fn->IsFunction();
}

result_t ConsoleObject_base::_new(obj_ptr<ConsoleObject_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new ConsoleObject();
    return 0;
}

result_t ConsoleObject_base::_new(v8::Local<v8::Value> stdout_val, v8::Local<v8::Value> stderr_val,
    obj_ptr<ConsoleObject_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    v8::Local<v8::Object> stdout_obj;
    if (!is_writable(isolate, stdout_val, stdout_obj)) {
        // If stdout is an Object without write(), treat it as options
        if (stdout_val->IsObject() && (stderr_val.IsEmpty() || stderr_val->IsUndefined())) {
            v8::Local<v8::Object> options = stdout_val.As<v8::Object>();
            v8::Local<v8::Context> context = isolate->context();

            options->Get(context, isolate->NewString("stdout")).ToLocal(&stdout_val);
            options->Get(context, isolate->NewString("stderr")).ToLocal(&stderr_val);

            if (stdout_val.IsEmpty() || stdout_val->IsNullOrUndefined())
                return Runtime::setError("ConsoleObject: options.stdout is required.");

            if (!is_writable(isolate, stdout_val, stdout_obj))
                return Runtime::setError("ConsoleObject: stdout must have a write() method.");
        } else
            return Runtime::setError("ConsoleObject: stdout must have a write() method.");
    }

    v8::Local<v8::Object> stderr_obj;
    if (stderr_val.IsEmpty() || stderr_val->IsNullOrUndefined() || stderr_val->IsUndefined())
        stderr_obj = stdout_obj;
    else if (!is_writable(isolate, stderr_val, stderr_obj))
        return Runtime::setError("ConsoleObject: stderr must have a write() method.");

    retVal = new ConsoleObject(isolate->m_isolate, stdout_obj, stderr_obj);
    return 0;
}

result_t util_base::debuglog(exlib::string section, obj_ptr<ConsoleObject_base>& retVal)
{
    if (section.empty())
        return Runtime::setError("ConsoleObject: section is required.");

    retVal = new ConsoleObject(section);
    return 0;
}

result_t util_base::debuglog(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<ConsoleObject_base>& retVal)
{
    if (section.empty())
        return Runtime::setError("ConsoleObject: section is required.");

    retVal = new ConsoleObject(section, fn);
    return 0;
}

result_t util_base::debug(exlib::string section, obj_ptr<ConsoleObject_base>& retVal)
{
    return debuglog(section, retVal);
}

result_t util_base::debug(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<ConsoleObject_base>& retVal)
{
    return debuglog(section, fn, retVal);
}

bool ConsoleObject::check_env()
{
    if (m_section.empty())
        return true;

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

bool ConsoleObject::first_call()
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
                    [](const v8::FunctionCallbackInfo<v8::Value>&) { });

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

void ConsoleObject::fill_prefix()
{
    if (m_section.empty() && !m_stdout.IsEmpty())
        return;

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

void ConsoleObject::_out(int32_t type, exlib::string& msg)
{
    if (!m_stdout.IsEmpty()) {
        Isolate* isolate = holder();
        v8::Local<v8::Context> context = isolate->context();

        v8::Local<v8::Object> stm = (type <= console_base::C_WARN)
            ? v8::Local<v8::Object>::New(isolate->m_isolate, m_stderr)
            : v8::Local<v8::Object>::New(isolate->m_isolate, m_stdout);

        v8::Local<v8::Value> write_fn;
        if (stm->Get(context, isolate->NewString("write")).ToLocal(&write_fn) && write_fn->IsFunction()) {
            msg.append(1, '\n');
            v8::Local<v8::Value> argv[1] = { isolate->NewString(msg) };
            write_fn.As<v8::Function>()->Call(context, stm, 1, argv).IsEmpty();
        }
    } else {
        outLog(type, msg);
    }
}

void ConsoleObject::_log(int32_t type, exlib::string fmt, OptArgs args)
{
    if (!first_call() && type > console_base::C_WARN)
        return;

    int32_t level;

    console_base::get_loglevel(level);

    if (type <= level) {
        Isolate* isolate = holder();
        exlib::string str;
        bool is_color = colors(isolate, type);

        util_format(isolate, fmt, args, is_color, str);
        str = (is_color ? m_prefix_color : m_prefix) + str;
        _out(type, str);
    }
}

void ConsoleObject::_log(int32_t type, exlib::string str, bool is_color)
{
    if (!first_call() && type > console_base::C_WARN)
        return;

    int32_t level;

    console_base::get_loglevel(level);

    if (type <= level) {
        str = (is_color ? m_prefix_color : m_prefix) + str;
        _out(type, str);
    }
}

result_t ConsoleObject::get_section(exlib::string& retVal)
{
    retVal = m_section;
    return 0;
}

result_t ConsoleObject::get_enabled(bool& retVal)
{
    retVal = check_env();
    return 0;
}

result_t ConsoleObject::_function(exlib::string fmt, OptArgs args)
{
    return debug(fmt, args);
}

result_t ConsoleObject::_function(OptArgs args)
{
    return debug(args);
}

result_t ConsoleObject::log(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_INFO, fmt, args);
    return 0;
}

result_t ConsoleObject::log(OptArgs args)
{
    _log(console_base::C_INFO, "", args);
    return 0;
}

result_t ConsoleObject::debug(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_DEBUG, fmt, args);
    return 0;
}

result_t ConsoleObject::debug(OptArgs args)
{
    _log(console_base::C_DEBUG, "", args);
    return 0;
}

result_t ConsoleObject::info(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_INFO, fmt, args);
    return 0;
}

result_t ConsoleObject::info(OptArgs args)
{
    _log(console_base::C_INFO, "", args);
    return 0;
}

result_t ConsoleObject::notice(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_NOTICE, fmt, args);
    return 0;
}

result_t ConsoleObject::notice(OptArgs args)
{
    _log(console_base::C_NOTICE, "", args);
    return 0;
}

result_t ConsoleObject::warn(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_WARN, fmt, args);
    return 0;
}

result_t ConsoleObject::warn(OptArgs args)
{
    _log(console_base::C_WARN, "", args);
    return 0;
}

result_t ConsoleObject::warning(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_WARN, fmt, args);
    return 0;
}

result_t ConsoleObject::warning(OptArgs args)
{
    _log(console_base::C_WARN, "", args);
    return 0;
}

result_t ConsoleObject::error(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_ERROR, fmt, args);
    return 0;
}

result_t ConsoleObject::error(OptArgs args)
{
    _log(console_base::C_ERROR, "", args);
    return 0;
}

result_t ConsoleObject::crit(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_CRIT, fmt, args);
    return 0;
}

result_t ConsoleObject::crit(OptArgs args)
{
    _log(console_base::C_CRIT, "", args);
    return 0;
}

result_t ConsoleObject::critical(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_CRIT, fmt, args);
    return 0;
}

result_t ConsoleObject::critical(OptArgs args)
{
    _log(console_base::C_CRIT, "", args);
    return 0;
}

result_t ConsoleObject::alert(exlib::string fmt, OptArgs args)
{
    _log(console_base::C_ALERT, fmt, args);
    return 0;
}

result_t ConsoleObject::alert(OptArgs args)
{
    _log(console_base::C_ALERT, "", args);
    return 0;
}

result_t ConsoleObject::trace(exlib::string fmt, OptArgs args)
{
    Isolate* isolate = holder();
    exlib::string str;

    util_format(isolate, fmt, args, colors(isolate, console_base::C_WARN), str);
    if (str.empty())
        str = "Trace";
    else
        str = "Trace: " + str;

    str.append(1, '\n');
    str.append(traceInfo(isolate->m_isolate, 10));

    _log(console_base::C_WARN, str);

    return 0;
}

result_t ConsoleObject::trace(OptArgs args)
{
    return trace("", args);
}

result_t ConsoleObject::dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options)
{
    exlib::string strBuffer;
    util_base::inspect(obj, options, strBuffer);

    _log(console_base::C_INFO, strBuffer, true);
    return 0;
}

result_t ConsoleObject::table(v8::Local<v8::Value> obj)
{
    return table(obj, v8::Local<v8::Array>());
}

result_t ConsoleObject::table(v8::Local<v8::Value> obj, v8::Local<v8::Array> fields)
{
    Isolate* isolate = holder();
    bool is_color = colors(isolate, console_base::C_INFO);
    exlib::string strBuffer = table_format(isolate, obj, fields, is_color, true);

    if (!m_section.empty())
        strBuffer = "\n" + strBuffer;

    _log(console_base::C_INFO, strBuffer, is_color);
    return 0;
}

static inline int64_t Ticks()
{
    return (int64_t)(uv_hrtime() / 1000);
}

result_t ConsoleObject::time(exlib::string label)
{
    m_timers[label] = Ticks();
    return 0;
}

result_t ConsoleObject::timeElapse(exlib::string label)
{
    int64_t t = Ticks() - m_timers[label];

    exlib::string strBuffer;
    char numStr[64];

    snprintf(numStr, sizeof(numStr), "%.10g", t / 1000.0);

    strBuffer.append(label);
    strBuffer.append(": ", 2);
    strBuffer.append(numStr);
    strBuffer.append("ms", 2);

    _log(console_base::C_INFO, strBuffer);
    return 0;
}

result_t ConsoleObject::timeEnd(exlib::string label)
{
    int64_t t = Ticks() - m_timers[label];

    m_timers.erase(label);

    exlib::string strBuffer;
    char numStr[64];

    snprintf(numStr, sizeof(numStr), "%.10g", t / 1000.0);

    strBuffer.append(label);
    strBuffer.append(": ", 2);
    strBuffer.append(numStr);
    strBuffer.append("ms", 2);

    _log(console_base::C_INFO, strBuffer);
    return 0;
}

}
