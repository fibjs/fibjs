/*
 * PacketMessage.h
 *
 *  Created on: Dec 26, 2013
 *      Author: lion
 */

#include "Message.h"
#include "ifs/PacketMessage.h"

#ifndef PACKETMESSAGE_H_
#define PACKETMESSAGE_H_

namespace fibjs
{

class PacketMessage: public PacketMessage_base
{
public:
    PacketMessage(int32_t maxSize, bool bRep = false)
        : m_maxSize(maxSize), m_bRep(bRep)
    {
    }

public:
    // Message_base
    virtual result_t get_value(std::string &retVal);
    virtual result_t set_value(const char *newVal);
    virtual result_t get_params(obj_ptr<List_base> &retVal);
    virtual result_t set_params(List_base *newVal);
    virtual result_t get_result(Variant &retVal);
    virtual result_t set_result(Variant newVal);
    virtual result_t get_body(obj_ptr<SeekableStream_base> &retVal);
    virtual result_t set_body(SeekableStream_base *newVal);
    virtual result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
    virtual result_t get_length(int64_t &retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base *stm, exlib::AsyncEvent *ac);
    virtual result_t readFrom(BufferedStream_base *stm, exlib::AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_response(obj_ptr<Message_base> &retVal);

public:
    // PacketMessage_base
    virtual result_t get_maxSize(int32_t &retVal);
    virtual result_t set_maxSize(int32_t newVal);

public:
    static void packetSize(int32_t sz, std::string &strBuf)
    {
        unsigned char buf[sizeof(int32_t) + 1] = {0};
        int n = sizeof(int32_t) + 1;
        int m = 0;

        while (sz)
        {
            buf[--n] = (unsigned char)((sz & 0x7f) | m);
            m = 0x80;
            sz >>= 7;
        }

        if (n == sizeof(int32_t) + 1)
            n --;

        strBuf.append((const char *)&buf[n], sizeof(int32_t) + 1 - n);
    }

public:
    obj_ptr<Stream_base> m_stm;
    int32_t m_maxSize;

private:
    Message::_msg m_message;
    obj_ptr<PacketMessage_base> m_response;
    bool m_bRep;
};

} /* namespace fibjs */
#endif /* PACKETMESSAGE_H_ */
