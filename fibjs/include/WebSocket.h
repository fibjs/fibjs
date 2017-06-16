/*
 * WebSocket.h
 *
 *  Created on: Feb 16, 2016
 *      Author: lion
 */

#include "ifs/WebSocket.h"
#include "ifs/ws.h"
#include "ifs/Stream.h"

#ifndef WEBSOCKET_H_
#define WEBSOCKET_H_

namespace fibjs {

class WebSocket : public WebSocket_base {
public:
    WebSocket(exlib::string url, exlib::string protocol, exlib::string origin)
        : m_ac(NULL)
        , m_url(url)
        , m_protocol(protocol)
        , m_origin(origin)
        , m_masked(true)
        , m_maxSize(67108864)
        , m_readyState(ws_base::_CONNECTING)
    {
    }

    WebSocket(Stream_base* stream, exlib::string protocol, AsyncEvent* ac)
        : m_stream(stream)
        , m_ac(ac)
        , m_protocol(protocol)
        , m_masked(false)
        , m_maxSize(67108864)
        , m_readyState(ws_base::_OPEN)
    {
    }

public:
    // WebSocket_base
    virtual result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::on(ev, func, retVal);
    }

    virtual result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::on(map, retVal);
    }

    virtual result_t prependListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::prependListener(ev, func, retVal);
    }

    virtual result_t prependListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::prependListener(map, retVal);
    }

    virtual result_t once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::once(ev, func, retVal);
    }

    virtual result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::once(map, retVal);
    }

    virtual result_t prependOnceListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::prependOnceListener(ev, func, retVal);
    }

    virtual result_t prependOnceListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        startRecv();
        return object_base::prependOnceListener(map, retVal);
    }

    result_t setListener(exlib::string ev, v8::Local<v8::Function> func)
    {
        startRecv();
        return object_base::setListener(ev, func);
    }

    virtual result_t addListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        return on(ev, func, retVal);
    }
    virtual result_t addListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return on(map, retVal);
    }
    virtual result_t off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        return object_base::off(ev, func, retVal);
    }
    virtual result_t off(exlib::string ev, v8::Local<v8::Object>& retVal)
    {
        return object_base::off(ev, retVal);
    }
    virtual result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return object_base::off(map, retVal);
    }
    virtual result_t removeListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        return object_base::off(ev, func, retVal);
    }
    virtual result_t removeListener(exlib::string ev, v8::Local<v8::Object>& retVal)
    {
        return object_base::off(ev, retVal);
    }
    virtual result_t removeListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return object_base::off(map, retVal);
    }
    virtual result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal)
    {
        return object_base::removeAllListeners(evs, retVal);
    }
    virtual result_t setMaxListeners(int32_t n)
    {
        return object_base::setMaxListeners(n);
    }
    virtual result_t getMaxListeners(int32_t& retVal)
    {
        return object_base::getMaxListeners(retVal);
    }
    virtual result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
    {
        return object_base::listeners(ev, retVal);
    }
    virtual result_t listenerCount(exlib::string ev, int32_t& retVal)
    {
        return object_base::listenerCount(ev, retVal);
    }
    virtual result_t emit(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value>& args, bool& retVal)
    {
        return object_base::emit(ev, args, retVal);
    }
    virtual result_t eventNames(v8::Local<v8::Array>& retVal)
    {
        return object_base::eventNames(retVal);
    }

public:
    // WebSocket_base
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_protocol(exlib::string& retVal);
    virtual result_t get_origin(exlib::string& retVal);
    virtual result_t get_readyState(int32_t& retVal);
    virtual result_t close(int32_t code, exlib::string reason);
    virtual result_t send(exlib::string data);
    virtual result_t send(Buffer_base* data);

public:
    EVENT_FUNC(open);
    EVENT_FUNC(message);
    EVENT_FUNC(close);
    EVENT_FUNC(error);

public:
    void startRecv();
    void endConnect(int32_t code, exlib::string reason);
    void endConnect(SeekableStream_base* body);

public:
    obj_ptr<Stream_base> m_stream;
    AsyncEvent* m_ac;

    exlib::string m_url;
    exlib::string m_protocol;
    exlib::string m_origin;

    bool m_masked;
    int32_t m_maxSize;

    exlib::atomic m_readyState;
    exlib::atomic m_readState;

    int32_t m_code;
    exlib::string m_reason;
};

} /* namespace fibjs */
#endif /* WEBSOCKET_H_ */
