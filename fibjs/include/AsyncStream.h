/*
 * AsyncStream.h
 *
 *  Created on: Jun 15, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/io.h"
#include "TextEncoder.h"
#include "Buffer.h"
#include "StreamReader.h"
#include "Fiber.h"
#include <list>

namespace fibjs {

class AsyncStreamReader;

// Non-template base class holding readable mode state,
// so AsyncStreamReader can access it via m_this pointer without templates
class AsyncStreamBase {
public:
    bool m_readable = false;
    bool m_paused = false;
    bool m_ended = false;
    bool m_finished = false;
    bool m_openEmitted = false;
    exlib::spinlock m_lock;
    std::list<obj_ptr<Buffer_base>> m_pendingQueue;
    size_t m_totalBytes = 0;
    size_t m_highWaterMark = 16384;
    obj_ptr<TextDecoder> m_decoder;
    AsyncStreamReader* reader = nullptr;

    static size_t roundUpPow2(size_t n)
    {
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n++;
        return n;
    }
};

class AsyncStreamReader : public AsyncState {
public:
    AsyncStreamReader(Stream_base* pThis, AsyncStreamBase* base)
        : AsyncState(NULL)
        , m_this(pThis)
        , m_base(base)
    {
        m_isolate = pThis->holder();
        m_holder = new ValueHolder(m_this->wrap());
        next(recv);
    }

    ~AsyncStreamReader()
    {
        m_base->reader = nullptr;
        m_this->isolate_unref();
    }

    void start()
    {
        m_this->isolate_ref();
        apost(0);
    }

    ON_STATE(AsyncStreamReader, recv)
    {
        return m_this->readBuffer(-1, m_buf, next(event));
    }

    ON_STATE(AsyncStreamReader, event)
    {
        if (n == CALL_RETURN_NULL) {
            m_base->m_ended = true;
            m_this->_emit("end");
            m_this->_emit("close");
            return next();
        }

        if (m_base->m_readable) {
            // readable mode: buffer data into pending queue
            exlib::string buf;
            m_buf->toString(buf);

            bool isFull;
            m_base->m_lock.lock();
            m_base->m_pendingQueue.push_back(m_buf);
            m_base->m_totalBytes += buf.length();
            isFull = m_base->m_totalBytes >= m_base->m_highWaterMark;
            m_base->m_lock.unlock();

            m_this->_emit("readable");

            if (isFull)
                return next(drain);
            return next(recv);
        }

        // flowing mode: emit data in JS thread and wait for handler to complete
        next(data_emitted);

        Variant data;
        if (!m_base->m_decoder) {
            data = m_buf;
        } else {
            exlib::string str;
            m_base->m_decoder->decode(m_buf, false, str);
            data = str;
        }

        obj_ptr<Stream_base> stream = m_this;
        AsyncStreamReader* self = this;
        m_isolate->sync([stream, data, self]() mutable -> int32_t {
            JSFiber::EnterJsScope s;

            v8::Local<v8::Value> arg = data;
            bool retVal;
            stream->_emit("data", &arg, 1, retVal);

            self->apost(0);
            return 0;
        });

        return CALL_E_PENDDING;
    }

    ON_STATE(AsyncStreamReader, data_emitted)
    {
        // JS data handler completed, check if paused
        if (m_base->m_paused) {
            next(recv);
            return CALL_E_PENDDING;
        }
        return next(recv);
    }

    ON_STATE(AsyncStreamReader, drain)
    {
        // woken up by read() via apost(0), continue reading
        next(recv);
        return CALL_E_PENDDING;
    }

    virtual int32_t error(int32_t v)
    {
        // Treat socket close errors as normal termination
        if (v == CALL_E_BAD_FILE || v == CALL_E_INVALID_CALL
            || v == CALL_E_NETNAME_DELETED || v == CALL_E_CLOSED_SOCKET) {
            m_base->m_ended = true;
            m_this->_emit("end");
            m_this->_emit("close");
            return v;
        }

        // Emit error event in JS context with proper Error object
        obj_ptr<Stream_base> stream = m_this;
        m_isolate->sync([stream, v]() -> int32_t {
            JSFiber::EnterJsScope s;

            v8::Local<v8::Value> err = FillError(v);
            bool retVal;
            stream->_emit("error", &err, 1, retVal);
            return 0;
        });

        return v;
    }

public:

private:
    Isolate* m_isolate;
    obj_ptr<ValueHolder> m_holder;
    obj_ptr<Stream_base> m_this;
    AsyncStreamBase* m_base;
    obj_ptr<Buffer_base> m_buf;
};

template <typename T>
class AsyncStream : public T, public AsyncStreamBase {
public:
    // object_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        // Emit open event on first event registration, queued before any data events
        if (!m_openEmitted) {
            int32_t fd = -1;
            this->get_fd(fd);
            if (fd >= 0) {
                m_openEmitted = true;
                this->_emit("open", Variant(fd));
            }
        }

        if (ev == "data")
            startRecvStream();
        else if (ev == "readable") {
            m_readable = true;
            startRecvStream();
        } else if (ev == "connect")
            m_connect_event = true;

        return 0;
    }

    // Stream_base
    class AsyncReadVariant : public AsyncState {
    public:
        AsyncReadVariant(AsyncStream<T>* pThis, int32_t bytes, Variant& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_bytes(bytes)
            , m_retVal(retVal)
        {
            next(doRead);
        }

        ON_STATE(AsyncReadVariant, doRead)
        {
            return m_pThis->readBuffer(m_bytes, m_buf, next(done));
        }

        ON_STATE(AsyncReadVariant, done)
        {
            if (n == CALL_RETURN_NULL)
                return next(CALL_RETURN_NULL);

            if (!m_pThis->m_decoder) {
                m_retVal = m_buf;
            } else {
                exlib::string str;
                m_pThis->m_decoder->decode(m_buf, true, str);
                m_retVal = str;
            }
            return next(0);
        }

    private:
        AsyncStream<T>* m_pThis;
        int32_t m_bytes;
        Variant& m_retVal;
        obj_ptr<Buffer_base> m_buf;
    };

    virtual result_t read(int32_t bytes, Variant& retVal, AsyncEvent* ac)
    {
        // readable mode: return from pending queue synchronously
        if (m_readable) {
            m_lock.lock();

            if (m_pendingQueue.empty()) {
                m_lock.unlock();
                return CALL_RETURN_NULL;
            }

            // read(n) with n > 0: check if enough bytes buffered
            if (bytes > 0) {
                if ((size_t)bytes > m_totalBytes) {
                    // not enough data, auto-grow highWaterMark if needed
                    if ((size_t)bytes > m_highWaterMark)
                        m_highWaterMark = roundUpPow2(bytes);
                    m_lock.unlock();
                    return CALL_RETURN_NULL;
                }
            }

            bool wasFull = m_totalBytes >= m_highWaterMark;
            obj_ptr<Buffer_base> buf;

            if (bytes <= 0) {
                // read() with no size: merge all chunks and return
                if (m_pendingQueue.size() == 1) {
                    buf = m_pendingQueue.front();
                } else {
                    exlib::string all;
                    for (auto& b : m_pendingQueue) {
                        exlib::string s;
                        b->toString(s);
                        all.append(s);
                    }
                    buf = new Buffer(all.c_str(), all.length());
                }
                m_pendingQueue.clear();
                m_totalBytes = 0;
            } else {
                // read(n): take exactly n bytes
                exlib::string result;
                size_t remaining = bytes;
                while (remaining > 0 && !m_pendingQueue.empty()) {
                    obj_ptr<Buffer_base> front = m_pendingQueue.front();
                    exlib::string s;
                    front->toString(s);

                    if (s.length() <= remaining) {
                        result.append(s);
                        remaining -= s.length();
                        m_pendingQueue.pop_front();
                    } else {
                        result.append(s.c_str(), remaining);
                        m_pendingQueue.front() = new Buffer(
                            s.c_str() + remaining, s.length() - remaining);
                        remaining = 0;
                    }
                }
                m_totalBytes -= bytes;
                buf = new Buffer(result.c_str(), result.length());
            }

            m_lock.unlock();

            if (!this->m_decoder) {
                retVal = buf;
            } else {
                exlib::string str;
                this->m_decoder->decode(buf, false, str);
                retVal = str;
            }

            // if buffer was at/above highWaterMark, wake reader to continue
            if (wasFull && reader)
                reader->post(0);

            return 0;
        }

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        (new AsyncReadVariant(this, bytes, retVal, ac))->apost(0);
        return CALL_E_PENDDING;
    }

    virtual result_t setEncoding(exlib::string encoding, obj_ptr<Stream_base>& retVal)
    {
        this->m_encoding = encoding;
        this->m_decoder = new TextDecoder(encoding, false, false);
        retVal = this;
        return 0;
    }

    virtual result_t writeBuffer(Buffer_base* data, AsyncEvent* ac)
    {
        return static_cast<T*>(this)->writeBuffer(data, ac);
    }

    virtual result_t write(Buffer_base* data, bool& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        retVal = true;
        return static_cast<T*>(this)->writeBuffer(data, ac);
    }

    virtual result_t write(Buffer_base* data, exlib::string encoding, bool& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        retVal = true;
        return static_cast<T*>(this)->writeBuffer(data, ac);
    }

    virtual result_t write(exlib::string data, exlib::string encoding, bool& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        obj_ptr<Buffer_base> buf;
        result_t hr = Buffer_base::from(data, encoding, buf);
        if (hr < 0)
            return hr;

        retVal = true;
        return static_cast<T*>(this)->writeBuffer(buf, ac);
    }

    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
    {
        return io_base::copyStream(this, stm, bytes, retVal, ac);
    }

    virtual result_t resume(obj_ptr<Stream_base>& retVal)
    {
        if (m_paused) {
            m_paused = false;
            if (reader) {
                object_base::isolate_ref();
                reader->apost(0);
            }
        } else {
            startRecvStream();
        }
        retVal = this;
        return 0;
    }

    virtual result_t pause(obj_ptr<Stream_base>& retVal)
    {
        if (!m_paused && reader) {
            m_paused = true;
            object_base::isolate_unref();
        }
        retVal = this;
        return 0;
    }

    virtual result_t unpipe(Stream_base* destination)
    {
        return 0;
    }

    // Async state machine to emit finish + close after optional write
    class AsyncEndEmitter : public AsyncState {
    public:
        AsyncEndEmitter(AsyncStream<T>* pThis, obj_ptr<Buffer_base> data, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_data(data)
        {
            if (m_data)
                next(doWrite);
            else
                next(emitEvents);
        }

        ON_STATE(AsyncEndEmitter, doWrite)
        {
            return static_cast<T*>(m_pThis)->writeBuffer(m_data, next(emitEvents));
        }

        ON_STATE(AsyncEndEmitter, emitEvents)
        {
            m_pThis->m_finished = true;
            m_pThis->_emit("finish");
            m_pThis->_emit("close");
            return next(0);
        }

    private:
        AsyncStream<T>* m_pThis;
        obj_ptr<Buffer_base> m_data;
    };

    virtual result_t end(int32_t& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        retVal = 0;
        (new AsyncEndEmitter(this, nullptr, ac))->apost(0);
        return CALL_E_PENDDING;
    }

    virtual result_t end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        retVal = 0;
        (new AsyncEndEmitter(this, data, ac))->apost(0);
        return CALL_E_PENDDING;
    }

    virtual result_t end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        retVal = 0;
        (new AsyncEndEmitter(this, data, ac))->apost(0);
        return CALL_E_PENDDING;
    }

    virtual result_t end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        obj_ptr<Buffer_base> buf;
        result_t hr = Buffer_base::from(data, encoding, buf);
        if (hr < 0)
            return hr;

        retVal = 0;
        (new AsyncEndEmitter(this, buf, ac))->apost(0);
        return CALL_E_PENDDING;
    }

    virtual result_t getReader(obj_ptr<StreamReader_base>& retVal)
    {
        retVal = new StreamReader(this);
        return 0;
    }

    virtual result_t ref(obj_ptr<Stream_base>& retVal)
    {
        object_base::isolate_ref();
        retVal = this;
        return 0;
    }

    virtual result_t unref(obj_ptr<Stream_base>& retVal)
    {
        object_base::isolate_unref();
        retVal = this;
        return 0;
    }

public:
    // Start async read stream, called when "data" event is listened or resume() is called
    // Decrements m_state by 1, only executed once via m_recvStarted guard
    void startRecvStream()
    {
        // Ensure only executed once
        if (m_recvStarted.CompareAndSwap(0, 1) != 0)
            return;

        auto state = m_state.dec();
        if (state <= 1 && !reader) {
            reader = new AsyncStreamReader(this, this);
        }
        if (state == 0)
            reader->start();
    }

    void startConnectEvent()
    {
        if (m_connectStarted.CompareAndSwap(0, 1) != 0)
            return;

        auto state = m_state.dec();
        if (state <= 1 && !reader) {
            reader = new AsyncStreamReader(this, this);
        }
        if (state == 0)
            reader->start();
    }

    // Called when connection is established (for Socket)
    // Decrements m_state by 1
    // err: 0 = success, other = error code
    void on_connected(int32_t err = 0)
    {
        if (m_state.dec() == 0) {
            if (err == 0)
                reader->start();
            else
                reader->apost(err);
        }
    }

protected:
    exlib::string m_encoding;
    // Stream state as counter:
    // 0 = ready to start async read (all conditions met)
    // 1 = waiting for one condition (either startRecvStream or setConnected)
    // 2 = waiting for two conditions (both startRecvStream and setConnected needed)
    exlib::atomic m_state = 1; // default: 1 (only need startRecvStream for non-socket streams)
    exlib::atomic m_recvStarted = 0; // guard to ensure startRecvStream only runs once
    exlib::atomic m_connectStarted = 0; // guard to ensure startConnectEvent only runs once
    bool m_connect_event = false;
};

}
