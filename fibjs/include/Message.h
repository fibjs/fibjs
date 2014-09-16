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
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t readAll(obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
    virtual result_t get_length(int64_t &retVal);
    virtual result_t clear();
    virtual result_t sendTo(Stream_base *stm, exlib::AsyncEvent *ac);
    virtual result_t readFrom(BufferedStream_base *stm, exlib::AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_response(obj_ptr<Message_base> &retVal);

public:
    class _msg
    {
        class values: public object_base
        {
        public:
            virtual bool isJSObject()
            {
                return true;
            }

        public:
            obj_ptr<List_base> m_params;
            VariantEx m_result;
        };

    public:
        result_t get_value(std::string &retVal);
        result_t set_value(const char *newVal);
        result_t get_params(obj_ptr<List_base> &retVal);
        result_t set_params(List_base *newVal);
        result_t get_result(Variant &retVal);
        result_t set_result(Variant newVal);
        result_t get_body(obj_ptr<SeekableStream_base> &retVal);
        result_t set_body(SeekableStream_base *newVal);
        result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
        result_t readAll(obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
        result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
        result_t get_length(int64_t &retVal);

    public:
        void clear()
        {
            m_value.clear();
            m_values.Release();
            m_body.Release();
        }

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
        std::string m_value;
        obj_ptr<values> m_values;
        obj_ptr<SeekableStream_base> m_body;
    };

public:
    Message(bool bRep = false) : m_bRep(bRep)
    {
    }

private:
    _msg m_message;
    obj_ptr<Message_base> m_response;
    bool m_bRep;
};

} /* namespace fibjs */
#endif /* MESSAGE_H_ */
