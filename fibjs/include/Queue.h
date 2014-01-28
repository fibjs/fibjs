/*
 * Queue.h
 *
 *  Created on: Nov 28, 2013
 *      Author: lion
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "ifs/Queue.h"
#include <list>

namespace fibjs
{

class Queue: public Queue_base
{
public:
    Queue(int32_t size) :
        m_size(size)
    {
    }

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
    // Object
    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal);

private:
    std::list<VariantEx> m_list;
    int32_t m_size;
};

} /* namespace fibjs */

#endif /* QUEUE_H_ */
