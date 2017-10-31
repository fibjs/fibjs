/*
 * Redis.cpp
 *
 *  Created on: Aug 2, 2014
 *      Author: lion
 */

#include "object.h"
#include "Redis.h"
#include "ifs/db.h"
#include "Url.h"
#include "Buffer.h"
#include "RedisHash.h"
#include "RedisList.h"
#include "RedisSet.h"
#include "RedisSortedSet.h"

namespace fibjs {

result_t db_base::openRedis(exlib::string connString,
    obj_ptr<Redis_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    const char* c_str = connString.c_str();

    exlib::string host;
    int32_t nPort = 6379;

    if (!qstrcmp(c_str, "redis:", 6)) {
        obj_ptr<Url> u = new Url();

        result_t hr = u->parse(c_str);
        if (hr < 0)
            return hr;

        host = u->m_hostname;
        c_str = host.c_str();

        if (u->m_port.length() > 0)
            nPort = atoi(u->m_port.c_str());
    }

    obj_ptr<Redis> conn = new Redis();
    retVal = conn;

    return conn->connect(c_str, nPort, ac);
}

result_t Redis::connect(const char* host, int32_t port, AsyncEvent* ac)
{
    result_t hr;

    hr = Socket_base::_new(net_base::_AF_INET, net_base::_SOCK_STREAM, m_sock);
    if (hr < 0)
        return hr;

    m_stmBuffered = new BufferedStream(m_sock);
    m_stmBuffered->set_EOL("\r\n");

    m_subMode = 0;

    return m_sock->connect(host, port, ac);
}

#define REDIS_MAX_LINE 1024
result_t Redis::_command(exlib::string& req, Variant& retVal, AsyncEvent* ac)
{
    class asyncCommand : public AsyncState {
    public:
        asyncCommand(Redis* pThis, exlib::string& req, Variant& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_req(req)
            , m_retVal(retVal)
        {
            m_subMode = pThis->m_subMode;

            m_stmBuffered = pThis->m_stmBuffered;
            set(send);
        }

        asyncCommand(Redis* pThis)
            : AsyncState(NULL)
            , m_pThis(pThis)
            , m_retVal(m_val)
        {
            m_subMode = pThis->m_subMode;

            m_stmBuffered = pThis->m_stmBuffered;
            set(read);
        }

        static int32_t send(AsyncState* pState, int32_t n)
        {
            asyncCommand* pThis = (asyncCommand*)pState;

            pThis->m_buffer = new Buffer(pThis->m_req);

            pThis->set(read);
            return pThis->m_stmBuffered->write(pThis->m_buffer, pThis);
        }

        static int32_t read(AsyncState* pState, int32_t n)
        {
            asyncCommand* pThis = (asyncCommand*)pState;

            if (pThis->m_subMode == 2)
                return pThis->done(n);

            pThis->set(read_ok);
            return pThis->m_stmBuffered->readLine(REDIS_MAX_LINE, pThis->m_strLine, pThis);
        }

        void _emit()
        {
            obj_ptr<NArray> list = (NArray*)m_val.object();

            if (list) {
                Variant vs[3];
                list->_indexed_getter(0, vs[0]);
                obj_ptr<Buffer_base> buf = Buffer_base::getInstance(vs[0].object());

                if (!buf)
                    return;

                exlib::string s;
                buf->toString(s);

                int32_t sz;

                if (!qstricmp(s.c_str(), "MESSAGE")) {
                    s = "s_";
                    sz = 2;
                } else if (!qstricmp(s.c_str(), "PMESSAGE")) {
                    s = "p_";
                    sz = 3;
                } else
                    return;

                vs[0].clear();
                list->_indexed_getter(1, vs[0]);
                obj_ptr<Buffer_base> buf1 = Buffer_base::getInstance(vs[0].object());

                if (!buf1)
                    return;

                exlib::string s1;
                buf1->toString(s1);

                s += s1;

                if (sz == 3) {
                    vs[2] = vs[0];
                    list->_indexed_getter(2, vs[0]);
                    list->_indexed_getter(3, vs[1]);
                } else
                    list->_indexed_getter(2, vs[1]);

                m_pThis->_emit(s.c_str(), vs, sz);
            }
        }

        int32_t setResult(int32_t hr = 0)
        {
            while (m_lists.size()) {
                int32_t idx = (int32_t)m_lists.size() - 1;
                m_lists[idx]->append(m_val);
                m_counts[idx]--;

                if (m_counts[idx]) {
                    m_val.clear();
                    set(read);
                    return 0;
                }

                m_val = m_lists[idx];
                m_lists.pop();
                m_counts.pop();

                hr = 0;
            }

            if (hr < 0)
                return done(hr);

            if (m_subMode == 0) {
                m_retVal = m_val;
                return done(hr);
            }

            _emit();

            m_val.clear();
            set(read);
            return 0;
        }

        static int32_t read_ok(AsyncState* pState, int32_t n)
        {
            asyncCommand* pThis = (asyncCommand*)pState;

            if (pThis->m_strLine.length() == 0)
                return CHECK_ERROR(Runtime::setError("Redis: Invalid response."));

            char ch = pThis->m_strLine[0];

            if (ch == '+') {
                pThis->m_val = new Buffer(pThis->m_strLine.substr(1));
                return pThis->setResult();
            }

            if (ch == '-')
                return CHECK_ERROR(Runtime::setError(pThis->m_strLine.substr(1)));

            if (ch == ':') {
                pThis->m_val = atoi(pThis->m_strLine.c_str() + 1);
                return pThis->setResult();
            }

            if (ch == '$') {
                int32_t sz = atoi(pThis->m_strLine.c_str() + 1);

                if (sz < 0) {
                    pThis->m_val.setNull();
                    return pThis->setResult(CALL_RETURN_NULL);
                }

                pThis->set(bulk_ok);
                return pThis->m_stmBuffered->read(sz + 2, pThis->m_buffer, pThis);
            }

            if (ch == '*') {
                int32_t sz = atoi(pThis->m_strLine.c_str() + 1);

                if (sz < 0) {
                    pThis->m_val.setNull();
                    return pThis->setResult(CALL_RETURN_NULL);
                }

                if (sz == 0) {
                    pThis->m_val = new NArray();
                    return pThis->setResult();
                }

                pThis->m_lists.append(new NArray());
                pThis->m_counts.append(sz);

                pThis->set(read);
                return 0;
            }

            return CHECK_ERROR(Runtime::setError("Redis: Invalid response."));
        }

        static int32_t bulk_ok(AsyncState* pState, int32_t n)
        {
            asyncCommand* pThis = (asyncCommand*)pState;

            if (n == CALL_RETURN_NULL)
                return pThis->setResult(CALL_RETURN_NULL);

            int32_t sz;
            pThis->m_buffer->get_length(sz);
            pThis->m_buffer->resize(sz - 2);
            pThis->m_val = pThis->m_buffer;
            pThis->m_buffer.Release();

            return pThis->setResult();
        }

        virtual int32_t error(int32_t v)
        {
            if (m_subMode == 1)
                m_pThis->_emit("suberror", (Variant*)NULL, 0);

            return v;
        }

    protected:
        obj_ptr<Redis> m_pThis;
        exlib::string m_req;
        Variant& m_retVal;
        Variant m_val;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<Buffer_base> m_buffer;
        QuickArray<obj_ptr<NArray>> m_lists;
        QuickArray<int32_t> m_counts;
        exlib::string m_strLine;
        int32_t m_subMode;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_subMode == 1) {
        (new asyncCommand(this))->post(0);
        m_subMode = 2;
    }

    return (new asyncCommand(this, req, retVal, ac))->post(0);
}

result_t Redis::command(exlib::string cmd, OptArgs args,
    v8::Local<v8::Value>& retVal)
{
    return doCommand(cmd, args, retVal);
}

result_t Redis::set(Buffer_base* key, Buffer_base* value, int64_t ttl)
{
    Variant v;

    if (ttl)
        return doCommand("SET", key, value, "PX", ttl, v);
    else
        return doCommand("SET", key, value, v);
}

result_t Redis::setNX(Buffer_base* key, Buffer_base* value, int64_t ttl)
{
    Variant v;

    if (ttl)
        return doCommand("SET", key, value, "PX", ttl, "NX", v);
    else
        return doCommand("SET", key, value, "NX", v);
}

result_t Redis::setXX(Buffer_base* key, Buffer_base* value, int64_t ttl)
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

result_t Redis::mset(OptArgs kvs)
{
    Variant v;
    return doCommand("MSET", kvs, v);
}

result_t Redis::msetNX(v8::Local<v8::Object> kvs)
{
    Variant v;
    return doCommand("MSETNX", kvs, v);
}

result_t Redis::msetNX(OptArgs kvs)
{
    Variant v;
    return doCommand("MSETNX", kvs, v);
}

result_t Redis::append(Buffer_base* key, Buffer_base* value, int32_t& retVal)
{
    return doCommand("APPEND", key, value, retVal);
}

result_t Redis::setRange(Buffer_base* key, int32_t offset, Buffer_base* value, int32_t& retVal)
{
    return doCommand("SETRANGE", key, offset, value, retVal);
}

result_t Redis::getRange(Buffer_base* key, int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    return doCommand("GETRANGE", key, start, end, retVal);
}

result_t Redis::strlen(Buffer_base* key, int32_t& retVal)
{
    return doCommand("STRLEN", key, retVal);
}

result_t Redis::bitcount(Buffer_base* key, int32_t start, int32_t end, int32_t& retVal)
{
    return doCommand("BITCOUNT", key, start, end, retVal);
}

result_t Redis::get(Buffer_base* key, obj_ptr<Buffer_base>& retVal)
{
    return doCommand("GET", key, retVal);
}

result_t Redis::mget(v8::Local<v8::Array> keys, obj_ptr<NArray>& retVal)
{
    return doCommand("MGET", keys, retVal);
}

result_t Redis::mget(OptArgs keys, obj_ptr<NArray>& retVal)
{
    return doCommand("MGET", keys, retVal);
}

result_t Redis::getset(Buffer_base* key, Buffer_base* value, obj_ptr<Buffer_base>& retVal)
{
    return doCommand("GETSET", key, value, retVal);
}

result_t Redis::decr(Buffer_base* key, int64_t num, int64_t& retVal)
{
    if (num == 1)
        return doCommand("DECR", key, retVal);
    else
        return doCommand("DECRBY", key, num, retVal);
}

result_t Redis::incr(Buffer_base* key, int64_t num, int64_t& retVal)
{
    if (num == 1)
        return doCommand("INCR", key, retVal);
    else
        return doCommand("INCRBY", key, num, retVal);
}

result_t Redis::setBit(Buffer_base* key, int32_t offset, int32_t value, int32_t& retVal)
{
    return doCommand("SETBIT", key, offset, value, retVal);
}

result_t Redis::getBit(Buffer_base* key, int32_t offset, int32_t& retVal)
{
    return doCommand("GETBIT", key, offset, retVal);
}

result_t Redis::exists(Buffer_base* key, bool& retVal)
{
    return doCommand("EXISTS", key, retVal);
}

result_t Redis::type(Buffer_base* key, exlib::string& retVal)
{
    return doCommand("TYPE", key, retVal);
}

result_t Redis::keys(exlib::string pattern, obj_ptr<NArray>& retVal)
{
    return doCommand("KEYS", pattern, retVal);
}

result_t Redis::del(v8::Local<v8::Array> keys, int32_t& retVal)
{
    return doCommand("DEL", keys, retVal);
}

result_t Redis::del(OptArgs keys, int32_t& retVal)
{
    return doCommand("DEL", keys, retVal);
}

result_t Redis::expire(Buffer_base* key, int64_t ttl, bool& retVal)
{
    return doCommand("PEXPIRE", key, ttl, retVal);
}

result_t Redis::ttl(Buffer_base* key, int64_t& retVal)
{
    return doCommand("PTTL", key, retVal);
}

result_t Redis::persist(Buffer_base* key, bool& retVal)
{
    return doCommand("PERSIST", key, retVal);
}

result_t Redis::rename(Buffer_base* key, Buffer_base* newkey)
{
    Variant v;
    return doCommand("RENAME", key, newkey, v);
}

result_t Redis::renameNX(Buffer_base* key, Buffer_base* newkey, bool& retVal)
{
    return doCommand("RENAMENX", key, newkey, retVal);
}

result_t Redis::getHash(Buffer_base* key, obj_ptr<RedisHash_base>& retVal)
{
    retVal = new RedisHash(key, this);
    return 0;
}

result_t Redis::getList(Buffer_base* key, obj_ptr<RedisList_base>& retVal)
{
    retVal = new RedisList(key, this);
    return 0;
}

result_t Redis::getSet(Buffer_base* key, obj_ptr<RedisSet_base>& retVal)
{
    retVal = new RedisSet(key, this);
    return 0;
}

result_t Redis::getSortedSet(Buffer_base* key, obj_ptr<RedisSortedSet_base>& retVal)
{
    retVal = new RedisSortedSet(key, this);
    return 0;
}

result_t Redis::dump(Buffer_base* key, obj_ptr<Buffer_base>& retVal)
{
    return doCommand("DUMP", key, retVal);
}

result_t Redis::restore(Buffer_base* key, Buffer_base* data, int64_t ttl)
{
    exlib::string strBuf;
    Variant v;

    data->toString(strBuf);
    return doCommand("RESTORE", key, ttl, strBuf, v);
}

result_t Redis::close()
{
    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_sock->ac_close();

    m_sock.Release();
    m_stmBuffered.Release();

    return 0;
}
}
