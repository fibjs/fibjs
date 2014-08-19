/*
 * Redis.h
 *
 *  Created on: Aug 2, 2014
 *      Author: lion
 */

#ifndef REDIS_H_
#define REDIS_H_

#include "ifs/Redis.h"
#include "ifs/Socket.h"
#include "BufferedStream.h"
#include "Variant.h"
#include "QuickArray.h"
#include "Buffer.h"

namespace fibjs
{

class Redis: public Redis_base
{
public:
    // Redis_base
    virtual result_t command(const char *cmd, const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Value> &retVal);
    virtual result_t set(Buffer_base *key, Buffer_base *value, int64_t ttl);
    virtual result_t setNX(Buffer_base *key, Buffer_base *value, int64_t ttl);
    virtual result_t setXX(Buffer_base *key, Buffer_base *value, int64_t ttl);
    virtual result_t mset(v8::Local<v8::Object> kvs);
    virtual result_t mset(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual result_t msetNX(v8::Local<v8::Object> kvs);
    virtual result_t msetNX(const v8::FunctionCallbackInfo<v8::Value> &args);
    virtual result_t append(Buffer_base *key, Buffer_base *value, int32_t &retVal);
    virtual result_t setRange(Buffer_base *key, int32_t offset, Buffer_base *value, int32_t &retVal);
    virtual result_t getRange(Buffer_base *key, int32_t start, int32_t end, obj_ptr<Buffer_base> &retVal);
    virtual result_t strlen(Buffer_base *key, int32_t &retVal);
    virtual result_t bitcount(Buffer_base *key, int32_t start, int32_t end, int32_t &retVal);
    virtual result_t get(Buffer_base *key, obj_ptr<Buffer_base> &retVal);
    virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<List_base> &retVal);
    virtual result_t mget(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal);
    virtual result_t getset(Buffer_base *key, Buffer_base *value, obj_ptr<Buffer_base> &retVal);
    virtual result_t decr(Buffer_base *key, int64_t num, int64_t &retVal);
    virtual result_t incr(Buffer_base *key, int64_t num, int64_t &retVal);
    virtual result_t setBit(Buffer_base *key, int32_t offset, int32_t value, int32_t &retVal);
    virtual result_t getBit(Buffer_base *key, int32_t offset, int32_t &retVal);
    virtual result_t exists(Buffer_base *key, bool &retVal);
    virtual result_t type(Buffer_base *key, std::string &retVal);
    virtual result_t keys(const char *pattern, obj_ptr<List_base> &retVal);
    virtual result_t del(v8::Local<v8::Array> keys, int32_t &retVal);
    virtual result_t del(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal);
    virtual result_t expire(Buffer_base *key, int64_t ttl, bool &retVal);
    virtual result_t ttl(Buffer_base *key, int64_t &retVal);
    virtual result_t persist(Buffer_base *key, bool &retVal);
    virtual result_t rename(Buffer_base *key, Buffer_base *newkey);
    virtual result_t renameNX(Buffer_base *key, Buffer_base *newkey, bool &retVal);
    virtual result_t sub(Buffer_base *channel, v8::Local<v8::Function> func);
    virtual result_t sub(v8::Local<v8::Object> map);
    virtual result_t unsub(Buffer_base *channel);
    virtual result_t unsub(v8::Local<v8::Array> channels);
    virtual result_t psub(const char *pattern, v8::Local<v8::Function> func);
    virtual result_t psub(v8::Local<v8::Object> map);
    virtual result_t unpsub(const char *pattern);
    virtual result_t unpsub(v8::Local<v8::Array> patterns);
    virtual result_t pub(Buffer_base *channel, Buffer_base *message, int32_t &retVal);
    virtual result_t getHash(Buffer_base *key, obj_ptr<RedisHash_base> &retVal);
    virtual result_t getList(Buffer_base *key, obj_ptr<RedisList_base> &retVal);
    virtual result_t getSet(Buffer_base *key, obj_ptr<RedisSet_base> &retVal);
    virtual result_t getSortedSet(Buffer_base *key, obj_ptr<RedisSortedSet_base> &retVal);
    virtual result_t dump(Buffer_base *key, obj_ptr<Buffer_base> &retVal);
    virtual result_t restore(Buffer_base *key, Buffer_base *data, int64_t ttl);
    virtual result_t close();

public:
    result_t connect(const char *host, int port, exlib::AsyncEvent *ac);
    result_t _command(std::string &req, Variant &retVal, exlib::AsyncEvent *ac);
    ASYNC_MEMBERVALUE2(Redis, _command, std::string, Variant);

    class _arg
    {
    public:
        _arg(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t pos = 0) :
            m_args(args), m_pos(pos)
        {}

    public:
        const v8::FunctionCallbackInfo<v8::Value> &m_args;
        int32_t m_pos;
    };

    class _param
    {
    public:
        _param() : m_size(0)
        {}

    public:
        result_t add(std::string &str)
        {
            char numStr[64];

            m_size += sprintf(numStr, "$%d", (int32_t)str.length());
            m_params.append(numStr);

            m_params.append(str);
            m_size += (int32_t)str.length() + 4;

            return 0;
        }

        result_t add(Buffer_base *buf)
        {
            std::string str;

            buf->toString(str);
            return add(str);
        }

        result_t add(const char *v)
        {
            std::string str(v);
            return add(str);
        }

        result_t add(int64_t v)
        {
            char numStr[64];

#ifdef WIN32
            sprintf(numStr, "%I64d", v);
#else
            sprintf(numStr, "%lld", (long long)v);
#endif
            return add(numStr);
        }

        result_t add(v8::Local<v8::Array> keys)
        {
            result_t hr;
            int32_t i;

            for (i = 0; i < (int32_t)keys->Length(); i ++)
            {
                hr = add(keys->Get(i));
                if (hr < 0)
                    return hr;
            }

            return 0;
        }

        result_t add(v8::Local<v8::Object> kvs)
        {
            if (kvs->IsArray())
                return CHECK_ERROR(CALL_E_INVALIDARG);

            v8::Local<v8::Array> keys = kvs->GetPropertyNames();

            result_t hr;
            int32_t i;

            for (i = 0; i < (int32_t)keys->Length(); i ++)
            {
                v8::Local<v8::Value> v = keys->Get(i);

                hr = add(v);
                if (hr < 0)
                    return hr;

                hr = add(kvs->Get(v));
                if (hr < 0)
                    return hr;
            }

            return 0;
        }

        result_t add(_arg &args)
        {
            result_t hr;
            int32_t i;

            for (i = args.m_pos; i < (int32_t)args.m_args.Length(); i ++)
            {
                hr = add(args.m_args[i]);
                if (hr < 0)
                    return hr;
            }

            return 0;
        }

        result_t add(v8::Local<v8::Value> v)
        {
            result_t hr;
            std::string str;

            hr = GetArgumentValue(v, str);
            if (hr < 0)
                return hr;
            return add(str);
        }

        std::string str()
        {
            std::string str;
            char numStr[64];
            int32_t sz, i;
            char *p;

            sz = sprintf(numStr, "*%d\r\n", (int32_t)m_params.size() / 2);

            str.resize(sz + m_size);
            p = &str[0];

            memcpy(p, numStr, sz);
            p += sz;

            for (i = 0; i < (int32_t)m_params.size(); i ++)
            {
                std::string &str = m_params[i];
                memcpy(p, str.c_str(), str.length());
                p += str.length();
                *p++ = '\r';
                *p++ = '\n';
            }

            return str;
        }

        QuickArray<std::string> m_params;
        int32_t m_size;
    };

public:
    static result_t retValue(Variant &v, obj_ptr<Buffer_base> &retVal)
    {
        retVal = Buffer_base::getInstance(v.object());
        return retVal ? 0 : CHECK_ERROR(CALL_E_INVALID_CALL);
    }

    static result_t retValue(Variant &v, std::string &retVal)
    {
        obj_ptr<Buffer_base> v1 = Buffer_base::getInstance(v.object());
        if (v1)
            v1->toString(retVal);
        return 0;
    }

    static result_t retValue(Variant &v, obj_ptr<List_base> &retVal)
    {
        retVal = List_base::getInstance(v.object());
        return retVal ? 0 : CHECK_ERROR(CALL_E_INVALID_CALL);
    }

    static result_t retValue(Variant &v, bool &retVal)
    {
        retVal = !!(int32_t)v;
        return 0;
    }

    template<typename T>
    static result_t retValue(Variant &v, T &retVal)
    {
        retVal = v;
        return 0;
    }

    result_t chkCommand(const char *cmd)
    {
        if (!m_sock)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (m_subMode)
        {
            if (qstricmp(cmd, "SUBSCRIBE") && qstricmp(cmd, "UNSUBSCRIBE")
                    && qstricmp(cmd, "PSUBSCRIBE") && qstricmp(cmd, "PUNSUBSCRIBE")
                    && qstricmp(cmd, "CLOSE"))
                return CHECK_ERROR(CALL_E_INVALID_CALL);
        }

        return 0;
    }

    template<typename T>
    result_t doCommand(const char *cmd, T &retVal)
    {
        result_t hr;

        hr = chkCommand(cmd);
        if (hr < 0)
            return hr;

        _param ps;
        Variant v;

        hr = ps.add(cmd);
        if (hr < 0)
            return hr;

        hr = ac__command(ps.str(), v);
        if (hr < 0 || hr == CALL_RETURN_NULL)
            return hr;

        return retValue(v, retVal);
    }

    template<typename T, typename T1>
    result_t doCommand(const char *cmd, T1 &a1, T &retVal)
    {
        result_t hr;

        hr = chkCommand(cmd);
        if (hr < 0)
            return hr;

        _param ps;
        Variant v;

        hr = ps.add(cmd);
        if (hr < 0)
            return hr;

        hr = ps.add(a1);
        if (hr < 0)
            return hr;

        hr = ac__command(ps.str(), v);
        if (hr < 0 || hr == CALL_RETURN_NULL)
            return hr;

        return retValue(v, retVal);
    }

    template<typename T, typename T1, typename T2>
    result_t doCommand(const char *cmd, T1 &a1, T2 &a2, T &retVal)
    {
        result_t hr;

        hr = chkCommand(cmd);
        if (hr < 0)
            return hr;

        _param ps;
        Variant v;

        hr = ps.add(cmd);
        if (hr < 0)
            return hr;

        hr = ps.add(a1);
        if (hr < 0)
            return hr;

        hr = ps.add(a2);
        if (hr < 0)
            return hr;

        hr = ac__command(ps.str(), v);
        if (hr < 0 || hr == CALL_RETURN_NULL)
            return hr;

        return retValue(v, retVal);
    }

    template<typename T, typename T1, typename T2, typename T3>
    result_t doCommand(const char *cmd, T1 &a1, T2 &a2, T3 &a3, T &retVal)
    {
        result_t hr;

        hr = chkCommand(cmd);
        if (hr < 0)
            return hr;

        _param ps;
        Variant v;

        hr = ps.add(cmd);
        if (hr < 0)
            return hr;

        hr = ps.add(a1);
        if (hr < 0)
            return hr;

        hr = ps.add(a2);
        if (hr < 0)
            return hr;

        hr = ps.add(a3);
        if (hr < 0)
            return hr;

        hr = ac__command(ps.str(), v);
        if (hr < 0 || hr == CALL_RETURN_NULL)
            return hr;

        return retValue(v, retVal);
    }

    template<typename T, typename T1, typename T2, typename T3, typename T4>
    result_t doCommand(const char *cmd, T1 &a1, T2 &a2, T3 &a3, T4 &a4, T &retVal)
    {
        result_t hr;

        hr = chkCommand(cmd);
        if (hr < 0)
            return hr;

        _param ps;
        Variant v;

        hr = ps.add(cmd);
        if (hr < 0)
            return hr;

        hr = ps.add(a1);
        if (hr < 0)
            return hr;

        hr = ps.add(a2);
        if (hr < 0)
            return hr;

        hr = ps.add(a3);
        if (hr < 0)
            return hr;

        hr = ps.add(a4);
        if (hr < 0)
            return hr;

        hr = ac__command(ps.str(), v);
        if (hr < 0 || hr == CALL_RETURN_NULL)
            return hr;

        return retValue(v, retVal);
    }

    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    result_t doCommand(const char *cmd, T1 &a1, T2 &a2, T3 &a3, T4 &a4, T5 &a5, T &retVal)
    {
        result_t hr;

        hr = chkCommand(cmd);
        if (hr < 0)
            return hr;

        _param ps;
        Variant v;

        hr = ps.add(cmd);
        if (hr < 0)
            return hr;

        hr = ps.add(a1);
        if (hr < 0)
            return hr;

        hr = ps.add(a2);
        if (hr < 0)
            return hr;

        hr = ps.add(a3);
        if (hr < 0)
            return hr;

        hr = ps.add(a4);
        if (hr < 0)
            return hr;

        hr = ps.add(a5);
        if (hr < 0)
            return hr;

        hr = ac__command(ps.str(), v);
        if (hr < 0 || hr == CALL_RETURN_NULL)
            return hr;

        return retValue(v, retVal);
    }

public:
    result_t sub(const char *prefix, const char *cmd, v8::Local<v8::Object> &map);
    result_t unsub(const char *prefix, const char *cmd, v8::Local<v8::Array> &channels);

public:
    obj_ptr<Socket_base> m_sock;
    obj_ptr<BufferedStream_base> m_stmBuffered;
    int32_t m_subMode;
};

} /* namespace fibjs */
#endif /* REDIS_H_ */
