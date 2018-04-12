#include "ifs/RbdImage.h"
#include <rados/librados.h>
#include <rados/librbd.h>

#ifndef _fj_RBDIMAGE_H
#define _fj_RBDIMAGE_H

namespace fibjs {

class RbdImage : public RbdImage_base {
public:
    RbdImage()
        : m_off(0)
        , m_image(NULL)
    {
    }

    ~RbdImage()
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
    virtual result_t eof(bool& retVal);
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac);

public:
    // RbdImage_base
    virtual result_t get_stripe_unit(int64_t& retVal);
    virtual result_t get_stripe_count(int64_t& retVal);
    virtual result_t get_features(int64_t& retVal);
    virtual result_t get_create_timestamp(date_t& retVal);
    virtual result_t get_block_name_prefix(exlib::string& retVal);
    virtual result_t resize(int64_t bytes, AsyncEvent* ac);
    virtual result_t createSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t removeSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t rollbackSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t listSnaps(v8::Local<v8::Array>& retVal, AsyncEvent* ac);
    virtual result_t protectSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t unprotectSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t setSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t isSnapProtected(exlib::string snapname, bool& retVal, AsyncEvent* ac);

public:
    result_t open(rados_ioctx_t io, exlib::string name, exlib::string snapshot);
    void close();

public:
    exlib::atomic m_off;
    rbd_image_t m_image;
    rados_ioctx_t m_ioctx;
    exlib::Locker m_lockWrite;
    exlib::Locker m_lockRead;
};
}

#endif // RBDIMAGE_H
