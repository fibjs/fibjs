/*
 * Redis.cpp
 *
 *  Created on: Aug 2, 2014
 *      Author: lion
 */

#include "Redis.h"
#include "ifs/db.h"
#include "Url.h"
#include "Buffer.h"
#include "List.h"
#include "RedisHash.h"
#include "RedisList.h"
#include "RedisSet.h"
#include "RedisSortedSet.h"

namespace fibjs
{

result_t db_base::openRedis(const char *connString,
                            obj_ptr<Redis_base> &retVal, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    std::string host;
    int nPort = 6379;

    if (!qstrcmp(connString, "redis:", 6))
    {
        obj_ptr<Url> u = new Url();

        result_t hr = u->parse(connString);
        if (hr < 0)
            return hr;

        host = u->m_hostname;
        connString = host.c_str();

        if (u->m_port.length() > 0)
            nPort = atoi(u->m_port.c_str());
    }

    obj_ptr<Redis> conn = new Redis();
    retVal = conn;

    return conn->connect(connString, nPort, ac);
}

result_t Redis::connect(const char *host, int port, exlib::AsyncEvent *ac)
{
    result_t hr;

    hr = Socket_base::_new(net_base::_AF_INET, net_base::_SOCK_STREAM, m_sock);
    if (hr < 0)
        return hr;

    m_stmBuffered = new BufferedStream(m_sock);
    m_stmBuffered->set_EOL("\r\n");

    return m_sock->connect(host, port, ac);
}

#define REDIS_MAX_LINE 1024
result_t Redis::_command(std::string &req, Variant &retVal, exlib::AsyncEvent *ac)
{
    class asyncCommand: public asyncState
    {
    public:
        asyncCommand(Redis *pThis, std::string &req, Variant &retVal, exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_req(req), m_retVal(retVal)
        {
            m_stmBuffered = pThis->m_stmBuffered;
            set(send);
        }

        static int send(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;

            pThis->m_buffer = new Buffer(pThis->m_req);

            pThis->set(read);
            return pThis->m_stmBuffered->write(pThis->m_buffer, pThis);
        }

        static int read(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;

            pThis->set(read_ok);
            return pThis->m_stmBuffered->readLine(REDIS_MAX_LINE, pThis->m_strLine, pThis);
        }

        int setResult(int hr = 0)
        {
            while (m_lists.size())
            {
                int32_t idx = (int32_t)m_lists.size() - 1;
                m_lists[idx]->push(m_val);
                m_counts[idx] --;

                if (m_counts[idx])
                {
                    m_val.clear();
                    set(read);
                    return 0;
                }

                m_val = m_lists[idx];
                m_lists.pop();
                m_counts.pop();

                hr = 0;
            }

            m_retVal = m_val;
            return done(hr);
        }

        static int read_ok(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;
            char ch = pThis->m_strLine[0];

            if (ch == '+')
            {
                pThis->m_val = pThis->m_strLine.substr(1);
                return pThis->setResult();
            }

            if (ch == '-')
                return Runtime::setError(pThis->m_strLine.substr(1));

            if (ch == ':')
            {
                pThis->m_val = atoi(pThis->m_strLine.c_str() + 1);
                return pThis->setResult();
            }

            if (ch == '$')
            {
                int32_t sz = atoi(pThis->m_strLine.c_str() + 1);

                if (sz < 0)
                {
                    pThis->m_val.setNull();
                    return pThis->setResult(CALL_RETURN_NULL);
                }

                pThis->set(bulk_ok);
                pThis->m_buffer.Release();

                return pThis->m_stmBuffered->read(sz + 2, pThis->m_buffer, pThis);
            }

            if (ch == '*')
            {
                int32_t sz = atoi(pThis->m_strLine.c_str() + 1);

                if (sz < 0)
                {
                    pThis->m_val.setNull();
                    return pThis->setResult(CALL_RETURN_NULL);
                }

                if (sz == 0)
                {
                    pThis->m_val = new List();
                    return pThis->setResult();
                }

                pThis->m_lists.append(new List());
                pThis->m_counts.append(sz);

                pThis->set(read);
                return 0;
            }

            return Runtime::setError("Invalid response.");
        }

        static int bulk_ok(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;

            if (n == CALL_RETURN_NULL)
                return pThis->setResult(CALL_RETURN_NULL);

            std::string str;

            pThis->m_buffer->toString(str);
            str.resize(str.length() - 2);
            pThis->m_val = str;
            return pThis->setResult();
        }

    protected:
        obj_ptr<Redis> m_pThis;
        std::string &m_req;
        Variant &m_retVal;
        Variant m_val;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<Buffer_base> m_buffer;
        QuickArray<obj_ptr<List_base> > m_lists;
        QuickArray<int32_t> m_counts;
        std::string m_strLine;
    };

    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncCommand(this, req, retVal, ac))->post(0);
}

result_t Redis::command(const char *cmd, const v8::FunctionCallbackInfo<v8::Value> &args,
                        v8::Local<v8::Value> &retVal)
{
    _arg a(args, 1);
    return doCommand(cmd, a, retVal);
}

result_t Redis::set(Buffer_base *key, Buffer_base *value, int64_t ttl)
{
    Variant v;

    if (ttl)
        return doCommand("SET", key, value, "PX", ttl, v);
    else
        return doCommand("SET", key, value, v);
}

result_t Redis::setNX(Buffer_base *key, Buffer_base *value, int64_t ttl)
{
    Variant v;

    if (ttl)
        return doCommand("SET", key, value, "PX", ttl, "NX", v);
    else
        return doCommand("SET", key, value, "NX", v);
}

result_t Redis::setXX(Buffer_base *key, Buffer_base *value, int64_t ttl)
{
    Variant v;

    if (ttl)
        return doCommand("SET", key, value, "PX", ttl, "XX", v);
    else
        return doCommand("SET", key, value, "XX", v);
}

result_t Redis::mset(v8::Local<v8::Object> kvs)
{
    Variant v;
    return doCommand("MSET", kvs, v);
}

result_t Redis::mset(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    Variant v;
    _arg a(args);
    return doCommand("MSET", a, v);
}

result_t Redis::msetNX(v8::Local<v8::Object> kvs)
{
    Variant v;
    return doCommand("MSETNX", kvs, v);
}

result_t Redis::msetNX(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    Variant v;
    _arg a(args);
    return doCommand("MSETNX", a, v);
}

result_t Redis::append(Buffer_base *key, Buffer_base *value, int32_t &retVal)
{
    return doCommand("APPEND", key, value, retVal);
}

result_t Redis::setRange(Buffer_base *key, int32_t offset, Buffer_base *value, int32_t &retVal)
{
    return doCommand("SETRANGE", key, offset, value, retVal);
}

result_t Redis::getRange(Buffer_base *key, int32_t start, int32_t end, obj_ptr<Buffer_base> &retVal)
{
    return doCommand("GETRANGE", key, start, end, retVal);
}

result_t Redis::strlen(Buffer_base *key, int32_t &retVal)
{
    return doCommand("STRLEN", key, retVal);
}

result_t Redis::bitcount(Buffer_base *key, int32_t start, int32_t end, int32_t &retVal)
{
    return doCommand("BITCOUNT", key, start, end, retVal);
}

result_t Redis::get(Buffer_base *key, obj_ptr<Buffer_base> &retVal)
{
    return doCommand("GET", key, retVal);
}

result_t Redis::mget(v8::Local<v8::Array> keys, obj_ptr<List_base> &retVal)
{
    return doCommand("MGET", keys, retVal);
}

result_t Redis::mget(const v8::FunctionCallbackInfo<v8::Value> &args, obj_ptr<List_base> &retVal)
{
    _arg a(args);
    return doCommand("MGET", a, retVal);
}

result_t Redis::getset(Buffer_base *key, Buffer_base *value, obj_ptr<Buffer_base> &retVal)
{
    return doCommand("GETSET", key, value, retVal);
}

result_t Redis::decr(Buffer_base *key, int64_t num, int64_t &retVal)
{
    if (num == 1)
        return doCommand("DECR", key, retVal);
    else
        return doCommand("DECRBY", key, num, retVal);
}

result_t Redis::incr(Buffer_base *key, int64_t num, int64_t &retVal)
{
    if (num == 1)
        return doCommand("INCR", key, retVal);
    else
        return doCommand("INCRBY", key, num, retVal);
}

result_t Redis::setBit(Buffer_base *key, int32_t offset, int32_t value, int32_t &retVal)
{
    return doCommand("SETBIT", key, offset, value, retVal);
}

result_t Redis::getBit(Buffer_base *key, int32_t offset, int32_t &retVal)
{
    return doCommand("GETBIT", key, offset, retVal);
}

result_t Redis::exists(Buffer_base *key, bool &retVal)
{
    return doCommand("EXISTS", key, retVal);
}

result_t Redis::type(Buffer_base *key, std::string &retVal)
{
    return doCommand("TYPE", key, retVal);
}

result_t Redis::keys(const char *pattern, obj_ptr<List_base> &retVal)
{
    return doCommand("KEYS", pattern, retVal);
}

result_t Redis::del(v8::Local<v8::Array> keys, int32_t &retVal)
{
    return doCommand("DEL", keys, retVal);
}

result_t Redis::del(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal)
{
    _arg a(args);
    return doCommand("DEL", a, retVal);
}

result_t Redis::expire(Buffer_base *key, int64_t ttl, bool &retVal)
{
    return doCommand("PEXPIRE", key, ttl, retVal);
}

result_t Redis::ttl(Buffer_base *key, int64_t &retVal)
{
    return doCommand("PTTL", key, retVal);
}

result_t Redis::persist(Buffer_base *key, bool &retVal)
{
    return doCommand("PERSIST", key, retVal);
}

result_t Redis::rename(Buffer_base *key, Buffer_base *newkey)
{
    Variant v;
    return doCommand("RENAME", key, newkey, v);
}

result_t Redis::renameNX(Buffer_base *key, Buffer_base *newkey, bool &retVal)
{
    return doCommand("RENAMENX", key, newkey, retVal);
}

result_t Redis::getHash(Buffer_base *key, obj_ptr<RedisHash_base> &retVal)
{
    retVal = new RedisHash(key, this);
    return 0;
}

result_t Redis::getList(Buffer_base *key, obj_ptr<RedisList_base> &retVal)
{
    retVal = new RedisList(key, this);
    return 0;
}

result_t Redis::getSet(Buffer_base *key, obj_ptr<RedisSet_base> &retVal)
{
    retVal = new RedisSet(key, this);
    return 0;
}

result_t Redis::getSortedSet(Buffer_base *key, obj_ptr<RedisSortedSet_base> &retVal)
{
    retVal = new RedisSortedSet(key, this);
    return 0;
}

result_t Redis::dump(Buffer_base *key, obj_ptr<Buffer_base> &retVal)
{
    return doCommand("DUMP", key, retVal);
}

result_t Redis::restore(Buffer_base *key, Buffer_base *data, int64_t ttl)
{
    std::string strBuf;
    Variant v;

    data->toString(strBuf);
    return doCommand("RESTORE", key, ttl, strBuf, v);
}

result_t Redis::close()
{
    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_sock.Release();
    m_stmBuffered.Release();

    return 0;
}

}
