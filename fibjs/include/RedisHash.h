/*
 * RedisHash.h
 *
 *  Created on: Aug 4, 2014
 *      Author: lion
 */

#pragma once

#include "Redis.h"

namespace fibjs {

class RedisHash : public RedisHash_base {
public:
    RedisHash(exlib::string key, Redis* rdb)
        : m_rdb(rdb)
        , m_key(key)
    {
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
    // RedisHash_base
    virtual result_t set(Union_set_field field, Union_set_value value);
    virtual result_t setNX(Union_setNX_field field, Union_setNX_value value);
    virtual result_t mset(v8::Local<v8::Object> kvs);
    virtual result_t mset(OptArgs kvs);
    virtual result_t get(Union_get_field field, obj_ptr<Buffer_base>& retVal);
    virtual result_t mget(v8::Local<v8::Array> fields, obj_ptr<NArray>& retVal);
    virtual result_t mget(OptArgs fields, obj_ptr<NArray>& retVal);
    virtual result_t incr(Union_incr_field field, int64_t num, int64_t& retVal);
    virtual result_t getAll(obj_ptr<NArray>& retVal);
    virtual result_t keys(obj_ptr<NArray>& retVal);
    virtual result_t len(int32_t& retVal);
    virtual result_t exists(Union_exists_field field, bool& retVal);
    virtual result_t del(v8::Local<v8::Array> fields, int32_t& retVal);
    virtual result_t del(OptArgs fields, int32_t& retVal);


private:
    exlib::string m_key;
    obj_ptr<Redis> m_rdb;
};
}
