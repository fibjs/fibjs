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

namespace fibjs
{

class WebSocket: public WebSocket_base
{
    EVENT_SUPPORT();

public:
    WebSocket(exlib::string url, exlib::string protocol, exlib::string origin):
        m_ac(NULL), m_url(url), m_protocol(protocol), m_origin(origin), m_masked(true),
        m_maxSize(67108864), m_readyState(ws_base::_CONNECTING)
    {}

    WebSocket(Stream_base* stream, exlib::string protocol, AsyncEvent *ac):
        m_stream(stream), m_ac(ac), m_protocol(protocol), m_masked(false),
        m_maxSize(67108864), m_readyState(ws_base::_OPEN)
    {}

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

    static result_t on_close(WebSocket* pThis);

public:
    obj_ptr<Stream_base> m_stream;
    AsyncEvent *m_ac;

    exlib::string m_url;
    exlib::string m_protocol;
    exlib::string m_origin;

    bool m_masked;
    int32_t m_maxSize;

    exlib::atomic m_readyState;

    int32_t m_code;
    exlib::string m_reason;
};

} /* namespace fibjs */
#endif /* WEBSOCKET_H_ */
