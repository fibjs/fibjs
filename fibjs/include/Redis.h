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

namespace fibjs
{

class Redis: public Redis_base
{
public:
    // Redis_base
    virtual result_t command(const char *cmd, const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Value> &retVal);
    virtual result_t close(exlib::AsyncEvent *ac);

public:
    result_t connect(const char *host, int port, exlib::AsyncEvent *ac);
    result_t _command(std::string &req, Variant &retVal, exlib::AsyncEvent *ac);
    ASYNC_MEMBERVALUE2(Redis, _command, std::string, Variant);

    class _param
    {
    public:
        _param() : m_size(0)
        {}

    public:
        result_t _add(std::string &str)
        {
            char numStr[64];

            m_size += sprintf(numStr, "$%d", (int32_t)str.length());
            m_params.append(numStr);

            m_params.append(str);
            m_size += (int32_t)str.length() + 4;

            return 0;
        }

        result_t add(const char *v)
        {
            std::string str(v);
            return _add(str);
        }

        result_t add(v8::Local<v8::Value> v)
        {
            result_t hr;
            std::string str;

            hr = GetArgumentValue(v, str);
            if (hr < 0)
                return hr;
            return _add(str);
        }

        result_t add(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t pos)
        {
            result_t hr;
            int32_t i;

            for (i = pos; i < (int32_t)args.Length(); i ++)
            {
                hr = add(args[i]);
                if (hr < 0)
                    return hr;
            }

            return 0;
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
    obj_ptr<Socket_base> m_sock;
    obj_ptr<BufferedStream_base> m_stmBuffered;
};

} /* namespace fibjs */
#endif /* REDIS_H_ */
