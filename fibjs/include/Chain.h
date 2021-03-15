/*
 * Chain.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/Chain.h"
#include "QuickArray.h"
#include "ifs/mq.h"
#include "Fiber.h"

#pragma once

namespace fibjs {

class Chain : public Chain_base {
public:
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(Handler_base* hdlr, object_base* v, AsyncEvent* ac)
            : AsyncState(ac)
            , m_v(v)
            , m_hr(CALL_RETURN_NULL)
            , m_pos(0)
        {
            m_msg = Message_base::getInstance(m_v);

            m_hdrs.resize(1);
            m_hdrs[0] = hdlr;

            next(invoke);
        }

        asyncInvoke(QuickArray<obj_ptr<Handler_base>>& hdlrs, object_base* v, AsyncEvent* ac)
            : AsyncState(ac)
            , m_v(v)
            , m_hr(CALL_RETURN_NULL)
            , m_pos(0)
        {
            int32_t i;

            m_msg = Message_base::getInstance(m_v);

            m_hdrs.resize(hdlrs.size());
            for (i = 0; i < (int32_t)hdlrs.size(); i++)
                m_hdrs[i] = hdlrs[i];

            next(invoke);
        }

    public:
        ON_STATE(asyncInvoke, invoke)
        {
            result_t hr;

            if (n == CALL_RETURN_NULL || m_hr == CALL_RETURN_NULL) {
                bool end = false;

                m_hr = 0;

                if (m_msg)
                    m_msg->isEnded(end);
                if (end || (m_pos == (int32_t)m_hdrs.size()))
                    return next(CALL_RETURN_NULL);

                m_next = m_hdrs[m_pos++];
            }

            if (m_hr == CALL_E_EXCEPTION)
                Runtime::setError(m_message);

            if (m_hr < 0)
                return next(m_hr);

            m_hdlr = m_next;
            m_next.Release();

            hr = m_hdlr->invoke(m_v, m_next, this);
            if (hr == CALL_E_NOASYNC) {
                sync(m_hdlr->holder());
                return CALL_E_PENDDING;
            }

            return hr;
        }

    public:
        virtual result_t js_invoke()
        {
            {
                JSFiber::EnterJsScope s;

                m_hr = js_run();
                if (m_hr == CALL_E_EXCEPTION)
                    m_message = Runtime::errMessage();
            }

            apost(0);

            return m_hr;
        }

    private:
        result_t js_run()
        {
            result_t hr;
            obj_ptr<Handler_base> hdlr1 = m_hdlr;
            obj_ptr<Handler_base> hdlr2;
            AsyncEvent ac;

            while (true) {
                hr = hdlr1->invoke(m_v, hdlr2, &ac);
                if (hr == CALL_E_NOSYNC) {
                    if (!m_vholder)
                        m_vholder = new ValueHolder(m_v->wrap());

                    m_next = hdlr1;
                    return 0;
                }

                if (hr < 0)
                    return hr;

                if (hr == CALL_RETURN_NULL) {
                    bool end = false;

                    if (m_msg)
                        m_msg->isEnded(end);
                    if (end || (m_pos == (int32_t)m_hdrs.size()))
                        return hr;

                    hdlr1 = m_hdrs[m_pos++];
                } else
                    hdlr1 = hdlr2;
            }

            return 0;
        }

    private:
        obj_ptr<Handler_base> m_hdlr;
        obj_ptr<Handler_base> m_next;
        obj_ptr<object_base> m_v;
        obj_ptr<Message_base> m_msg;
        obj_ptr<ValueHolder> m_vholder;
        result_t m_hr;
        exlib::string m_message;
        std::vector<obj_ptr<Handler_base>> m_hdrs;
        int32_t m_pos;
    };

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    // Chain_base
    virtual result_t append(v8::Local<v8::Array> hdlrs);
    virtual result_t append(Handler_base* hdlr);

private:
    QuickArray<obj_ptr<Handler_base>> m_array;
};

} /* namespace fibjs */
