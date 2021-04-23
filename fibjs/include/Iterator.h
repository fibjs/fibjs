/*
 * Iterator.h
 *
 *  Created on: Apr 21, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/Iterator.h"
#include <functional>

namespace fibjs {

class Iterator : public Iterator_base {
public:
    Iterator(object_base* obj, std::function<void(size_t, v8::Local<v8::Value>&)> proc)
        : m_obj(obj)
        , m_proc(proc)
        , m_index(0)
        , m_done(false)
    {
    }

public:
    // Iterator_base
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t next(obj_ptr<NextType>& retVal)
    {
        retVal = new NextType();

        if (!m_done)
            m_proc(m_index++, retVal->value);

        if (retVal->value.IsEmpty()) {
            retVal->value = v8::Undefined(holder()->m_isolate);
            retVal->done = true;
            m_done = true;
        } else
            retVal->done = false;

        return 0;
    }

private:
    obj_ptr<object_base> m_obj;
    std::function<void(size_t, v8::Local<v8::Value>&)> m_proc;
    size_t m_index;
    bool m_done;
};

} /* namespace fibjs */
