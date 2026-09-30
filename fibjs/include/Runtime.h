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
        : m_isolate(isolate)
    {
        RegInThread();
    }

public:
    static Runtime* current();

    // Single entry for every producer. The description is stored in the
    // per-thread state (the same-thread path) and the type name is written into
    // the payload as well, so it survives crossing a thread/fiber boundary.
    static result_t setError(const ErrorPayload& payload, const char* type_name, result_t code, exlib::string err)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;
        rt->m_error = err;
        rt->m_errorTypeName = type_name ? type_name : "";

        ErrorPayload p = payload;
        if (p.error_type_name.empty() && type_name && *type_name)
            p.error_type_name = type_name;
        setErrorPayload(p);
        return CALL_E_EXCEPTION;
    }

    static result_t setError(const ErrorPayload& payload, const char* type_name, exlib::string err)
    {
        return setError(payload, type_name, CALL_E_EXCEPTION, err);
    }

    static result_t setError(const ErrorPayload& payload, result_t code, exlib::string err)
    {
        return setError(payload, payload.error_type_name.c_str(), code, err);
    }

    // Payload-only entry: uses the result code / message / error type name
    // carried by the payload itself. A payload without an explicit result code
    // is reported as a generic CALL_E_EXCEPTION.
    static result_t setError(const ErrorPayload& payload)
    {
        return setError(payload, payload.error_type_name.c_str(),
            payload.result_code != 0 ? payload.result_code : CALL_E_EXCEPTION,
            payload.message);
    }

    static result_t setError(result_t code, exlib::string err)
    {
        return setError(ErrorPayload(), "", code, err);
    }

    static result_t setError(result_t code, const char* fmt, ...)
    {
        exlib::string err;
        if (fmt) {
            va_list args;
            va_start(args, fmt);
            char buf[1024];
            vsnprintf(buf, sizeof(buf), fmt, args);
            va_end(args);
            err.assign(buf);
        }

        return setError(ErrorPayload(), "", code, err);
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

    static result_t setError(exlib::string err)
    {
        return setError(CALL_E_EXCEPTION, err);
    }

    static result_t setError(const char* fmt, ...)
    {
        exlib::string err;
        va_list args;
        va_start(args, fmt);
        char buf[1024];
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        err.assign(buf);

        return setError(ErrorPayload(), "", CALL_E_EXCEPTION, err);
    }

    static exlib::string errMessage()
    {
        Runtime* rt = Runtime::current();
        exlib::string msg = rt->m_error;
        rt->m_error.clear();        return msg;
    }

    static result_t errCode()
    {
        return Runtime::current()->m_code;
    }

    static exlib::string errTypeName()
    {
        Runtime* rt = Runtime::current();
        exlib::string name = rt->m_errorTypeName;
        rt->m_errorTypeName.clear();
        return name;
    }

    static result_t errNumber()
    {
        return Runtime::current()->m_code;
    }

    // The description of a CALL_E_EXCEPTION (result code / message) lives in
    // per-thread state, which does NOT travel with an ErrorPayload across a
    // thread or fiber boundary. An emit path that builds the JS error on
    // another thread must therefore copy the description explicitly, or the
    // receiving thread resolves the sentinel against its own (empty) state and
    // reports "[0] Success". The error *type* needs no copy here: it rides in
    // the payload's error_type_name.
    struct ErrorDescription {
        result_t code = 0;
        exlib::string message;

        // A state code of 0 means "nothing was captured".
        bool valid() const { return code != 0; }
    };

    // Non-consuming peek: the async-call capture that follows the emit still
    // needs the state on this thread.
    static ErrorDescription captureErrorDescription(result_t hr)
    {
        ErrorDescription desc;

        if (hr == CALL_E_EXCEPTION) {
            Runtime* rt = Runtime::current();
            desc.code = rt->m_code;
            desc.message = rt->m_error;
        }

        return desc;
    }

    // Restore a description captured by captureErrorDescription() together with
    // the payload carried across the boundary. Both are applied in one step so
    // that restoring the description cannot clear the payload first.
    static void applyErrorDescription(const ErrorDescription& desc, const ErrorPayload& payload)
    {
        if (desc.valid()) {
            Runtime* rt = Runtime::current();
            rt->m_code = desc.code;
            rt->m_error = desc.message;
            rt->m_errorTypeName = payload.error_type_name;
        }

        if (!payload.empty())
            setErrorPayload(payload);
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
    SandBox* m_module_pending = nullptr;

private:
    result_t m_code;
    exlib::string m_error;
    exlib::string m_errorTypeName;
    Isolate* m_isolate;
};

} /* namespace fibjs */
