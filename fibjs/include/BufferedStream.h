/*
 * BufferedStream.h
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "ifs/BufferedStream.h"
#include "StringBuffer.h"
#include "encoding_iconv.h"

#ifndef BUFFEREDSTREAM_H_
#define BUFFEREDSTREAM_H_

namespace fibjs
{

class BufferedStream: public fibjs::BufferedStream_base
{
public:
    BufferedStream(Stream_base *stm) :
        m_stm(stm), m_pos(0), m_temp(0)
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
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, AsyncEvent *ac);
    virtual result_t close(AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes, int64_t &retVal, AsyncEvent *ac);

public:
    // BufferedStream_base
    virtual result_t readText(int32_t size, exlib::string &retVal, AsyncEvent *ac);
    virtual result_t readLine(int32_t maxlen, exlib::string &retVal, AsyncEvent *ac);
    virtual result_t readLines(int32_t maxlines, v8::Local<v8::Array> &retVal);
    virtual result_t readUntil(const char *mk, int32_t maxlen, exlib::string &retVal, AsyncEvent *ac);
    virtual result_t writeText(const char *txt, AsyncEvent *ac);
    virtual result_t writeLine(const char *txt, AsyncEvent *ac);
    virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
    virtual result_t get_charset(exlib::string &retVal);
    virtual result_t set_charset(const char *newVal);
    virtual result_t get_EOL(exlib::string &retVal);
    virtual result_t set_EOL(const char *newVal);

public:
    void append(int32_t n)
    {
        if (n > 0)
        {
            if (m_pos == 0 && n == (int32_t) m_buf.length())
                m_strbuf.append(m_buf);
            else
            {
                exlib::string s1(m_buf.substr(m_pos, n));
                m_strbuf.append(s1);
            }
            m_pos += n;
        }
    }

public:
    obj_ptr<Stream_base> m_stm;
    exlib::string m_buf;
    int32_t m_pos;
    int32_t m_temp;
    exlib::string m_eol;
    StringBuffer m_strbuf;
    encoding_iconv m_iconv;
};

} /* namespace fibjs */
#endif /* BUFFEREDSTREAM_H_ */
