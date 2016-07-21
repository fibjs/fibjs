/*
 * BlockQueue.h
 *
 *  Created on: Nov 28, 2013
 *      Author: lion
 */

#ifndef BLOCKQUEUE_H_
#define BLOCKQUEUE_H_

#include "ifs/BlockQueue.h"
#include <list>
#include "Semaph.h"

namespace fibjs
{

class BlockQueue: public BlockQueue_base
{
public:
    BlockQueue(int32_t size) :
        m_size(size), m_semPut(size), m_semTake(0)
    {
    }

    FIBER_FREE();

public:
    // Queue_base
    virtual result_t add(v8::Local<v8::Value> e, bool &retVal);
    virtual result_t offer(v8::Local<v8::Value> e, bool &retVal);
    virtual result_t remove(v8::Local<v8::Value> &retVal);
    virtual result_t poll(v8::Local<v8::Value> &retVal);
    virtual result_t element(v8::Local<v8::Value> &retVal);
    virtual result_t peek(v8::Local<v8::Value> &retVal);
    virtual result_t clear();
    virtual result_t toArray(v8::Local<v8::Array> &retVal);
    virtual result_t get_length(int32_t &retVal);

public:
    // BlockQueue_base
    virtual result_t put(v8::Local<v8::Value> e);
    virtual result_t take(v8::Local<v8::Value> &retVal);

public:
    // Object
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value> &retVal);

private:
    std::list<VariantEx> m_list;
    int32_t m_size;
    Semaphore m_semPut;
    Semaphore m_semTake;
};

} /* namespace fibjs */

#endif /* BLOCKQUEUE_H_ */
