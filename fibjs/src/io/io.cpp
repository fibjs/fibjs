/*
 * fs.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include <string.h>

#include "object.h"
#include "ifs/io.h"
#include "File.h"

namespace fibjs {

DECLARE_MODULE(io);

result_t io_base::copyStream(Stream_base* from, Stream_base* to, int64_t bytes,
    int64_t& retVal, AsyncEvent* ac)
{
    class asyncCopy : public AsyncState {
    public:
        asyncCopy(Stream_base* from, Stream_base* to, int64_t bytes,
            int64_t& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_from(from)
            , m_to(to)
            , m_bytes(bytes)
            , m_retVal(
                  retVal)
        {
            m_retVal = 0;
            next(read);
        }

        ON_STATE(asyncCopy, read)
        {
            int64_t len;

            if (m_bytes == 0)
                return next();

            if (m_bytes < 0)
                len = -1;
            else if (m_bytes > STREAM_BUFF_SIZE)
                len = STREAM_BUFF_SIZE;
            else
                len = m_bytes;

            m_buf.Release();
            return m_from->read((int32_t)len, m_buf, next(write));
        }

        ON_STATE(asyncCopy, write)
        {
            if (n == CALL_RETURN_NULL)
                return next();

            int32_t blen = Buffer::Cast(m_buf)->length();
            m_retVal += blen;

            if (m_bytes > 0)
                m_bytes -= blen;

            return m_to->write(m_buf, next(read));
        }

    public:
        obj_ptr<Stream_base> m_from;
        obj_ptr<Stream_base> m_to;
        int64_t m_bytes;
        int64_t& m_retVal;
        obj_ptr<Buffer_base> m_buf;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncCopy(from, to, bytes, retVal, ac))->post(0);
}

#define BRIDGE_READ 1
#define BRIDGE_WRITE 2
#define BRIDGE_DONE 0

result_t io_base::bridge(Stream_base* stm1, Stream_base* stm2, AsyncEvent* ac)
{
    class AsyncData {
    public:
        class AsyncCopy : public AsyncState {
        public:
            AsyncCopy(AsyncData* data, int32_t idx)
                : AsyncState(NULL)
                , m_data(data)
                , m_from(idx)
                , m_to(1 - idx)
            {
                next(read);
            }

            ON_STATE(AsyncCopy, read)
            {
                m_buf.Release();

                if (m_data->m_states[m_from].CompareAndSwap(
                        BRIDGE_WRITE, BRIDGE_READ)
                    != BRIDGE_WRITE)
                    return error(0);

                return m_data->m_stms[m_from]->read(-1, m_buf, next(write));
            }

            ON_STATE(AsyncCopy, write)
            {
                if (n == CALL_RETURN_NULL)
                    return error(0);

                if (m_data->m_states[m_from].CompareAndSwap(
                        BRIDGE_READ, BRIDGE_WRITE)
                    != BRIDGE_READ)
                    return error(0);

                return m_data->m_stms[m_to]->write(m_buf, next(read));
            }

            ON_STATE(AsyncCopy, cancel)
            {
                return m_data->m_stms[m_to]->close(next(end));
            }

            ON_STATE(AsyncCopy, end)
            {
                return error(0);
            }

            virtual int32_t error(int32_t v)
            {
                if (m_data->m_states[m_from].xchg(BRIDGE_DONE) == BRIDGE_READ) {
                    m_data->m_states[m_to].xchg(BRIDGE_DONE);
                    return next(cancel);
                }

                m_data->release();
                return next();
            }

        public:
            AsyncData* m_data;
            int32_t m_from, m_to;
            obj_ptr<Buffer_base> m_buf;
        };

    public:
        AsyncData(Stream_base* stm1, Stream_base* stm2, AsyncEvent* ac)
            : m_ac(ac)
        {
            m_stms[0] = stm1;
            m_stms[1] = stm2;

            m_states[0] = BRIDGE_WRITE;
            m_states[1] = BRIDGE_WRITE;

            m_ref = 2;

            (new AsyncCopy(this, 0))->apost(0);
            (new AsyncCopy(this, 1))->apost(0);
        }

        void release()
        {
            if (m_ref.dec() == 0) {
                m_ac->post(0);
                delete this;
            }
        }

    public:
        obj_ptr<Stream_base> m_stms[2];
        exlib::atomic m_states[2];
        exlib::atomic m_ref;
        AsyncEvent* m_ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    new AsyncData(stm1, stm2, ac);
    return CALL_E_PENDDING;
}
}
