/*
 * Message.h
 *
 *  Created on: Sep 2, 2012
 *      Author: lion
 */

#include "ifs/Message.h"
#include "ifs/List.h"
#include "ifs/SeekableStream.h"

#ifndef MESSAGE_H_
#define MESSAGE_H_

namespace fibjs
{

class Message: public Message_base
{
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
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t readAll(obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, AsyncEvent *ac);
    virtual result_t get_length(int64_t &retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base *stm, AsyncEvent *ac);
    virtual result_t readFrom(Stream_base *stm, AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_response(obj_ptr<Message_base> &retVal);

public:
    Message(bool bRep = false) : m_bRep(bRep)
    {
    }

public:
    result_t set_value(std::string &newVal)
    {
        m_value = newVal;
        return 0;
    }

    obj_ptr<SeekableStream_base> &body()
    {
        return m_body;
    }

private:
    obj_ptr<List_base> m_params;
    VariantEx m_result;
    std::string m_value;
    obj_ptr<SeekableStream_base> m_body;
    obj_ptr<Message_base> m_response;
    bool m_bRep;
};

} /* namespace fibjs */
#endif /* MESSAGE_H_ */
