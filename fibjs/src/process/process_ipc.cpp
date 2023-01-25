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
        static result_t sync_emit(EventMessage* msg)
        {
            JSFiber::EnterJsScope s;
            v8::Local<v8::Object> o = v8::Local<v8::Object>::New(msg->m_ipc->m_isolate->m_isolate, msg->m_ipc->m_o);
            JSTrigger t(msg->m_ipc->m_isolate->m_isolate, o);
            v8::Local<v8::Value> v;

            result_t hr = json_base::decode(msg->m_data, v);

            delete msg;

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
            m_this->m_isolate->Ref();
            m_bs = new BufferedStream(pThis->m_stream);
            m_bs->set_EOL("\n");
            next(read);
        }

        ~asyncRead()
        {
            syncCall(m_this->m_isolate, sync_delete, m_this);
            m_this->m_isolate->Unref();
        }

    public:
        ON_STATE(asyncRead, read)
        {
            m_line.clear();
            return m_bs->readLine(-1, m_line, next(event));
        }

        ON_STATE(asyncRead, event)
        {
            if (n == CALL_RETURN_NULL)
                return next();

            syncCall(m_this->m_isolate, EventMessage::sync_emit, new EventMessage(m_this, m_line));
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
    obj_ptr<Buffer> data = new Buffer(s);
    return stream->ac_write(data);
}

result_t ChildProcess::Ipc::sync_delete(Ipc* pThis)
{
    JSFiber::EnterJsScope s;

    if (pThis->m_channel) {
        pThis->m_channel.Release();

        v8::Local<v8::Object> o = v8::Local<v8::Object>::New(pThis->m_isolate->m_isolate, pThis->m_o);
        JSTrigger t(pThis->m_isolate->m_isolate, o);
        bool r;

        t._emit("disconnect", NULL, 0, r);
    }

    delete pThis;
    return 0;
}

static void on_newListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    v8::String::Utf8Value ev(isolate->m_isolate, args[0]);

    if (!qstrcmp("message", *ev)) {
        if (!isolate->m_channel)
            isolate->m_ipc_mode = 0;
        else if (isolate->m_ipc_mode == 0) {
            new ChildProcess::Ipc(isolate, args.This(), isolate->m_channel);
            isolate->m_ipc_mode = 2;
        } else if (isolate->m_ipc_mode == 1) {
            isolate->Ref();
            isolate->m_ipc_mode = 2;
        }
    }
}

static void on_removeListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current();
    v8::String::Utf8Value ev(isolate->m_isolate, args[0]);

    if (!qstrcmp("message", *ev)) {
        JSTrigger t(isolate->m_isolate, args.This());
        v8::Local<v8::Array> a;

        t.listeners("message", a);
        if (a->Length() == 0) {
            if (!isolate->m_channel)
                isolate->m_ipc_mode = 0;
            else {
                isolate->Unref();
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
