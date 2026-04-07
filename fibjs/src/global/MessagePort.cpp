/*
 * MessagePort.cpp
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#include "object.h"
#include "MessagePort.h"
#include "MessageEvent.h"
#include "MessageChannel.h"
#include "EventEmitter.h"

namespace fibjs {

result_t MessageEvent_base::_new(exlib::string type, v8::Local<v8::Object> eventInitDict,
    obj_ptr<MessageEvent_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(eventInitDict);
    v8::Local<v8::Context> ctx = isolate->context();

    v8::Local<v8::Value> data = v8::Undefined(isolate->m_isolate);
    v8::Local<v8::Value> v;
    if (eventInitDict->Get(ctx, isolate->NewString("data")).ToLocal(&v) && !v->IsUndefined())
        data = v;

    obj_ptr<MessageEvent> ev = new MessageEvent(type, data);
    ev->wrap(This);
    retVal = ev;
    return 0;
}

result_t MessageChannel_base::_new(obj_ptr<MessageChannel_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<MessagePort> port1 = new MessagePort();
    obj_ptr<MessagePort> port2 = new MessagePort();

    port1->setPeer(port2);
    port2->setPeer(port1);

    obj_ptr<MessageChannel> mc = new MessageChannel(port1, port2);
    mc->wrap(This);
    retVal = mc;
    return 0;
}

result_t MessagePort::postMessage(v8::Local<v8::Value> data)
{
    return postMessage(data, v8::Local<v8::Array>());
}

result_t MessagePort::postMessage(v8::Local<v8::Value> data, v8::Local<v8::Array> transfer)
{
    if (m_closed || !m_peer || m_peer->m_closed)
        return 0;

    // Detach transferred ArrayBuffers
    if (!transfer.IsEmpty() && transfer->Length() > 0) {
        Isolate* isolate = holder();
        v8::Local<v8::Context> ctx = isolate->context();
        for (uint32_t i = 0; i < transfer->Length(); i++) {
            v8::Local<v8::Value> item;
            if (transfer->Get(ctx, i).ToLocal(&item) && item->IsArrayBuffer()) {
                item.As<v8::ArrayBuffer>()->Detach();
            }
        }
    }

    m_peer->enqueueMessage(data);
    return 0;
}

void MessagePort::enqueueMessage(v8::Local<v8::Value> data)
{
    Isolate* isolate = holder();
    m_queue.emplace_back(isolate->m_isolate, data);
    if (m_started)
        flush();
}

void MessagePort::flush()
{
    Isolate* isolate = holder();
    while (!m_queue.empty()) {
        v8::Local<v8::Value> data = m_queue.front().Get(isolate->m_isolate);
        m_queue.pop_front();

        obj_ptr<MessageEvent> ev = new MessageEvent("message", data);
        _emit("message", ev);
    }
}

result_t MessagePort::start()
{
    if (!m_started) {
        m_started = true;
        flush();
    }
    return 0;
}

result_t MessagePort::close()
{
    if (!m_closed) {
        m_closed = true;
        if (m_peer) {
            m_peer->m_peer = nullptr;
            m_peer = nullptr;
        }
        _emit("close");
    }
    return 0;
}

result_t MessagePort::ref()
{
    return 0;
}

result_t MessagePort::unref()
{
    return 0;
}

result_t MessagePort::get_onmessage(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    if (m_onmessage.IsEmpty())
        retVal = v8::Null(isolate->m_isolate);
    else
        retVal = m_onmessage.Get(isolate->m_isolate);
    return 0;
}

result_t MessagePort::set_onmessage(v8::Local<v8::Value> newVal)
{
    Isolate* isolate = holder();

    // Remove previous handler
    if (!m_onmessage.IsEmpty()) {
        v8::Local<v8::Function> prev = m_onmessage.Get(isolate->m_isolate).As<v8::Function>();
        v8::Local<v8::Object> retObj;
        off(isolate->NewString("message"), prev, retObj);
    }

    if (newVal->IsFunction()) {
        m_onmessage.Reset(isolate->m_isolate, newVal);
        v8::Local<v8::Object> retObj;
        on(isolate->NewString("message"), newVal.As<v8::Function>(), retObj);
        start();
    } else {
        m_onmessage.Reset();
    }

    return 0;
}

result_t MessagePort::get_onmessageerror(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    if (m_onmessageerror.IsEmpty())
        retVal = v8::Null(isolate->m_isolate);
    else
        retVal = m_onmessageerror.Get(isolate->m_isolate);
    return 0;
}

result_t MessagePort::set_onmessageerror(v8::Local<v8::Value> newVal)
{
    Isolate* isolate = holder();

    if (!m_onmessageerror.IsEmpty()) {
        v8::Local<v8::Function> prev = m_onmessageerror.Get(isolate->m_isolate).As<v8::Function>();
        v8::Local<v8::Object> retObj;
        off(isolate->NewString("messageerror"), prev, retObj);
    }

    if (newVal->IsFunction()) {
        m_onmessageerror.Reset(isolate->m_isolate, newVal);
        v8::Local<v8::Object> retObj;
        on(isolate->NewString("messageerror"), newVal.As<v8::Function>(), retObj);
    } else {
        m_onmessageerror.Reset();
    }

    return 0;
}

} /* namespace fibjs */
