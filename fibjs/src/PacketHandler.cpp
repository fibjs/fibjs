/*
 * PacketHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/mq.h"
#include "PacketHandler.h"
#include "PacketMessage.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/console.h"

namespace fibjs
{

static const char *s_staticCounter[] =
{ "total", "pendding" };
static const char *s_Counter[] =
{ "request", "response", "error" };

enum
{
    PACKET_TOTAL = 0,
    PACKET_PENDDING,
    PACKET_REQUEST,
    PACKET_RESPONSE,
    PACKET_ERROR
};

result_t PacketHandler_base::_new(v8::Local<v8::Value> hdlr,
                                  obj_ptr<PacketHandler_base> &retVal)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;

    retVal = new PacketHandler(hdlr1);
    return 0;
}

PacketHandler::PacketHandler(Handler_base *hdlr) :
    m_hdlr(hdlr), m_maxSize(67108864)
{
    m_stats = new Stats();
    m_stats->init(s_staticCounter, 2, s_Counter, 3);
}

result_t PacketHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                               exlib::AsyncEvent *ac)
{
    class asyncInvoke: public asyncState
    {
    public:
        asyncInvoke(PacketHandler *pThis, Stream_base *stm, exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_stm(stm)
        {
            m_stmBuffered = new BufferedStream(stm);
            m_msg = new PacketMessage(pThis->m_maxSize);
            m_msg->get_response(m_rep);

            set(read);
        }

        static int read(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->m_msg->clear();
            pThis->set(invoke);
            return pThis->m_msg->readFrom(pThis->m_stmBuffered, pThis);
        }

        static int invoke(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            if (n == CALL_RETURN_NULL)
                return pThis->done(CALL_RETURN_NULL);

            pThis->m_pThis->m_stats->inc(PACKET_TOTAL);
            pThis->m_pThis->m_stats->inc(PACKET_REQUEST);
            pThis->m_pThis->m_stats->inc(PACKET_PENDDING);

            pThis->set(send);
            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_msg, pThis);
        }

        static int send(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->set(end);
            return pThis->m_rep->sendTo(pThis->m_stm, pThis);
        }

        static int end(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->m_pThis->m_stats->inc(PACKET_RESPONSE);
            pThis->m_pThis->m_stats->dec(PACKET_PENDDING);

            pThis->set(read);
            return 0;
        }

        virtual int error(int v)
        {
            m_pThis->m_stats->inc(PACKET_ERROR);

            if (is(send))
                asyncLog(console_base::_ERROR, "PacketHandler: " + getResultMessage(v));

            if (is(end))
                m_pThis->m_stats->dec(PACKET_PENDDING);
            else if (is(invoke))
            {
                m_pThis->m_stats->inc(PACKET_TOTAL);
                m_pThis->m_stats->inc(PACKET_REQUEST);
            }

            return v;
        }

    private:
        obj_ptr<PacketHandler> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<Message_base> m_msg;
        obj_ptr<Message_base> m_rep;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
    if (stm == NULL)
    {
        obj_ptr<Message_base> msg = Message_base::getInstance(v);
        if (msg != NULL)
            msg->get_stream(stm);

    }

    if (stm == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t PacketHandler::get_maxSize(int32_t &retVal)
{
    retVal = m_maxSize;
    return 0;
}

result_t PacketHandler::set_maxSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxSize = newVal;
    return 0;
}

result_t PacketHandler::get_stats(obj_ptr<Stats_base> &retVal)
{
    retVal = m_stats;
    return 0;
}

}
