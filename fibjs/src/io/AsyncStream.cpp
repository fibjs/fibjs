/*
 * AsyncStream.cpp
 *
 *  Created on: Jun 15, 2025
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "AsyncStream.h"

namespace fibjs {

result_t startRecvStream(Stream_base* stream, exlib::atomic& readState)
{
    class asyncRead : public AsyncState {
    public:
        asyncRead(Stream_base* pThis)
            : AsyncState(NULL)
            , m_this(pThis)
        {
            m_this->isolate_ref();
            m_isolate = pThis->holder();
            m_holder = new ValueHolder(m_this->wrap());
            next(recv);
        }

        ~asyncRead()
        {
            m_this->isolate_unref();
        }

        ON_STATE(asyncRead, recv)
        {
            return m_this->read(-1, m_buf, next(event));
        }

        ON_STATE(asyncRead, event)
        {
            if (n == CALL_RETURN_NULL) {
                m_this->_emit("close");
                return next();
            }

            m_this->_emit("data", m_buf);
            return next(recv);
        }

        virtual int32_t error(int32_t v)
        {
            // Like TcpServer, treat socket close errors as normal termination
            // instead of error events
            if (v == CALL_E_BAD_FILE || v == CALL_E_INVALID_CALL || v == CALL_E_NETNAME_DELETED) {
                m_this->_emit("close");
                return v;
            }

            // Emit error event in JS context with proper Error object
            m_isolate->sync([this, v]() -> int32_t {
                v8::Local<v8::Value> err = FillError(v);
                bool retVal;
                m_this->_emit("error", &err, 1, retVal);
                return 0;
            });
            
            return v;
        }

    private:
        Isolate* m_isolate;
        obj_ptr<ValueHolder> m_holder;
        obj_ptr<Stream_base> m_this;
        obj_ptr<Buffer_base> m_buf;
    };

    if (readState.CompareAndSwap(ReadState::C_OPEN, ReadState::C_READING) == ReadState::C_OPEN)
        (new asyncRead(stream))->apost(0);

    return 0;
}

}
