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

    // The code-only producer entry (the socket C ABI in Socket_api.cpp): the
    // addon reports a failure code, its JS wrapper surfaces the sentinel, and
    // the boundary resolves the code from the per-thread state. It cannot
    // return a value across the C ABI, so the state write stays until 5c folds
    // these callers into the pending-exception sentinel.
    static result_t setError(result_t code)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;
        rt->m_error.clear();
        rt->m_errorTypeName.clear();

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
        bool valid() const { return !code.has_value(); }
    };

    // Non-consuming peek: the async-call capture that follows the emit still
    // needs the state on this thread.
    static ErrorDescription captureErrorDescription(result_t hr)
    {
        ErrorDescription desc;

        if (hr.is_exception()) {
            Runtime* rt = Runtime::current();
            desc.code = rt->m_code;
            desc.message = rt->m_error;
        }

        return desc;
    }

    // Restore a description captured by captureErrorDescription() on another
    // thread/fiber: the sentinel's scalar state does not travel with the value,
    // so a hop that rebuilds the JS error elsewhere re-applies it first. The
    // type name needs no copy - it rides with the value (payload view).
    static void applyErrorDescription(const ErrorDescription& desc, const ErrorPayload* payload)
    {
        if (desc.valid()) {
            Runtime* rt = Runtime::current();
            rt->m_code = desc.code;
            rt->m_error = desc.message;
            rt->m_errorTypeName = payload ? payload->error_type_name : "";
        }
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

    // Builder-landing mirror (result.h): reproduces the old setError state
    // side effects while producers migrate; 5c narrows it.
    friend void mirror_error_state(const ErrorPayload& payload);
};

} /* namespace fibjs */
