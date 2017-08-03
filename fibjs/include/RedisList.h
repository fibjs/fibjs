/*
 * RedisList.h
 *
 *  Created on: Aug 5, 2014
 *      Author: lion
 */

#ifndef REDISLIST_H_
#define REDISLIST_H_

#include "Redis.h"

namespace fibjs {

class RedisList : public RedisList_base {
public:
    RedisList(Buffer_base* key, Redis* rdb)
        : m_rdb(rdb)
    {
        key->toString(m_key);
    }

public:
    virtual bool enterTask(exlib::Task_base* current)
    {
        return m_rdb->enterTask(current);
    }

    virtual void enter()
    {
        m_rdb->enter();
    }

    virtual void leave(exlib::Task_base* current = NULL)
    {
        m_rdb->leave(current);
    }

public:
    // RedisList_base
    virtual result_t push(v8::Local<v8::Array> values, int32_t& retVal);
    virtual result_t push(std::vector<v8::Local<v8::Value>>& values, int32_t& retVal);
    virtual result_t pop(obj_ptr<Buffer_base>& retVal);
    virtual result_t rpush(v8::Local<v8::Array> values, int32_t& retVal);
    virtual result_t rpush(std::vector<v8::Local<v8::Value>>& values, int32_t& retVal);
    virtual result_t rpop(obj_ptr<Buffer_base>& retVal);
    virtual result_t set(int32_t index, Buffer_base* value);
    virtual result_t get(int32_t index, obj_ptr<Buffer_base>& retVal);
    virtual result_t insertBefore(Buffer_base* pivot, Buffer_base* value, int32_t& retVal);
    virtual result_t insertAfter(Buffer_base* pivot, Buffer_base* value, int32_t& retVal);
    virtual result_t remove(int32_t count, Buffer_base* value, int32_t& retVal);
    virtual result_t trim(int32_t start, int32_t stop);
    virtual result_t len(int32_t& retVal);
    virtual result_t range(int32_t start, int32_t stop, obj_ptr<List_base>& retVal);

private:
    exlib::string m_key;
    obj_ptr<Redis> m_rdb;
};
}

#endif