/*
 * Chain.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "object.h"
#include "Chain.h"
#include "JSHandler.h"
#include "ifs/mq.h"

namespace fibjs {

result_t Chain_base::_new(v8::Local<v8::Array> hdlrs,
    obj_ptr<Chain_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Chain_base> chain = new Chain();
    chain->wrap(This);

    result_t hr = chain->append(hdlrs);
    if (hr < 0)
        return hr;

    retVal = chain;

    return 0;
}

result_t Chain::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(Chain* pThis, object_base* v, AsyncEvent* ac)
            : AsyncState(ac)
            , m_v(v)
            , m_pos(0)
        {
            int32_t i;

            m_msg = Message_base::getInstance(m_v);

            m_array.resize(pThis->m_array.size());
            for (i = 0; i < (int32_t)pThis->m_array.size(); i++)
                pThis->m_array[i].get(m_array[i]);

            set(invoke);
        }

    public:
        static int32_t invoke(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
            bool end = false;

            if (pThis->m_msg)
                pThis->m_msg->isEnded(end);
            if (end || (pThis->m_pos == (int32_t)pThis->m_array.size()))
                return pThis->done(CALL_RETURN_NULL);

            pThis->m_pos++;
            return mq_base::invoke(pThis->m_array[pThis->m_pos - 1],
                pThis->m_v, pThis);
        }

    private:
        std::vector<obj_ptr<Handler_base>> m_array;
        obj_ptr<object_base> m_v;
        obj_ptr<Message_base> m_msg;
        int32_t m_pos;
    };

    if (m_array.size() == 0)
        return CHECK_ERROR(Runtime::setError("Chain: empty chain."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncInvoke(this, v, ac))->post(0);
}

result_t Chain::append(Handler_base* hdlr)
{
    int32_t no = (int32_t)m_array.size();

    char strBuf[32];
    sprintf(strBuf, "handler_%d", no);

    SetPrivate(strBuf, hdlr->wrap());
    m_array.append(hdlr);

    return 0;
}

result_t Chain::append(v8::Local<v8::Value> hdlr)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;
    return append(hdlr1);
}

result_t Chain::append(v8::Local<v8::Array> hdlrs)
{
    int32_t len = hdlrs->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> v = hdlrs->Get(i);
        obj_ptr<Handler_base> hdlr = Handler_base::getInstance(v);

        if (hdlr) {
            append(hdlr);
            continue;
        }

        hr = append(v);
        if (hr < 0)
            return hr;
    }

    return 0;
}

} /* namespace fibjs */
