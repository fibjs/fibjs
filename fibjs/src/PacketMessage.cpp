/*
 * PacketMessage.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: lion
 */

#include "PacketMessage.h"
#include "Buffer.h"

namespace fibjs
{

result_t PacketMessage_base::_new(int32_t maxSize, obj_ptr<PacketMessage_base> &retVal,
                                  v8::Local<v8::Object> This)
{
    retVal = new PacketMessage(maxSize);
    return 0;
}

result_t PacketMessage::get_value(std::string &retVal)
{
    return m_message.get_value(retVal);
}

result_t PacketMessage::set_value(const char *newVal)
{
    return m_message.set_value(newVal);
}

result_t PacketMessage::get_params(obj_ptr<List_base> &retVal)
{
    return m_message.get_params(retVal);
}

result_t PacketMessage::set_params(List_base *newVal)
{
    return m_message.set_params(newVal);
}

result_t PacketMessage::get_result(Variant &retVal)
{
    return m_message.get_result(retVal);
}

result_t PacketMessage::set_result(Variant newVal)
{
    return m_message.set_result(newVal);
}

result_t PacketMessage::get_body(obj_ptr<SeekableStream_base> &retVal)
{
    return m_message.get_body(retVal);
}

result_t PacketMessage::set_body(SeekableStream_base *newVal)
{
    return m_message.set_body(newVal);
}

result_t PacketMessage::get_length(int64_t &retVal)
{
    return m_message.get_length(retVal);
}

result_t PacketMessage::clear()
{
    m_message.clear();

    if (m_response)
        m_response->clear();

    return 0;
}

result_t PacketMessage::sendTo(Stream_base *stm, exlib::AsyncEvent *ac)
{
    class asyncSendTo: public asyncState
    {
    public:
        asyncSendTo(PacketMessage *pThis, Stream_base *stm,
                    exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_stm(stm)
        {
            m_pThis->get_body(m_body);
            set(read);
        }

        static int read(asyncState *pState, int n)
        {
            asyncSendTo *pThis = (asyncSendTo *) pState;

            pThis->m_body->rewind();

            pThis->set(sendPacket);
            return pThis->m_body->readAll(pThis->m_buffer, pThis);
        }

        static int sendPacket(asyncState *pState, int n)
        {
            asyncSendTo *pThis = (asyncSendTo *) pState;

            std::string strBuf;

            if (pThis->m_buffer != NULL)
            {
                std::string strData;
                int32_t len;

                pThis->m_buffer->toString(strData);
                len = (int32_t)strData.length();

                packetSize(len, strBuf);
                strBuf.append(strData);
            }
            else
                strBuf.append("\0", 1);

            pThis->m_buffer = new Buffer(strBuf);

            pThis->set(NULL);
            return pThis->m_stm->write(pThis->m_buffer, pThis);
        }

    public:
        PacketMessage *m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Buffer_base> m_buffer;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSendTo(this, stm, ac))->post(0);
}

result_t PacketMessage::readFrom(BufferedStream_base *stm, exlib::AsyncEvent *ac)
{

    class asyncReadFrom: public asyncState
    {
    public:
        asyncReadFrom(PacketMessage *pThis, BufferedStream_base *stm,
                      exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_stm(stm)
        {
            m_pThis->get_body(m_body);
            set(begin);
        }

        static int begin(asyncState *pState, int n)
        {
            asyncReadFrom *pThis = (asyncReadFrom *) pState;

            pThis->set(body);
            return pThis->m_stm->readPacket(pThis->m_pThis->m_maxSize,
                                            pThis->m_buffer, pThis);
        }

        static int body(asyncState *pState, int n)
        {
            asyncReadFrom *pThis = (asyncReadFrom *) pState;

            if (n == CALL_RETURN_NULL)
                return pThis->done(CALL_RETURN_NULL);

            pThis->set(body_end);
            return pThis->m_body->write(pThis->m_buffer, pThis);
        }

        static int body_end(asyncState *pState, int n)
        {
            asyncReadFrom *pThis = (asyncReadFrom *) pState;

            pThis->m_body->rewind();
            return pThis->done();
        }

    public:
        PacketMessage *m_pThis;
        obj_ptr<BufferedStream_base> m_stm;
        obj_ptr<SeekableStream_base> m_body;
        obj_ptr<Buffer_base> m_buffer;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    stm->get_stream(m_stm);

    return (new asyncReadFrom(this, stm, ac))->post(0);
}

result_t PacketMessage::get_stream(obj_ptr<Stream_base> &retVal)
{
    if (!m_stm)
        return CALL_RETURN_NULL;

    retVal = m_stm;
    return 0;
}

result_t PacketMessage::get_response(obj_ptr<Message_base> &retVal)
{
    if (m_bRep)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!m_response)
        m_response = new PacketMessage(m_maxSize, true);

    retVal = m_response;
    return 0;
}

result_t PacketMessage::get_maxSize(int32_t &retVal)
{
    retVal = m_maxSize;
    return 0;
}

result_t PacketMessage::set_maxSize(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxSize = newVal;
    return 0;
}

} /* namespace fibjs */
