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
#include "Fiber.h"
#include "ifs/v8.h"
#include "Worker.h"

namespace fibjs {

namespace {

class MessagePortKeepAliveToken : public object_base {
public:
    MessagePortKeepAliveToken(MessagePort* port)
        : m_port(port)
    {
        holder(port->holder());
        isolate_ref();
    }

    // isolate 终止时的中断：只关闭 port（其生命周期终点即事务终点）
    virtual result_t stop()
    {
        if (m_port)
            return m_port->close();

        return 0;
    }

    ~MessagePortKeepAliveToken()
    {
        // 幂等兜底：正常路径由 MessagePort::releaseKeepAliveRef() 释放（电平幂等，重复无害）
        isolate_unref();
    }

private:
    obj_ptr<MessagePort> m_port;
};

} // namespace

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

    port1->holder(Isolate::current());
    port2->holder(Isolate::current());

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

    obj_ptr<MessagePort> peer = m_peer;
    bool hasTransfer = !transfer.IsEmpty() && transfer->Length() > 0;
    bool isCrossIsolatePort = m_can_deliver_inline;

    obj_ptr<Buffer_base> serialized;
    result_t hr = v8_base::serialize(data, serialized);
    if (hr < 0)
        return hr;

    if (hasTransfer) {
        Isolate* isolate = holder();
        v8::Local<v8::Context> ctx = isolate->context();
        for (uint32_t i = 0; i < transfer->Length(); i++) {
            v8::Local<v8::Value> item;
            if (transfer->Get(ctx, i).ToLocal(&item) && item->IsArrayBuffer())
                item.As<v8::ArrayBuffer>()->Detach();
        }
    }

    if (!isCrossIsolatePort)
        return peer->enqueueSerializedMessage(serialized);

    peer->get_holder()->sync([peer, serialized]() -> int {
        JSFiber::EnterJsScope s;
        return peer->enqueueSerializedMessage(serialized);
    });

    return 0;
}

bool MessagePort::canDeliverMessages()
{
    if (!m_raw_message_mode || !m_message_target)
        return true;

    Isolate* isolate = holder();
    int32_t listenerCount = 0;
    m_message_target->listenerCount(isolate->NewString("message"), listenerCount);
    return listenerCount > 0;
}

bool MessagePort::contributesKeepAlive()
{
    if (!m_raw_message_mode || !m_message_target)
        return true;

    return false;
}

void MessagePort::ensureKeepAliveRef()
{
    if (!contributesKeepAlive() || m_keepalive_refed)
        return;

    m_keepalive_holder = new MessagePortKeepAliveToken(this);
    m_keepalive_refed = true;
}

void MessagePort::releaseKeepAliveRef()
{
    if (!m_keepalive_refed)
        return;

    if (m_keepalive_holder) {
        m_keepalive_holder->isolate_unref();
        m_keepalive_holder.Release();
    }
    m_keepalive_refed = false;
}

result_t MessagePort::enqueueSerializedMessage(Buffer_base* data)
{
    // 只有「投递已排程」才持有 isolate：未 start 的 port 不 pin isolate
    //（对齐 Node：未启动的端口不维持事件循环）。
    // 消息仍留在队列里，后续 start() 会走 flush() 正常投递，不丢消息。
    if (m_started && canDeliverMessages())
        isolate_ref();

    m_queue.emplace_back(data);
    if (m_started)
        flush();

    return 0;
}

result_t MessagePort::emitSerializedMessage(Buffer_base* data)
{
    v8::Local<v8::Value> value;
    result_t hr = v8_base::deserialize(data, value);
    if (hr >= 0) {
        if (m_raw_message_mode) {
            if (m_message_target)
                return m_message_target->emitMessage(value);

            v8::Local<v8::Value> args[] = { value };
            bool retVal;
            return _emit("message", args, 1, retVal);
        }

        obj_ptr<MessageEvent> ev = new MessageEvent("message", value);
        return _emit("message", ev);
    }

    return hr;
}

void MessagePort::flush()
{
    if (!m_started || m_flush_pending || m_queue.empty())
        return;

    if (!canDeliverMessages()) {
        return;
    }

    m_flush_pending = true;
    Isolate* isolate = holder();
    isolate->sync([this]() -> int {
        JSFiber::EnterJsScope s;

        while (!m_queue.empty()) {
            obj_ptr<Buffer_base> data = m_queue.front();
            m_queue.pop_front();

            emitSerializedMessage(data);
        }

        isolate_unref();
        m_flush_pending = false;

        return 0;
    });
}

bool MessagePort::receiveMessage(v8::Local<v8::Value>& retVal)
{
    if (m_queue.empty())
        return false;

    obj_ptr<Buffer_base> data = m_queue.front();
    m_queue.pop_front();

    if (m_queue.empty())
        isolate_unref();

    return v8_base::deserialize(data, retVal) >= 0;
}

bool MessagePort::hasMessageListeners()
{
    int32_t listenerCount = 0;
    JSTrigger(this).listenerCount("message", listenerCount);
    return listenerCount > 0;
}

bool MessagePort::hasPendingMessages()
{
    return !m_queue.empty();
}

result_t MessagePort::start()
{
    if (m_closed)
        return 0;

    if (!m_started) {
        m_started = true;
        ensureKeepAliveRef();
        flush();
    }
    return 0;
}

result_t MessagePort::close()
{
    if (!m_closed) {
        m_closed = true;
        releaseKeepAliveRef();

        // D2：关闭不丢已入队消息，同时保留「立即进入 closed 状态 + 立即发 close 事件」的既有语义，
        // 因此允许 close 之后继续把 close 前已入队的消息投递完。
        //   1) 仍有投递路径（已 start 且有监听路径）：排程一次 flush，
        //      hold 由 flush 末尾的 isolate_unref() 自主回收；
        //   2) 完全没有投递路径：丢弃残留队列并立即回收，否则会形成永久 pin。
        if (!m_queue.empty() && !m_flush_pending) {
            if (m_started && canDeliverMessages())
                flush();
            else {
                m_queue.clear();
                isolate_unref();
            }
        }

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
    ensureKeepAliveRef();
    return 0;
}

result_t MessagePort::unref()
{
    releaseKeepAliveRef();
    return 0;
}

result_t MessagePort::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
{
    if (type == "newListener" && ev == "message") {
        start();
    }

    return 0;
}

} /* namespace fibjs */
