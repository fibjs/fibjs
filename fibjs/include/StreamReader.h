/*
 * StreamReader.h
 *
 *  Created on: Feb 28, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/StreamReader.h"
#include "ifs/Stream.h"
#include "Buffer.h"
#include "Fiber.h"

namespace fibjs {

class StreamReader : public StreamReader_base {
public:
    StreamReader(Stream_base* stream)
        : m_stream(stream)
    {
    }

public:
    // StreamReader_base
    class AsyncRead : public AsyncState {
    public:
        AsyncRead(StreamReader* reader, obj_ptr<ReadType>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_reader(reader)
            , m_retVal(retVal)
        {
            next(doRead);
        }

        ON_STATE(AsyncRead, doRead)
        {
            return m_reader->m_stream->readBuffer(-1, m_buf, next(done));
        }

        ON_STATE(AsyncRead, done)
        {
            m_retVal = new ReadType();
            if (n == CALL_RETURN_NULL || !m_buf) {
                m_retVal->done = true;
                m_retVal->value = new Buffer();
                m_reader->resolve_closed();
            } else {
                m_retVal->done = false;
                m_retVal->value = m_buf;
            }
            return next(0);
        }

    private:
        obj_ptr<StreamReader> m_reader;
        obj_ptr<ReadType>& m_retVal;
        obj_ptr<Buffer_base> m_buf;
    };

    virtual result_t read(obj_ptr<ReadType>& retVal, AsyncEvent* ac)
    {
        if (!m_stream)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new AsyncRead(this, retVal, ac))->post(0);
    }

    virtual result_t releaseLock()
    {
        m_stream.Release();
        return 0;
    }

    class AsyncCancel : public AsyncState {
    public:
        AsyncCancel(StreamReader* reader, AsyncEvent* ac)
            : AsyncState(ac)
            , m_reader(reader)
        {
            next(doClose);
        }

        ON_STATE(AsyncCancel, doClose)
        {
            if (!m_reader->m_stream)
                return next(0);

            return m_reader->m_stream->close(next(done));
        }

        ON_STATE(AsyncCancel, done)
        {
            m_reader->m_stream.Release();
            m_reader->resolve_closed();
            return next(0);
        }

    private:
        obj_ptr<StreamReader> m_reader;
    };

    virtual result_t cancel(exlib::string reason, AsyncEvent* ac)
    {
        if (!m_stream)
            return 0;

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new AsyncCancel(this, ac))->post(0);
    }

    virtual result_t get_closed(v8::Local<v8::Promise>& retVal)
    {
        Isolate* isolate = holder();
        if (!isolate)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        if (m_closed_promise.IsEmpty()) {
            v8::Local<v8::Context> context = isolate->context();
            auto resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
            m_closed_resolver.Reset(isolate->m_isolate, resolver);
            m_closed_promise.Reset(isolate->m_isolate, resolver->GetPromise());
        }

        retVal = m_closed_promise.Get(isolate->m_isolate);
        return 0;
    }

private:
    void resolve_closed()
    {
        Isolate* isolate = holder();
        if (!isolate)
            return;

        obj_ptr<StreamReader> pThis(this);
        isolate->sync([pThis, isolate]() -> int32_t {
            JSFiber::EnterJsScope s;

            if (!pThis->m_closed_resolver.IsEmpty()) {
                v8::Local<v8::Promise::Resolver> resolver = pThis->m_closed_resolver.Get(isolate->m_isolate);
                v8::Local<v8::Context> context = isolate->context();
                resolver->Resolve(context, v8::Undefined(isolate->m_isolate)).Check();
                pThis->m_closed_resolver.Reset();
            }
            return 0;
        });
    }

private:
    obj_ptr<Stream_base> m_stream;
    v8::Global<v8::Promise::Resolver> m_closed_resolver;
    v8::Global<v8::Promise> m_closed_promise;
};

} // namespace fibjs
