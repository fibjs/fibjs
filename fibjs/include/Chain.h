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

#ifndef CHAIN_H_
#define CHAIN_H_

namespace fibjs {

class Chain : public Chain_base {
public:
    class asyncInvoke : public AsyncState {
    private:
        class ValueHolder : public object_base {
        public:
            v8::Global<v8::Value> m_v;
        };

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

            set(invoke);
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

            set(invoke);
        }

    public:
        static int32_t invoke(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            result_t hr;

            if (n == CALL_RETURN_NULL || pThis->m_hr == CALL_RETURN_NULL) {
                bool end = false;

                pThis->m_hr = 0;

                if (pThis->m_msg)
                    pThis->m_msg->isEnded(end);
                if (end || (pThis->m_pos == (int32_t)pThis->m_hdrs.size()))
                    return pThis->done(CALL_RETURN_NULL);

                pThis->m_next = pThis->m_hdrs[pThis->m_pos++];
            }

            if (pThis->m_hr == CALL_E_EXCEPTION)
                Runtime::setError(pThis->m_message);

            if (pThis->m_hr < 0)
                return pThis->m_hr;

            pThis->m_hdlr = pThis->m_next;
            pThis->m_next.Release();

            hr = pThis->m_hdlr->invoke(pThis->m_v, pThis->m_next, pThis);
            if (hr == CALL_E_NOASYNC) {
                pThis->sync(pThis->m_hdlr->holder());
                return CALL_E_PENDDING;
            }

            return hr;
        }

    public:
        virtual result_t js_invoke()
        {
            {
                JSFiber::scope s;

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
                    if (!m_vholder) {
                        m_vholder = new ValueHolder();
                        m_vholder->m_v.Reset(m_v->holder()->m_isolate, m_v->wrap());
                        m_vholder->setJSObject();
                    }

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
#endif /* CHAIN_H_ */
