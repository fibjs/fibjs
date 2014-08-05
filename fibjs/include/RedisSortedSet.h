/*
 * RedisSortedSet.h
 *
 *  Created on: Aug 6, 2014
 *      Author: lion
 */

#ifndef REDISSORTEDSET_H_
#define REDISSORTEDSET_H_

#include "Redis.h"

namespace fibjs
{

class RedisSortedSet: public RedisSortedSet_base
{
public:
    RedisSortedSet(const char *key, Redis *rdb) : m_key(key), m_rdb(rdb)
    {}

public:
    // RedisSortedSet_base
    virtual result_t add(v8::Local<v8::Array> sms, int32_t &retVal);
    virtual result_t add(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal);
    virtual result_t score(const char *member, std::string &retVal);
    virtual result_t incr(const char *member, int64_t num, std::string &retVal);
    virtual result_t remove(v8::Local<v8::Array> members, int32_t &retVal);
    virtual result_t remove(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal);
    virtual result_t len(int32_t &retVal);
    virtual result_t count(int32_t min, int32_t max, int32_t &retVal);
    virtual result_t range(int32_t start, int32_t stop, bool withScores, obj_ptr<List_base> &retVal);

private:
    std::string m_key;
    obj_ptr<Redis> m_rdb;
};

}

#endif