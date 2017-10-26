/*
 * zlib.cpp
 *
 *  Created on: Sep 13, 2012
 *      Author: lion
 */

#include "object.h"
#include "zlib.h"

namespace fibjs {

DECLARE_MODULE(zlib);

result_t zlib_base::createDeflate(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new def(to, -1);
    return 0;
}

result_t zlib_base::createDeflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new defraw(to);
    return 0;
}

result_t zlib_base::createGunzip(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new gunz(to);
    return 0;
}

result_t zlib_base::createGzip(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new gz(to);
    return 0;
}

result_t zlib_base::createInflate(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new inf(to);
    return 0;
}

result_t zlib_base::createInflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal)
{
    retVal = new infraw(to);
    return 0;
}

result_t zlib_base::deflate(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(NULL, level))->process(data, retVal, ac);
}

result_t zlib_base::deflateTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(stm, level))->process(data, ac);
}

result_t zlib_base::deflateTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new def(stm, level))->process(src, ac);
}

result_t zlib_base::inflate(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(NULL))->process(data, retVal, ac);
}

result_t zlib_base::inflateTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(stm))->process(data, ac);
}

result_t zlib_base::inflateTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new inf(stm))->process(src, ac);
}

result_t zlib_base::gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(NULL))->process(data, retVal, ac);
}

result_t zlib_base::gzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(stm))->process(data, ac);
}

result_t zlib_base::gzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gz(stm))->process(src, ac);
}

result_t zlib_base::gunzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(NULL))->process(data, retVal, ac);
}

result_t zlib_base::gunzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(stm))->process(data, ac);
}

result_t zlib_base::gunzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new gunz(stm))->process(src, ac);
}

result_t zlib_base::deflateRaw(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(NULL))->process(data, retVal, ac);
}

result_t zlib_base::deflateRawTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(stm))->process(data, ac);
}

result_t zlib_base::deflateRawTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new defraw(stm))->process(src, ac);
}

result_t zlib_base::inflateRaw(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(NULL))->process(data, retVal, ac);
}

result_t zlib_base::inflateRawTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(stm))->process(data, ac);
}

result_t zlib_base::inflateRawTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new infraw(stm))->process(src, ac);
}
}
