/**
 * @author richardo2016@gmail.com
 * @email richardo2016
 * @create date 2021-04-10 02:16:35
 * @modify date 2021-04-10 02:16:35
 * 
 * @desc tty about stream implementation
 */

#pragma once

#include "ifs/TTYInputStream.h"
#include "ifs/TTYOutputStream.h"
#include "ifs/process.h"
#include "ifs/tty.h"
#include "UVStream.h"
#include "AsyncUV.h"

namespace fibjs {

#ifndef _WIN32
#define _fileno fileno
#endif

inline bool _is_ctx_atty(int32_t fd)
{
    bool is_tty;

    tty_base::isatty(fd, is_tty);

    return is_tty;
}

class TTYInputStream : public UVStream_tmpl<TTYInputStream_base> {
public:
    TTYInputStream(int32_t fd)
        : UVStream_tmpl<TTYInputStream_base>(fd)
        , m_isRaw(false)
    {
        uv_call([&] {
            uv_tty_init(s_uv_loop, &m_tty, fd, 0);
            return uv_stream_set_blocking(&m_stream, 1);
        });
    }

public:
    // TTYInputStream_base
    virtual result_t get_isTTY(bool& retVal)
    {
        retVal = true;

        return 0;
    }
    virtual result_t get_isRaw(bool& retVal);
    virtual result_t setRawMode(bool isRawMode, obj_ptr<TTYInputStream_base>& retVal);

public:
    bool m_isRaw;
};

class TTYOutputStream : public UVStream_tmpl<TTYOutputStream_base> {
public:
    TTYOutputStream(int32_t fd)
        : UVStream_tmpl<TTYOutputStream_base>(fd)
    {
        uv_call([&] {
            uv_tty_init(s_uv_loop, &m_tty, fd, 0);
            return uv_stream_set_blocking(&m_stream, 1);
        });
    }

public:
    // TTYOutputStream_base
    virtual result_t get_isTTY(bool& retVal)
    {
        retVal = true;

        return 0;
    }
    virtual result_t clearLine(int32_t dir);
    virtual result_t clearScreenDown();
    virtual result_t getWindowSize(obj_ptr<NArray>& retVal);

public:
    static const char* kClearToLineBeginning;
    static const char* kClearToLineEnd;
    static const char* kClearLine;
    static const char* kClearScreenDown;
};

} /* namespace fibjs */
