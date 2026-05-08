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

class Buffer_base;
class Worker;

class MessagePort : public MessagePort_base {
public:
    MessagePort()
        : m_peer(nullptr)
        , m_closed(false)
        , m_started(false)
        , m_keepalive_refed(false)
        , m_can_deliver_inline(false)
        , m_flush_pending(false)
        , m_raw_message_mode(false)
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

    void setRawMessageMode(bool rawMessageMode)
    {
        m_raw_message_mode = rawMessageMode;
    }

    void setCanDeliverInline(bool canDeliverInline)
    {
        m_can_deliver_inline = canDeliverInline;
    }

    void setMessageTarget(Worker* messageTarget)
    {
        m_message_target = messageTarget;
    }

public:
    // MessagePort_base
    virtual result_t postMessage(v8::Local<v8::Value> data);
    virtual result_t postMessage(v8::Local<v8::Value> data, v8::Local<v8::Array> transfer);
    virtual result_t start();
    virtual result_t close();
    virtual result_t ref();
    virtual result_t unref();
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func);

public:
    bool receiveMessage(v8::Local<v8::Value>& retVal);
    bool hasMessageListeners();
    bool hasPendingMessages();
    void flush();

private:
    bool canDeliverMessages();
    bool contributesKeepAlive();
    void ensureKeepAliveRef();
    void releaseKeepAliveRef();
    result_t enqueueSerializedMessage(Buffer_base* data);
    result_t emitSerializedMessage(Buffer_base* data);

private:
    MessagePort* m_peer;
    bool m_closed;
    bool m_started;
    bool m_keepalive_refed;
    bool m_can_deliver_inline;
    bool m_raw_message_mode;
    bool m_flush_pending;
    std::list<obj_ptr<Buffer_base>> m_queue;
    obj_ptr<Worker> m_message_target;
};

} /* namespace fibjs */
