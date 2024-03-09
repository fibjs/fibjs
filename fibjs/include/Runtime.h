/*
 * Runtime.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#pragma once

#include "utils.h"

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

    static result_t setError(result_t hr)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = hr;
        return rt->m_code;
    }

    static result_t setError(result_t code, exlib::string err)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;
        rt->m_error = err;
        return rt->m_code;
    }

    static result_t setError(result_t code, const char* err)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = code;

        if (err)
            rt->m_error.assign(err);
        else
            rt->m_error.clear();

        return rt->m_code;
    }

    static result_t setError(exlib::string err)
    {
        return setError(CALL_E_EXCEPTION, err);
    }

    static result_t setError(const char* err)
    {
        return setError(CALL_E_EXCEPTION, err);
    }

    static exlib::string errMessage()
    {
        Runtime* rt = Runtime::current();
        exlib::string msg = rt->m_error;
        rt->m_error.clear();
        return msg;
    }

    static result_t errNumber()
    {
        return Runtime::current()->m_code;
    }

    Isolate* isolate()
    {
        assert(v8::Locker::IsLocked(m_isolate->m_isolate));
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

private:
    result_t m_code;
    exlib::string m_error;
    Isolate* m_isolate;
};

} /* namespace fibjs */
