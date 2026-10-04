/*
 * console_log.cpp
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#include "console.h"
#include "Isolate.h"

namespace fibjs {

static int32_t s_loglevel = console_base::C_NOTSET;
std_logger* s_std;

#define MAX_LOGGER 10
static logger* s_logs[MAX_LOGGER];

class logger_initer {
public:
    logger_initer()
    {
        s_std = new std_logger;
    }
} s_logger_initer;

result_t addLogger(logger* lgr)
{
    int32_t n = 0;

    for (n = 0; n < MAX_LOGGER && s_logs[n]; n++)
        ;

    if (n >= MAX_LOGGER)
        return CHECK_ERROR(Runtime::setError("console: Too many items."));

    s_logs[n] = lgr;
    return 0;
}

void outLog(int32_t priority, exlib::string msg)
{
    if (priority > s_loglevel)
        return;

    int32_t i;

    for (i = 0; i < MAX_LOGGER; i++) {
        logger* lgr = s_logs[i];

        if (lgr)
            lgr->log(priority, msg);
        else
            break;
    }

    if (i == 0)
        s_std->log(priority, msg);
}

void errorLog(exlib::string msg)
{
    outLog(console_base::C_ERROR, msg);
}

void flushLog()
{
    int32_t i;

    for (i = 0; i < MAX_LOGGER; i++) {
        logger* lgr = s_logs[i];

        if (lgr)
            lgr->flush();
        else
            break;
    }

    s_std->flush();

    Isolate* isolate = Isolate::main();
    if (isolate) {
        if (isolate->m_stdout)
            isolate->m_stdout->cc_flush();
        if (isolate->m_stderr)
            isolate->m_stderr->cc_flush();
    }
}

result_t console_base::get_loglevel(int32_t& retVal)
{
    retVal = s_loglevel;
    return 0;
}

result_t console_base::set_loglevel(int32_t newVal)
{
    s_loglevel = newVal;
    return 0;
}

result_t console_base::add(exlib::string type)
{
    Isolate* isolate = Isolate::current();
    if (isolate->m_id > 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "console.add is not available in worker threads."));

    v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
    o->Set(isolate->context(), isolate->NewString("type", 4), isolate->NewString(type)).IsJust();
    return add(o);
}

static result_t console_add_object(v8::Local<v8::Object> cfg)
{
    Isolate* isolate = Isolate::current();
    if (isolate->m_id > 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "console.add is not available in worker threads."));

    JSValue type;

    type = cfg->Get(isolate->context(), isolate->NewString("type", 4));
    if (IsEmpty(type))
        return CHECK_ERROR(Runtime::setError("console: Missing log type."));

    v8::String::Utf8Value s(isolate->m_isolate, type);
    if (!*s)
        return CHECK_ERROR(Runtime::setError("console: Unknown log type."));

    logger* lgr;

    if (!qstrcmp(*s, "console"))
        lgr = new std_logger();

#ifdef _WIN32
    else if (!qstrcmp(*s, "event"))
        lgr = new event_logger();
#else
    else if (!qstrcmp(*s, "syslog"))
        lgr = new sys_logger();
#endif

#ifdef Darwin
    else if (!qstrcmp(*s, "nslog"))
        lgr = new nslog_logger();
#endif

    else if (!qstrcmp(*s, "file"))
        lgr = new file_logger();
    else
        return CHECK_ERROR(Runtime::setError("console: Unknown log type."));

    if (lgr) {
        result_t hr = lgr->config(isolate, cfg);
        if (hr < 0) {
            lgr->stop();
            return hr;
        }

        hr = addLogger(lgr);
        if (hr < 0)
            return hr;
    }

    return 0;
}

static result_t console_add_array(v8::Local<v8::Array> cfg)
{
    Isolate* isolate = Isolate::current();
    if (isolate->m_id > 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "console.add is not available in worker threads."));

    int32_t sz = cfg->Length();
    int32_t i;
    result_t hr;
    v8::Local<v8::Context> context = isolate->context();

    for (i = 0; i < sz; i++) {
        JSValue v = cfg->Get(context, i);

        // a failing element read -- a getter that throws -- is the JavaScript
        // error of that getter, not a type error of the argument
        if (v.IsEmpty())
            return CALL_E_JAVASCRIPT;

        exlib::string s;

        hr = GetArgumentValue(isolate, v, s, true);
        if (hr != CALL_E_TYPEMISMATCH)
            hr = console_base::add(s);
        else {
            v8::Local<v8::Object> o;
            hr = GetArgumentValue(isolate, v, o, true);
            if (hr != CALL_E_TYPEMISMATCH)
                hr = console_base::add(o);
            else
                return CALL_E_TYPEMISMATCH;
        }
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t console_base::use(exlib::string type)
{
    return add(type);
}

result_t console_base::add(Union_add_cfg cfg)
{
    if (std::holds_alternative<v8::Local<v8::Array>>(cfg))
        return console_add_array(std::get<v8::Local<v8::Array>>(cfg));

    return console_add_object(std::get<v8::Local<v8::Object>>(cfg));
}

result_t console_base::use(Union_use_cfg cfg)
{
    return add(cfg);
}

result_t console_base::reset()
{
    Isolate* isolate = Isolate::current();
    if (isolate->m_id > 1)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "console.reset is not available in worker threads."));

    int32_t i;

    for (i = 0; i < MAX_LOGGER; i++) {
        logger* lgr = s_logs[i];

        if (lgr) {
            lgr->stop();
            s_logs[i] = 0;
        } else
            break;
    }

    return 0;
}
}
