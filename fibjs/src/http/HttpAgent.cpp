/*
 * HttpAgent.cpp
 *
 * Node.js-compatible http.Agent implementation.
 * Provides a base class for connection pool management agents.
 * Currently implements the interface stubs; socket pooling to be added later.
 */

#include "object.h"
#include "ifs/HttpAgent.h"

namespace fibjs {

class HttpAgent : public HttpAgent_base {
public:
    HttpAgent()
        : m_keepAlive(false)
        , m_maxSockets(INT32_MAX)
        , m_maxTotalSockets(INT32_MAX)
        , m_maxFreeSockets(256)
        , m_defaultPort(80)
        , m_protocol("http:")
        , m_totalSocketCount(0)
    {
    }

public:
    // HttpAgent_base
    virtual result_t getName(v8::Local<v8::Object> options, exlib::string& retVal)
    {
        // Build key: protocol:hostname:port:localAddress
        Isolate* isolate = holder();
        v8::Local<v8::Context> ctx = isolate->context();
        v8::Local<v8::Value> host_v, port_v, local_v;

        options->Get(ctx, isolate->NewString("host")).ToLocal(&host_v);
        options->Get(ctx, isolate->NewString("port")).ToLocal(&port_v);
        options->Get(ctx, isolate->NewString("localAddress")).ToLocal(&local_v);

        exlib::string key = m_protocol + "//";

        if (!host_v.IsEmpty() && host_v->IsString()) {
            exlib::string h;
            GetArgumentValue(isolate, host_v, h);
            key += h;
        } else {
            key += "localhost";
        }

        key += ":";
        if (!port_v.IsEmpty() && !port_v->IsUndefined() && !port_v->IsNull()) {
            exlib::string p;
            GetArgumentValue(isolate, port_v, p);
            key += p;
        } else {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", m_defaultPort);
            key += buf;
        }

        key += ":";
        if (!local_v.IsEmpty() && local_v->IsString()) {
            exlib::string la;
            GetArgumentValue(isolate, local_v, la);
            key += la;
        }

        retVal = key;
        return 0;
    }

    virtual result_t destroy()
    {
        // No socket pool yet; nothing to destroy
        return 0;
    }

    virtual result_t get_keepAlive(bool& retVal)
    {
        retVal = m_keepAlive;
        return 0;
    }

    virtual result_t set_keepAlive(bool newVal)
    {
        m_keepAlive = newVal;
        return 0;
    }

    virtual result_t get_maxSockets(int32_t& retVal)
    {
        retVal = m_maxSockets;
        return 0;
    }

    virtual result_t set_maxSockets(int32_t newVal)
    {
        m_maxSockets = newVal;
        return 0;
    }

    virtual result_t get_maxTotalSockets(int32_t& retVal)
    {
        retVal = m_maxTotalSockets;
        return 0;
    }

    virtual result_t set_maxTotalSockets(int32_t newVal)
    {
        m_maxTotalSockets = newVal;
        return 0;
    }

    virtual result_t get_maxFreeSockets(int32_t& retVal)
    {
        retVal = m_maxFreeSockets;
        return 0;
    }

    virtual result_t set_maxFreeSockets(int32_t newVal)
    {
        m_maxFreeSockets = newVal;
        return 0;
    }

    virtual result_t get_defaultPort(int32_t& retVal)
    {
        retVal = m_defaultPort;
        return 0;
    }

    virtual result_t set_defaultPort(int32_t newVal)
    {
        m_defaultPort = newVal;
        return 0;
    }

    virtual result_t get_protocol(exlib::string& retVal)
    {
        retVal = m_protocol;
        return 0;
    }

    virtual result_t set_protocol(exlib::string newVal)
    {
        m_protocol = newVal;
        return 0;
    }

    virtual result_t get_freeSockets(v8::Local<v8::Object>& retVal)
    {
        // No pool yet; return empty object
        Isolate* isolate = holder();
        retVal = v8::Object::New(isolate->m_isolate);
        return 0;
    }

    virtual result_t get_sockets(v8::Local<v8::Object>& retVal)
    {
        // No pool yet; return empty object
        Isolate* isolate = holder();
        retVal = v8::Object::New(isolate->m_isolate);
        return 0;
    }

    virtual result_t get_totalSocketCount(int32_t& retVal)
    {
        retVal = m_totalSocketCount;
        return 0;
    }

public:
    bool m_keepAlive;
    int32_t m_maxSockets;
    int32_t m_maxTotalSockets;
    int32_t m_maxFreeSockets;
    int32_t m_defaultPort;
    exlib::string m_protocol;
    int32_t m_totalSocketCount;
};

result_t HttpAgent_base::_new(v8::Local<v8::Object> options,
    obj_ptr<HttpAgent_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpAgent> agent = new HttpAgent();

    // Parse options if provided
    if (!options.IsEmpty()) {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> ctx = isolate->context();
        v8::Local<v8::Value> v;

        if (options->Get(ctx, isolate->NewString("keepAlive")).ToLocal(&v) && v->IsBoolean())
            agent->m_keepAlive = v->BooleanValue(isolate->m_isolate);

        if (options->Get(ctx, isolate->NewString("maxSockets")).ToLocal(&v) && v->IsNumber()) {
            double d = v->NumberValue(ctx).ToChecked();
            agent->m_maxSockets = (d >= 2147483647.0) ? INT32_MAX : (int32_t)d;
        }

        if (options->Get(ctx, isolate->NewString("maxTotalSockets")).ToLocal(&v) && v->IsNumber()) {
            double d = v->NumberValue(ctx).ToChecked();
            agent->m_maxTotalSockets = (d >= 2147483647.0) ? INT32_MAX : (int32_t)d;
        }

        if (options->Get(ctx, isolate->NewString("maxFreeSockets")).ToLocal(&v) && v->IsNumber())
            agent->m_maxFreeSockets = (int32_t)v->NumberValue(ctx).ToChecked();

        if (options->Get(ctx, isolate->NewString("defaultPort")).ToLocal(&v) && v->IsNumber())
            agent->m_defaultPort = (int32_t)v->NumberValue(ctx).ToChecked();
    }

    retVal = agent;
    return 0;
}

} // namespace fibjs
