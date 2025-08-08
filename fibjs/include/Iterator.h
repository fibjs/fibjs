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
    typedef std::function<void(bool)> IteratorCallback;
    typedef std::function<void(size_t, Variant&, IteratorCallback)> IteratorFunc;

public:
    Iterator(object_base* obj, IteratorFunc proc)
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

    virtual result_t next(obj_ptr<NextType>& retVal, AsyncEvent* ac)
    {
        if (m_done) {
            retVal = new NextType();
            retVal->done = true;
            return 0;
        }

        if (ac->isSync()) {
            return CALL_E_NOSYNC;
        }

        retVal = new NextType();
        m_proc(m_index++, retVal->value, [this, &retVal, ac](bool done) {
            retVal->done = m_done = !done;
            ac->post(0);
        });

        return CALL_E_PENDDING;
    }

private:
    obj_ptr<object_base> m_obj;
    IteratorFunc m_proc;
    size_t m_index;
    bool m_done;
};

} /* namespace fibjs */
