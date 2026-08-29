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
    // (err, has_value): err == 0 && has_value == true  => value available
    //                    err == 0 && has_value == false => iteration done
    //                    err < 0                        => failed, thrown to caller
    typedef std::function<void(result_t, bool)> IteratorCallback;
    typedef std::function<void(size_t, Variant&, IteratorCallback)> IteratorFunc;
    typedef std::function<void()> IteratorEndFunc;

public:
    Iterator(object_base* obj, IteratorFunc proc, IteratorEndFunc onclose = nullptr)
        : m_obj(obj)
        , m_proc(proc)
        , m_onclose(onclose)
        , m_index(0)
        , m_done(false)
    {
    }

    ~Iterator()
    {
        close();
    }

public:
    // Iterator_base
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    // Async iterator: returns itself. On the sync prototype next() returns a
    // plain {done,value}; on the async prototype it returns Promise<{done,value}>.
    // Both can be consumed by for await.
    virtual result_t symbol_asyncIterator(obj_ptr<Iterator_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t _return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
    {
        close();
        retVal = new ReturnType();
        retVal->done = true;
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
        m_proc(m_index++, retVal->value,
            [this, &retVal, ac](result_t err, bool has_value) {
                if (m_done) // already terminated by return()/close, drop the late callback
                    return;
                retVal->done = m_done = !has_value;
                ac->post(err); // err < 0 => next() throws on the JS side
            });

        return CALL_E_PENDDING;
    }

public:
    // Idempotent close: releases resources held by m_onclose; after this,
    // next() always returns {done:true}
    void close()
    {
        if (!m_done) {
            m_done = true;
            if (m_onclose)
                m_onclose();
        }
    }

private:
    obj_ptr<object_base> m_obj; // keeps the host object alive
    IteratorFunc m_proc;
    IteratorEndFunc m_onclose; // optional; HttpCollection/XmlNodeList pass nullptr
    size_t m_index;
    bool m_done;
};

} /* namespace fibjs */
