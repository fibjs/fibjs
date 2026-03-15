/*
 * Runtime.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#pragma once

#include "utils.h"
#include <cstdarg>

namespace fibjs {

class Runtime {
public:
    Runtime(Isolate* isolate)
        : m_promise_error_no(0)
        , m_isolate(isolate)
    {
        RegInThread();
    }

public:
    static Runtime* current();

    static result_t setError(result_t code, exlib::string err)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;
        rt->m_error = err;
        return CALL_E_EXCEPTION;
    }

    static result_t setError(result_t code, const char* fmt, ...)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;

        if (fmt) {
            va_list args;
            va_start(args, fmt);
            char buf[1024];
            vsnprintf(buf, sizeof(buf), fmt, args);
            va_end(args);
            rt->m_error.assign(buf);
        } else {
            rt->m_error.clear();
        }

        return CALL_E_EXCEPTION;
    }

    static result_t setError(ErrorType type, result_t code, exlib::string err)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;
        rt->m_error = err;
        rt->m_errorType = type;
        return CALL_E_EXCEPTION;
    }

    static result_t setError(ErrorType type, result_t code, const char* fmt, ...)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;
        rt->m_errorType = type;

        if (fmt) {
            va_list args;
            va_start(args, fmt);
            char buf[1024];
            vsnprintf(buf, sizeof(buf), fmt, args);
            va_end(args);
            rt->m_error.assign(buf);
        } else {
            rt->m_error.clear();
        }

        return CALL_E_EXCEPTION;
    }

    static result_t setParamError(const char* name = nullptr)
    {
        Runtime* rt = Runtime::current();
        rt->m_code = CALL_E_PARAMNOTOPTIONAL;
        if (name)
            rt->m_error = name;
        else
            rt->m_error.clear();
        return CALL_E_PARAMNOTOPTIONAL;
    }

    static result_t setError(ErrorType type, exlib::string err)
    {
        return setError(type, CALL_E_EXCEPTION, err);
    }

    static result_t setError(ErrorType type, const char* err)
    {
        return setError(type, CALL_E_EXCEPTION, exlib::string(err));
    }

    static result_t setError(exlib::string err)
    {
        return setError(CALL_E_EXCEPTION, err);
    }

    static result_t setError(const char* fmt, ...)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = CALL_E_EXCEPTION;

        va_list args;
        va_start(args, fmt);
        char buf[1024];
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        rt->m_error.assign(buf);

        return CALL_E_EXCEPTION;
    }

    static exlib::string errMessage()
    {
        Runtime* rt = Runtime::current();
        exlib::string msg = rt->m_error;
        rt->m_error.clear();
        return msg;
    }

    static result_t errCode()
    {
        return Runtime::current()->m_code;
    }

    static ErrorType errType()
    {
        Runtime* rt = Runtime::current();
        ErrorType t = rt->m_errorType;
        rt->m_errorType = kError;
        return t;
    }

    static result_t errNumber()
    {
        return Runtime::current()->m_code;
    }

    Isolate* isolate()
    {
        ex_assert(v8::Locker::IsLocked(m_isolate->m_isolate));
        return m_isolate;
    }

    Isolate* safe_isolate()
    {
        return m_isolate;
    }

    static bool is_current(Isolate* isolate)
    {
        Runtime* rt = current();
        if (rt == NULL)
            return false;

        Isolate* isolate1 = rt->m_isolate;

        if (isolate1 && !v8::Locker::IsLocked(isolate1->m_isolate))
            isolate1 = NULL;
        return isolate1 == isolate;
    }

    static bool check()
    {
        return !is_current(NULL);
    }

private:
    void RegInThread();

public:
    v8::Global<v8::Array> m_promise_error;
    int32_t m_promise_error_no;

    SandBox* m_module_pending = nullptr;

private:
    result_t m_code;
    exlib::string m_error;
    ErrorType m_errorType = kError;
    Isolate* m_isolate;
};

} /* namespace fibjs */
