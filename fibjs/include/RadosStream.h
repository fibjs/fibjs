#include "ifs/RadosStream.h"
#include <rados/librados.h>

#ifndef _fj_RADOSSTREAM_H
#define _fj_RADOSSTREAM_H

namespace fibjs {

class RadosStream : public RadosStream_base {
public:
    RadosStream()
    {
    }

    ~RadosStream()
    {
        close();
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac);

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence);
    virtual result_t tell(int64_t& retVal);
    virtual result_t rewind();
    virtual result_t size(int64_t& retVal);
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t eof(bool& retVal);
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac);

public:
    // RadosStream_base
    virtual result_t get_key(exlib::string& retVal);
    virtual result_t radosStat(obj_ptr<RadosStat_base>& retVal, AsyncEvent* ac);
    virtual result_t writeFull(Buffer_base* data, AsyncEvent* ac);
    virtual result_t append(Buffer_base* data, AsyncEvent* ac);

public:
    result_t open(rados_ioctx_t io, exlib::string key);
    result_t remove(AsyncEvent* ac);
    void close()
    {
        m_ioctx = NULL;
    }

public:
    exlib::atomic m_off;
    rados_ioctx_t m_ioctx;
    exlib::string m_key;
    exlib::Locker m_lockWrite;
    exlib::Locker m_lockRead;

};
}

#endif // RADOSSTREAM_H
