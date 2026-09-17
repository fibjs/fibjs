/*
 * process_ipc.cpp
 *
 *  Created on: Apr 6, 2022
 *      Author: lion
 */

#include "object.h"
#include "ifs/json.h"
#include "ifs/process.h"
#include "Fiber.h"
#include "Buffer.h"
#include "ChildProcess.h"
#include "EventEmitter.h"
#include "BufferedStream.h"

namespace fibjs {

namespace {

class ProcessIpcHoldToken : public object_base {
public:
    // has_reader：本 token 是否伴随一条 IPC 读循环（= 一个未结束的事务）。
    //   true  —— 结束策略在 ~asyncRead()（读循环真正结束）处释放；
    //   false —— 只是一条 listener pin（m_ipc_mode == 1 路径），pin 即事务，由 stop()/on_removeListener 释放。
    ProcessIpcHoldToken(Isolate* isolate, Stream_base* channel, bool has_reader)
        : m_channel(channel)
        , m_has_reader(has_reader)
    {
        holder(isolate);
        isolate_ref();
    }

    // isolate 终止时的中断：只关闭 channel 唤醒读循环，不做无条件 unref。
    // 由 Isolate::m_processIpcHolder 持有，析构发生在 isolate 拆除期，因此不加析构 unref。
    virtual result_t stop()
    {
        if (m_channel)
            m_channel->cc_close(holder());

        // 没有读循环时没有别的事务结束策略，在此结束这条 pin
        if (!m_has_reader)
            isolate_unref();

        return 0;
    }

private:
    obj_ptr<Stream_base> m_channel;
    bool m_has_reader;
};

} // namespace

ChildProcess::Ipc::Ipc(Isolate* _isolate, v8::Local<v8::Object> _o, obj_ptr<Stream_base>& stream)
    : m_isolate(_isolate)
    , m_o(_isolate->m_isolate, _o)
    , m_stream(stream)
    , m_channel(stream)
{
    class EventMessage {
    public:
        EventMessage(Ipc* _ipc, exlib::string data)
            : m_ipc(_ipc)
            , m_data(data)
        {
        }

    public:
        int emit()
        {
            JSFiber::EnterJsScope s;
            v8::Local<v8::Object> o = m_ipc->m_o.Get(m_ipc->m_isolate->m_isolate);
            JSTrigger t(m_ipc->m_isolate->m_isolate, o);
            v8::Local<v8::Value> v;

            result_t hr = json_base::decode(m_data, v);

            delete this;

            if (hr < 0)
                return hr;

            bool r;
            t._emit("message", &v, 1, r);

            return 0;
        }

    public:
        Ipc* m_ipc;
        exlib::string m_data;
    };

    class asyncRead : public AsyncState {
    public:
        asyncRead(Ipc* pThis)
            : AsyncState(NULL)
            , m_this(pThis)
        {
            m_bs = new BufferedStream(pThis->m_stream);
            m_bs->set_EOL("\n");
            next(read);
        }

        ~asyncRead()
        {
            m_this->m_isolate->sync([ipc = m_this]() -> int {
                JSFiber::EnterJsScope s;

                if (ipc->m_channel) {
                    ipc->m_channel.Release();

                    v8::Local<v8::Object> o = ipc->m_o.Get(ipc->m_isolate->m_isolate);
                    JSTrigger t(ipc->m_isolate->m_isolate, o);
                    bool r;

                    t._emit("disconnect", NULL, 0, r);
                }

                if (ipc->m_isolate->m_processIpcHolder) {
                    ipc->m_isolate->m_processIpcHolder->isolate_unref();
                    ipc->m_isolate->m_processIpcHolder.Release();
                    ipc->m_isolate->m_ipc_mode = 0;
                }

                delete ipc;

                return 0;
            });
        }

    public:
        ON_STATE(asyncRead, read)
        {
            m_line.clear();
            return m_bs->readLine(-1, m_line, next(event));
        }

        ON_STATE(asyncRead, event)
        {
            if (n == CALL_RETURN_NULL) {
                m_this->m_stream->_emit("close");
                return next();
            }

            m_this->m_isolate->sync([msg = new EventMessage(m_this, m_line)]() -> int {
                return msg->emit();
            });

            return next(read);
        }

    private:
        Ipc* m_this;
        obj_ptr<BufferedStream> m_bs;
        exlib::string m_line;
    };

    (new asyncRead(this))->apost(0);
}

result_t ChildProcess::Ipc::send(Stream_base* stream, v8::Local<v8::Value> msg)
{
    exlib::string s;
    result_t hr = json_base::encode(msg, s);
    if (hr < 0)
        return hr;

    s.append(1, '\n');
    obj_ptr<Buffer> data = new Buffer(s.c_str(), s.length());
    return stream->ac_writeBuffer(data);
}

result_t ChildProcess::Ipc::sync_delete(Ipc* pThis)
{
    JSFiber::EnterJsScope s;

    if (pThis->m_channel) {
        pThis->m_channel.Release();

        v8::Local<v8::Object> o = pThis->m_o.Get(pThis->m_isolate->m_isolate);
        JSTrigger t(pThis->m_isolate->m_isolate, o);
        bool r;

        t._emit("disconnect", NULL, 0, r);
    }

    delete pThis;
    return 0;
}

static void on_newListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::String::Utf8Value ev(isolate->m_isolate, args[0]);

    if (!qstrcmp("message", *ev)) {
        if (!isolate->m_channel)
            isolate->m_ipc_mode = 0;
        else if (isolate->m_ipc_mode == 0) {
            new ChildProcess::Ipc(isolate, args.This(), isolate->m_channel);
            // 伴随读循环：释放交给 ~asyncRead()
            isolate->m_processIpcHolder = new ProcessIpcHoldToken(isolate, isolate->m_channel, true);
            isolate->m_ipc_mode = 2;
        } else if (isolate->m_ipc_mode == 1) {
            if (!isolate->m_processIpcHolder)
                // 无读循环，只是 listener pin：由 stop()/on_removeListener 释放
                isolate->m_processIpcHolder = new ProcessIpcHoldToken(isolate, isolate->m_channel, false);
            isolate->m_ipc_mode = 2;
        }
    }
}

static void on_removeListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::String::Utf8Value ev(isolate->m_isolate, args[0]);

    if (!qstrcmp("message", *ev)) {
        JSTrigger t(isolate->m_isolate, args.This());
        v8::Local<v8::Array> a;

        t.listeners(fibjs::NewString(isolate->m_isolate, "message"), a);
        if (a->Length() == 0) {
            if (!isolate->m_channel)
                isolate->m_ipc_mode = 0;
            else {
                if (isolate->m_processIpcHolder) {
                    isolate->m_processIpcHolder->isolate_unref();
                    isolate->m_processIpcHolder.Release();
                }
                isolate->m_ipc_mode = 1;
            }
        }
    }
}

void init_process_ipc(Isolate* isolate)
{
    char buffer[16];
    size_t sz = sizeof(buffer);

    if (isolate->m_id == 1 && !uv_os_getenv("NODE_CHANNEL_FD", buffer, &sz)) {
        JSTrigger t(isolate->m_isolate, process_base::class_info().getModule(isolate));
        v8::Local<v8::Object> r;

        int fd = atoi(buffer);
        if (fd >= 0) {
            isolate->m_channel = new UVStream(fd, 1);
            if (fd < 3)
                isolate->m_stdio[fd] = isolate->m_channel;
        }

        t.on("removeListener", isolate->NewFunction("on_removeListener", on_removeListener), r);
        t.on("newListener", isolate->NewFunction("on_newListener", on_newListener), r);
    }
}

}
