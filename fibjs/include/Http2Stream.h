/*
 * Http2Stream.h
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/Http2Stream.h"
#include "AsyncStream.h"
#include "SimpleObject.h"
#include "Buffer.h"
#include <list>

namespace fibjs {

class Http2Session;

class Http2Stream : public AsyncStream<Http2Stream_base> {
    FIBER_FREE();

public:
    Http2Stream(Http2Session* session, int32_t stream_id)
        : m_session(session)
        , m_stream_id(stream_id)
    {
    }

public:
    // Http2Stream_base
    virtual result_t get_id(int32_t& retVal);
    virtual result_t get_closed(bool& retVal);
    virtual result_t get_destroyed(bool& retVal);
    virtual result_t get_headers(obj_ptr<NObject>& retVal);
    virtual result_t respond(v8::Local<v8::Object> headers);
    virtual result_t additionalHeaders(v8::Local<v8::Object> headers);
    virtual result_t sendTrailers(v8::Local<v8::Object> headers);
    virtual result_t rstStream(int32_t code);

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal);
    virtual result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t writeBuffer(Buffer_base* data, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);

public:
    // Called by Http2Session when DATA frames arrive
    void onData(const uint8_t* data, size_t len);
    // Called by Http2Session when headers are received
    void onHeaders(obj_ptr<NObject> headers);
    // Called by Http2Session when stream is closed
    void onClose(uint32_t error_code);
    // Called by Http2Session when trailers are received
    void onTrailers(obj_ptr<NObject> headers);

    // Wait for response headers to arrive (async, blocks fiber)
    result_t waitHeaders(AsyncEvent* ac);

public:
    Http2Session* m_session;
    int32_t m_stream_id;

    bool m_closed = false;
    bool m_destroyed = false;

    obj_ptr<NObject> m_headers;
    exlib::Event m_headers_event;

    // Read buffer management (receiving DATA frames)
    exlib::Locker m_read_lock;
    exlib::Locker m_write_lock;
    exlib::spinlock m_recv_lock;
    std::list<obj_ptr<Buffer_base>> m_recv_queue;
    exlib::Event m_recv_event;
    bool m_recv_end = false;

    // Send buffer management (outgoing body DATA frames)
    exlib::spinlock m_send_lock;
    exlib::string m_send_data;
    size_t m_send_offset = 0;
    bool m_send_end = false;
};

} /* namespace fibjs */
