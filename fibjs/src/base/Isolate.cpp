/**
 * @author richardo2016@gmail.com
 * @email richardo2016
 * @create date 2021-04-11 19:01:26
 * @modify date 2021-04-11 19:01:26
 * 
 * @desc fibjs's Isolate implementation
 */

#include "TTYStream.h"

namespace fibjs {

v8::Local<v8::String> NewString(v8::Isolate* isolate, const char* data, ssize_t length)
{
    if (length > INT_MAX) {
        ThrowResult(CALL_E_OVERFLOW);
        return v8::Local<v8::String>();
    }

    return v8::String::NewFromUtf8(isolate, data, v8::String::kNormalString, (int32_t)length);
}

void Isolate::get_stdin(obj_ptr<Stream_base>& retVal)
{
    if (!m_stdin) {
        int32_t fd = _fileno(stdin);
        if (_is_ctx_atty(fd))
            m_stdin = new TTYInputStream(fd);
        else
            m_stdin = new UVStream(fd);
    }

    retVal = m_stdin;
}

void Isolate::get_stdout(obj_ptr<Stream_base>& retVal)
{
    if (!m_stdout) {
        int32_t fd = _fileno(stdout);
        if (_is_ctx_atty(fd))
            m_stdout = new TTYOutputStream(fd);
        else
            m_stdout = new UVStream(fd);
    }

    retVal = this->m_stdout;
}

void Isolate::get_stderr(obj_ptr<Stream_base>& retVal)
{
    if (!m_stderr) {
        int32_t fd = _fileno(stderr);
        if (_is_ctx_atty(fd))
            m_stderr = new TTYOutputStream(fd);
        else
            m_stderr = new UVStream(fd);
    }

    retVal = m_stderr;
}

} // namespace fibjs