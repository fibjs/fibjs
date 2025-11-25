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

result_t startRecvStream(Stream_base* stream, exlib::atomic& readState);
enum ReadState {
    C_OPEN = 0,
    C_READING = 1,
    C_CLOSED = 2
};

template <typename T>
class AsyncStream : public T {
public:
    // object_base
    virtual result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        if (ev == "data")
            startRecvStream(this, m_readState);

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
        startRecvStream(this, m_readState);
        retVal = this;
        return 0;
    }

    virtual result_t pause(obj_ptr<Stream_base>& retVal)
    {
        retVal = this;
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

private:
    exlib::atomic m_readState;
};

}
