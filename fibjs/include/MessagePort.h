/*
 * MessagePort.h
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/MessagePort.h"
#include "MessageEvent.h"
#include "EventEmitter.h"
#include <list>

namespace fibjs {

class MessagePort : public MessagePort_base {
public:
    MessagePort()
        : m_peer(nullptr)
        , m_closed(false)
        , m_started(false)
    {
    }

    ~MessagePort()
    {
        if (m_peer) {
            m_peer->m_peer = nullptr;
            m_peer = nullptr;
        }
    }

public:
    void setPeer(MessagePort* peer)
    {
        m_peer = peer;
    }

public:
    // MessagePort_base
    virtual result_t postMessage(v8::Local<v8::Value> data);
    virtual result_t postMessage(v8::Local<v8::Value> data, v8::Local<v8::Array> transfer);
    virtual result_t start();
    virtual result_t close();
    virtual result_t ref();
    virtual result_t unref();
    virtual result_t get_onmessage(v8::Local<v8::Value>& retVal);
    virtual result_t set_onmessage(v8::Local<v8::Value> newVal);
    virtual result_t get_onmessageerror(v8::Local<v8::Value>& retVal);
    virtual result_t set_onmessageerror(v8::Local<v8::Value> newVal);

private:
    void enqueueMessage(v8::Local<v8::Value> data);
    void flush();

private:
    MessagePort* m_peer;
    bool m_closed;
    bool m_started;
    std::list<v8::Global<v8::Value>> m_queue;
    v8::Global<v8::Value> m_onmessage;
    v8::Global<v8::Value> m_onmessageerror;
};

} /* namespace fibjs */
