/*
 * AsyncStream
.h
 *
 *  Created on: Jun 15, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/io.h"

namespace fibjs {

void tryStartRead(Stream_base* stream, exlib::atomic& state);

template <typename T>
class AsyncStream : public T {
public:
    // object_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        if (ev == "data")
            startRecvStream();
        else if (ev == "connect")
            m_connect_event = true;

        return 0;
    }

    // Stream_base
    virtual result_t write(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
    {
        return static_cast<T*>(this)->write(data, retVal, ac);
    }

    virtual result_t write(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        obj_ptr<Buffer_base> buf;
        result_t hr = Buffer_base::from(data, encoding, buf);
        if (hr < 0)
            return hr;

        return static_cast<T*>(this)->write(buf, retVal, ac);
    }

    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
    {
        return io_base::copyStream(this, stm, bytes, retVal, ac);
    }

    virtual result_t resume(obj_ptr<Stream_base>& retVal)
    {
        startRecvStream();
        retVal = this;
        return 0;
    }

    virtual result_t pause(obj_ptr<Stream_base>& retVal)
    {
        retVal = this;
        return 0;
    }

    virtual result_t end(int32_t& retVal, AsyncEvent* ac)
    {
        retVal = 0;
        return 0;
    }

    virtual result_t end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
    {
        return static_cast<T*>(this)->write(data, retVal, ac);
    }

    virtual result_t end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
    {
        return this->write(data, encoding, retVal, ac);
    }

    virtual result_t end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac)
    {
        return this->write(data, encoding, retVal, ac);
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

        // Decrement state and try to start reading
        tryStartRead(this, m_state);
    }

    // Called when connection is established (for Socket)
    // Decrements m_state by 1
    void setConnected()
    {
        tryStartRead(this, m_state);
    }

protected:
    // Stream state as counter:
    // 0 = ready to start async read (all conditions met)
    // 1 = waiting for one condition (either startRecvStream or setConnected)
    // 2 = waiting for two conditions (both startRecvStream and setConnected needed)
    exlib::atomic m_state = 1; // default: 1 (only need startRecvStream for non-socket streams)
    exlib::atomic m_recvStarted = 0; // guard to ensure startRecvStream only runs once
    bool m_connect_event = false;
};

}
