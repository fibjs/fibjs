/*
 * RedisHash.h
 *
 *  Created on: Aug 4, 2014
 *      Author: lion
 */

#ifndef REDISHASH_H_
#define REDISHASH_H_

#include "Redis.h"

namespace fibjs
{

class RedisHash: public RedisHash_base
{
public:
    RedisHash(const char *key, Redis *rdb) : m_key(key), m_rdb(rdb)
    {}

public:
    // RedisHash_base
    virtual result_t set(const char *field, const char *value);
    virtual result_t setNX(const char *field, const char *value);
    virtual result_t mset(v8::Local<v8::Array> kvs);
    virtual result_t mset(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual result_t get(const char *field, std::string &retVal);
    virtual result_t mget(v8::Local<v8::Array> fields, obj_ptr<List_base> &retVal);
    virtual result_t mget(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal);
    virtual result_t incr(const char *field, int64_t num, int64_t &retVal);
    virtual result_t getAll(obj_ptr<List_base> &retVal);
    virtual result_t keys(obj_ptr<List_base> &retVal);
    virtual result_t len(int32_t &retVal);
    virtual result_t exists(const char *field, bool &retVal);
    virtual result_t del(v8::Local<v8::Array> fields, int32_t &retVal);
    virtual result_t del(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal);

private:
    std::string m_key;
    obj_ptr<Redis> m_rdb;
};

}

#endif