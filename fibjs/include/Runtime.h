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
    Isolate* m_isolate;
};

} /* namespace fibjs */
