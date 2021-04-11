/**
 * @author richardo2016@gmail.com
 * @email richardo2016
 * @create date 2021-04-11 19:01:26
 * @modify date 2021-04-11 19:01:26
 * 
 * @desc fibjs's Isolate implementation
 */

#include "UVStream.h"

namespace fibjs {

#ifndef _WIN32
#define _fileno fileno
#endif

void Isolate::get_stdin(obj_ptr<Stream_base>& retVal)
{
    int32_t fd = _fileno(stdin);

    if (!m_stdin)
        m_stdin = new UVStream(fd);

    retVal = m_stdin;
}

void Isolate::get_stdout(obj_ptr<Stream_base>& retVal)
{
    int32_t fd = _fileno(stdout);

    if (!m_stdout)
        m_stdout = new UVStream(fd);

    retVal = this->m_stdout;
}

void Isolate::get_stderr(obj_ptr<Stream_base>& retVal)
{
    int32_t fd = _fileno(stderr);

    if (!m_stderr)
        m_stderr = new UVStream(fd);

    retVal = m_stderr;
}

} // namespace fibjs