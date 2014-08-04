/*
 * RedisList.h
 *
 *  Created on: Aug 5, 2014
 *      Author: lion
 */

#ifndef REDISLIST_H_
#define REDISLIST_H_

#include "Redis.h"

namespace fibjs
{

class RedisList: public RedisList_base
{
public:
    RedisList(const char *key, Redis *rdb) : m_key(key), m_rdb(rdb)
    {}

public:
    // RedisList_base
    virtual result_t push(v8::Local<v8::Array> values, int32_t &retVal);
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal);
    virtual result_t pop(std::string &retVal);
    virtual result_t rpush(v8::Local<v8::Array> values, int32_t &retVal);
    virtual result_t rpush(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal);
    virtual result_t rpop(std::string &retVal);
    virtual result_t set(int32_t index, const char *value);
    virtual result_t get(int32_t index, std::string &retVal);
    virtual result_t insertBefore(const char *pivot, const char *value, int32_t &retVal);
    virtual result_t insertAfter(const char *pivot, const char *value, int32_t &retVal);
    virtual result_t remove(int32_t count, const char *value, int32_t &retVal);
    virtual result_t trim(int32_t start, int32_t stop);
    virtual result_t len(int32_t &retVal);
    virtual result_t range(int32_t start, int32_t stop, obj_ptr<List_base> &retVal);

private:
    std::string m_key;
    obj_ptr<Redis> m_rdb;
};

}

#endif