/*
 * Runtime.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "utils.h"

#ifndef RUNTIME_H_
#define RUNTIME_H_

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

    static result_t setError(result_t hr)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = hr;
        return rt->m_code;
    }

    static result_t setError(exlib::string err)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = CALL_E_EXCEPTION;
        rt->m_error = err;
        return rt->m_code;
    }

    static result_t setError(const char* err = NULL)
    {
        Runtime* rt = Runtime::current();

        rt->m_code = CALL_E_EXCEPTION;
        rt->m_error.assign(err ? err : "");
        return rt->m_code;
    }

    static const exlib::string& errMessage()
    {
        return Runtime::current()->m_error;
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

private:
    result_t m_code;
    exlib::string m_error;
    Isolate* m_isolate;
};

} /* namespace fibjs */
#endif /* RUNTIME_H_ */
