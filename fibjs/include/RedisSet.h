/*
 * RedisSet.h
 *
 *  Created on: Aug 5, 2014
 *      Author: lion
 */

#pragma once

#include "Redis.h"

namespace fibjs {

class RedisSet : public RedisSet_base {
public:
    RedisSet(exlib::string key, Redis* rdb)
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
    // RedisSet_base
    virtual result_t add(v8::Local<v8::Array> members, int32_t& retVal);
    virtual result_t add(OptArgs members, int32_t& retVal);
    virtual result_t remove(v8::Local<v8::Array> members, int32_t& retVal);
    virtual result_t remove(OptArgs members, int32_t& retVal);
    virtual result_t len(int32_t& retVal);
    virtual result_t exists(Union_exists_member member, bool& retVal);
    virtual result_t members(obj_ptr<NArray>& retVal);
    virtual result_t pop(obj_ptr<Buffer_base>& retVal);
    virtual result_t randMember(v8::Local<v8::Value>& retVal);
    virtual result_t randMember(int32_t count, v8::Local<v8::Value>& retVal);


private:
    exlib::string m_key;
    obj_ptr<Redis> m_rdb;
};
}
