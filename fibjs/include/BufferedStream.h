/*
 * BufferedStream.h
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "ifs/BufferedStream.h"
#include "StringBuffer.h"

#ifndef BUFFEREDSTREAM_H_
#define BUFFEREDSTREAM_H_

namespace fibjs
{

class BufferedStream: public fibjs::BufferedStream_base
{
public:
    BufferedStream(Stream_base *stm) :
        m_stm(stm), m_pos(0), m_mkpos(0)
    {
#ifdef _WIN32
        m_eol.assign("\r\n", 2);
#else
        m_eol.assign("\n", 1);
#endif
        extMemory(4096);
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, exlib::AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
    virtual result_t close(exlib::AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes, int64_t &retVal, exlib::AsyncEvent *ac);

public:
    // BufferedStream_base
    virtual result_t readText(int32_t size, std::string &retVal, exlib::AsyncEvent *ac);
    virtual result_t readLine(int32_t maxlen, std::string &retVal, exlib::AsyncEvent *ac);
    virtual result_t readLines(int32_t maxlines, v8::Handle<v8::Array> &retVal);
    virtual result_t readUntil(const char *mk, int32_t maxlen, std::string &retVal, exlib::AsyncEvent *ac);
    virtual result_t writeText(const char *txt, exlib::AsyncEvent *ac);
    virtual result_t writeLine(const char *txt, exlib::AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_EOL(std::string &retVal);
    virtual result_t set_EOL(const char *newVal);

public:
    void append(int n)
    {
        if (n > 0)
        {
            if (m_pos == 0 && n == (int) m_buf.length())
                m_strbuf.append(m_buf);
            else
            {
                std::string s1(m_buf.substr(m_pos, n));
                m_strbuf.append(s1);
            }
            m_pos += n;
        }
    }

public:
    obj_ptr<Stream_base> m_stm;
    std::string m_buf;
    int m_pos;
    int m_mkpos;
    std::string m_eol;
    StringBuffer m_strbuf;
};

} /* namespace fibjs */
#endif /* BUFFEREDSTREAM_H_ */
