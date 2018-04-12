/*
 * RbdImage.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: asionius <asionius@163.com>
 */

#include "object.h"
#include "ifs/io.h"
#include "RbdImage.h"
#include "Stat.h"
#include "Buffer.h"
#include <exlib/include/fiber.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace fibjs {

#ifndef _WIN32

#define MAX_BLOCK_NAME_PREFIX 4096

extern void* get_librbd_handle(void);

static int (*_rbd_open)(rados_ioctx_t, const char*, rbd_image_t*, const char*);
static int (*_rbd_close)(rbd_image_t);
static int (*_rbd_resize)(rbd_image_t, uint64_t);
static int (*_rbd_stat)(rbd_image_t, rbd_image_info_t*, size_t);
static int (*_rbd_get_size)(rbd_image_t, uint64_t*);
static int (*_rbd_get_create_timestamp)(rbd_image_t, struct timespec*);
static int (*_rbd_get_features)(rbd_image_t, uint64_t*);
static int (*_rbd_get_stripe_unit)(rbd_image_t, uint64_t*);
static int (*_rbd_get_stripe_count)(rbd_image_t, uint64_t*);
static int (*_rbd_get_block_name_prefix)(rbd_image_t, char*, size_t);
static int (*_rbd_copy2)(rbd_image_t, rbd_image_t);
static int (*_rbd_aio_write)(rbd_image_t, uint64_t, size_t, const char*, rbd_completion_t);
static int (*_rbd_aio_read)(rbd_image_t, uint64_t, size_t, char*, rbd_completion_t);
static int (*_rbd_aio_create_completion)(void*, rbd_callback_t, rbd_completion_t*);
static ssize_t (*_rbd_aio_get_return_value)(rbd_completion_t);
static void (*_rbd_aio_release)(rbd_completion_t);
static int (*_rbd_aio_flush)(rbd_image_t, rbd_completion_t);

static int (*_rbd_snap_list)(rbd_image_t, rbd_snap_info_t*, int*);
static int (*_rbd_snap_create)(rbd_image_t, const char*);
static int (*_rbd_snap_remove)(rbd_image_t, const char*);
static int (*_rbd_snap_rollback)(rbd_image_t, const char*);
static int (*_rbd_snap_protect)(rbd_image_t, const char*);
static int (*_rbd_snap_unprotect)(rbd_image_t, const char*);
static int (*_rbd_snap_is_protected)(rbd_image_t, const char*, int*);
static int (*_rbd_snap_set)(rbd_image_t, const char*);

static inline int32_t load_librbd(void)
{
    static bool ldlib = false;
    void* handle = NULL;

    if (ldlib)
        return 0;

    handle = get_librbd_handle();
    if (!handle)
        return CHECK_ERROR(Runtime::setError("Invalid library handle"));

    _rbd_open = (int (*)(rados_ioctx_t, const char*,
        rbd_image_t*, const char*))dlsym(handle, "rbd_open");
    _rbd_close = (int (*)(rbd_image_t))dlsym(handle, "rbd_close");
    _rbd_resize = (int (*)(rbd_image_t, uint64_t))dlsym(handle, "rbd_resize");
    _rbd_stat = (int (*)(rbd_image_t, rbd_image_info_t*, size_t))dlsym(handle, "rbd_stat");
    _rbd_get_size = (int (*)(rbd_image_t, uint64_t*))dlsym(handle, "rbd_get_size");
    _rbd_get_features = (int (*)(rbd_image_t, uint64_t*))dlsym(handle, "rbd_get_features");
    _rbd_get_stripe_unit = (int (*)(rbd_image_t, uint64_t*))dlsym(handle, "rbd_get_stripe_unit");
    _rbd_get_stripe_count = (int (*)(rbd_image_t, uint64_t*))dlsym(handle, "rbd_get_stripe_count");
    _rbd_get_block_name_prefix = (int (*)(rbd_image_t, char*, size_t))dlsym(handle, "rbd_get_block_name_prefix");
    _rbd_get_create_timestamp = (int (*)(rbd_image_t, struct timespec*))dlsym(handle, "rbd_get_create_timestamp");
    _rbd_copy2 = (int (*)(rbd_image_t, rbd_image_t))dlsym(handle, "rbd_copy2");
    _rbd_aio_write = (int (*)(rbd_image_t, uint64_t,
        size_t, const char*, rbd_completion_t))dlsym(handle, "rbd_aio_write");
    _rbd_aio_read = (int (*)(rbd_image_t, uint64_t,
        size_t, char*, rbd_completion_t))dlsym(handle, "rbd_aio_read");
    _rbd_aio_create_completion = (int (*)(void*, rbd_callback_t,
        rbd_completion_t*))dlsym(handle, "rbd_aio_create_completion");
    _rbd_aio_get_return_value = (ssize_t(*)(rbd_completion_t))dlsym(handle, "rbd_aio_get_return_value");
    _rbd_aio_release = (void (*)(rbd_completion_t))dlsym(handle, "rbd_aio_release");
    _rbd_aio_flush = (int (*)(rbd_image_t, rbd_completion_t))dlsym(handle, "rbd_aio_flush");

    _rbd_snap_list = (int (*)(rbd_image_t, rbd_snap_info_t*, int*))dlsym(handle, "rbd_snap_list");
    _rbd_snap_create = (int (*)(rbd_image_t, const char*))dlsym(handle, "rbd_snap_create");
    _rbd_snap_remove = (int (*)(rbd_image_t, const char*))dlsym(handle, "rbd_snap_remove");
    _rbd_snap_rollback = (int (*)(rbd_image_t, const char*))dlsym(handle, "rbd_snap_rollback");
    _rbd_snap_protect = (int (*)(rbd_image_t, const char*))dlsym(handle, "rbd_snap_protect");
    _rbd_snap_unprotect = (int (*)(rbd_image_t, const char*))dlsym(handle, "rbd_snap_unprotect");
    _rbd_snap_is_protected = (int (*)(rbd_image_t, const char*, int*))dlsym(handle, "rbd_snap_is_protected");
    _rbd_snap_set = (int (*)(rbd_image_t, const char*))dlsym(handle, "rbd_snap_set");

    return 0;
}

class asyncRbdCallback : public exlib::Task_base {
public:
    asyncRbdCallback(RbdImage* pThis, AsyncEvent* ac, exlib::Locker& locker)
        : m_comp(NULL)
        , m_pThis(pThis)
        , m_ac(ac)
        , m_locker(locker)
    {
    }
    virtual ~asyncRbdCallback()
    {
        if (m_comp) {
            _rbd_aio_release(m_comp);
            m_comp = NULL;
        }
    }

public:
    virtual void suspend()
    {
    }

    virtual void suspend(exlib::spinlock& lock)
    {
        lock.unlock();
    }

    virtual void resume()
    {
    }

public:
    result_t call()
    {
        if (m_locker.lock(this)) {
            result_t hr = process();
            if (hr != CALL_E_PENDDING) {
                m_locker.unlock(this);
                delete this;

                return hr;
            }
        }

        return CALL_E_PENDDING;
    }

    virtual int32_t process()
    {
        return 0;
    }

    virtual void proc()
    {
        ready(process());
    }

    result_t before()
    {
        result_t hr;

        if (!m_comp) {
            hr = _rbd_aio_create_completion((void*)this, complete_callback, &m_comp);
            if (hr < 0)
                return CHECK_ERROR(hr);
        }

        return 0;
    }

    void ready(int32_t v)
    {
        m_locker.unlock(this);
        m_ac->apost(v);
        delete this;
    }
    void onready()
    {
        proc();
    }

public:
    static void complete_callback(rbd_completion_t comp, void* arg)
    {
        ((asyncRbdCallback*)arg)->onready();
    }

public:
    rbd_completion_t m_comp;
    obj_ptr<RbdImage> m_pThis;
    AsyncEvent* m_ac;
    exlib::Locker& m_locker;
};

result_t RbdImage::read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    class asyncRead : public asyncRbdCallback {
    public:
        asyncRead(int64_t bytes, obj_ptr<Buffer_base>& retVal, RbdImage* pThis,
            AsyncEvent* ac, exlib::Locker& locker)
            : asyncRbdCallback(pThis, ac, locker)
            , m_bytes(bytes)
            , m_retVal(retVal)
        {
        }
        virtual result_t process()
        {
            result_t hr;

            hr = before();
            if (hr < 0)
                return hr;

            m_buf.resize(m_bytes);
            char* p = &m_buf[0];

            hr = _rbd_aio_read(m_pThis->m_image, m_pThis->m_off.value(), m_bytes, p, m_comp);
            if (hr < 0)
                return CHECK_ERROR(hr);

            return CALL_E_PENDDING;
        }

        virtual void proc()
        {
            int64_t nRead;

            nRead = _rbd_aio_get_return_value(m_comp);
            if (nRead < 0) {
                CHECK_ERROR(nRead);
                ready(nRead);
            } else if (nRead == 0)
                ready(CALL_RETURN_NULL);
            else {
                m_pThis->m_off.add(nRead);
                m_buf.resize(nRead);
                m_retVal = new Buffer(m_buf);
                ready(0);
            }
        }

    public:
        int64_t m_bytes;
        exlib::string m_buf;
        obj_ptr<Buffer_base>& m_retVal;
    };

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int64_t lbytes = bytes;
    result_t hr;

    if (lbytes < 0) {
        uint64_t bytes;
        hr = _rbd_get_size(m_image, &bytes);
        if (hr < 0)
            return CHECK_ERROR(hr);

        lbytes = bytes;
    }

    return (new asyncRead(lbytes, retVal, this, ac, m_lockRead))->call();
}

result_t RbdImage::write(Buffer_base* data, AsyncEvent* ac)
{
    class asyncWrite : public asyncRbdCallback {
    public:
        asyncWrite(exlib::string data, RbdImage* pThis,
            AsyncEvent* ac, exlib::Locker& locker)
            : asyncRbdCallback(pThis, ac, locker)
            , m_data(data)
        {
        }
        virtual result_t process()
        {
            result_t hr;

            hr = before();
            if (hr < 0)
                return hr;

            hr = _rbd_aio_write(m_pThis->m_image, m_pThis->m_off.value(), m_data.length(), m_data.c_str(), m_comp);
            if (hr < 0)
                return CHECK_ERROR(hr);

            return CALL_E_PENDDING;
        }

        virtual void proc()
        {
            result_t hr;

            hr = _rbd_aio_get_return_value(m_comp);
            if (hr < 0) {
                CHECK_ERROR(hr);
                ready(hr);
            } else {
                m_pThis->m_off.add(m_data.length());
                ready(0);
            }
        }

    public:
        exlib::string m_data;
    };

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strBuf;
    data->toString(strBuf);

    return (new asyncWrite(strBuf, this, ac, m_lockWrite))->call();
}

result_t RbdImage::close(AsyncEvent* ac)
{
    result_t hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_image == NULL)
        return 0;

    hr = _rbd_close(m_image);
    if (hr < 0)
        return CHECK_ERROR(hr);
    m_image = NULL;

    return 0;
}
void RbdImage::close()
{
    if (m_image)
        _rbd_close(m_image);
}

result_t RbdImage::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
{
    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return io_base::copyStream(this, stm, bytes, retVal, ac);
}

result_t RbdImage::seek(int64_t offset, int32_t whence)
{
    result_t hr;
    uint64_t size;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    hr = _rbd_get_size(m_image, &size);
    if (hr < 0)
        return CHECK_ERROR(hr);

    if (whence == SEEK_SET)
        m_off = offset;
    else if (whence == SEEK_CUR)
        m_off.add(offset);
    else if (whence == SEEK_END)
        m_off = size + offset;
    else
        return CHECK_ERROR(-EINVAL);

    if (m_off.value() < 0)
        return CHECK_ERROR(-EINVAL);

    if (m_off.value() > size)
        m_off = size;

    return 0;
}

result_t RbdImage::tell(int64_t& retVal)
{
    retVal = m_off.value();
    return 0;
}

result_t RbdImage::rewind()
{
    m_off = 0;
    return 0;
}

result_t RbdImage::size(int64_t& retVal)
{
    uint64_t size;
    result_t hr;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!_rbd_get_size)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    hr = _rbd_get_size(m_image, &size);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = size;

    return 0;
}

result_t RbdImage::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    result_t hr;

    hr = cc_read(-1, retVal);
    if (hr < 0)
        return hr;

    return 0;
}

result_t RbdImage::eof(bool& retVal)
{
    int64_t p, sz;
    tell(p);
    size(sz);
    retVal = p == sz;
    return 0;
}

result_t RbdImage::truncate(int64_t bytes, AsyncEvent* ac)
{
    return resize(bytes, ac);
}

result_t RbdImage::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<Stat> st = new Stat();
    int64_t sz;
    date_t time;

    size(sz);
    get_create_timestamp(time);
    st->init();
    st->size = sz;
    st->ctime = time;

    retVal = st;
    return 0;
}

result_t RbdImage::get_stripe_unit(int64_t& retVal)
{
    uint64_t unit;
    result_t hr;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!_rbd_get_stripe_unit)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    hr = _rbd_get_stripe_unit(m_image, &unit);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = unit;
    return 0;
}

result_t RbdImage::get_stripe_count(int64_t& retVal)
{
    uint64_t cnt;
    result_t hr;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!_rbd_get_stripe_unit)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    hr = _rbd_get_stripe_unit(m_image, &cnt);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = cnt;
    return 0;
}

result_t RbdImage::get_features(int64_t& retVal)
{
    uint64_t features;
    result_t hr;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!_rbd_get_features)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    hr = _rbd_get_features(m_image, &features);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = features;
    return 0;
}

result_t RbdImage::get_create_timestamp(date_t& retVal)
{
    result_t hr;
    struct timespec tm;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!_rbd_get_create_timestamp)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    hr = _rbd_get_create_timestamp(m_image, &tm);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = (double)tm.tv_nsec / 1000.0;

    return 0;
}
result_t RbdImage::get_block_name_prefix(exlib::string& retVal)
{
    result_t hr;

    exlib::string name;
    char* p;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!_rbd_get_block_name_prefix)
        return CHECK_ERROR(Runtime::setError("Symbol not found"));

    name.resize(MAX_BLOCK_NAME_PREFIX);
    p = &name[0];

    hr = _rbd_get_block_name_prefix(m_image, p, MAX_BLOCK_NAME_PREFIX);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = name.c_str();

    return 0;
}

result_t RbdImage::resize(int64_t bytes, AsyncEvent* ac)
{
    result_t hr;

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (bytes < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    hr = _rbd_resize(m_image, bytes);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}
result_t RbdImage::flush(AsyncEvent* ac)
{
    class asyncFlush : public asyncRbdCallback {
    public:
        asyncFlush(RbdImage* pThis, AsyncEvent* ac, exlib::Locker& locker)
            : asyncRbdCallback(pThis, ac, locker)
        {
        }

    public:
        virtual result_t process()
        {
            result_t hr;

            hr = before();
            if (hr < 0)
                return hr;

            hr = _rbd_aio_flush(m_pThis->m_image, m_comp);
            if (hr < 0)
                return CHECK_ERROR(hr);

            return CALL_E_PENDDING;
        }
        virtual void proc()
        {
            result_t hr;

            hr = _rbd_aio_get_return_value(m_comp);
            if (hr < 0) {
                CHECK_ERROR(hr);
                ready(hr);
            } else {
                ready(0);
            }
        }
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return (new asyncFlush(this, ac, m_lockWrite))->call();
}

result_t RbdImage::open(rados_ioctx_t io, exlib::string name, exlib::string snapshot)
{
    result_t hr;

    hr = load_librbd();
    if (hr < 0)
        return CHECK_ERROR(hr);

    if (!io)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (snapshot == "")
        hr = _rbd_open(io, name.c_str(), &m_image, NULL);
    else
        hr = _rbd_open(io, name.c_str(), &m_image, snapshot.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::createSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;

    hr = _rbd_snap_create(m_image, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::removeSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;

    hr = _rbd_snap_remove(m_image, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::rollbackSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;

    hr = _rbd_snap_rollback(m_image, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::listSnaps(v8::Local<v8::Array>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;
    int32_t max_size = 128;
    int32_t i;
    rbd_snap_info_t snaps[max_size];
    exlib::string snapname;
    Isolate* isolate = holder();
    retVal = v8::Array::New(isolate->m_isolate);

    hr = _rbd_snap_list(m_image, snaps, &max_size);
    if (hr < 0)
        return CHECK_ERROR(hr);

    for (i = 0; i < hr; i++) {
        snapname = snaps[i].name;
        retVal->Set(i, isolate->NewString(snapname));
    }

    return 0;
}

result_t RbdImage::protectSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;

    hr = _rbd_snap_protect(m_image, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::unprotectSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;

    hr = _rbd_snap_unprotect(m_image, snapname.c_str());
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::setSnap(exlib::string snapname, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;
    const char* snap = snapname.c_str();

    hr = _rbd_snap_set(m_image, snap[0] == '\0' ? NULL : snap);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t RbdImage::isSnapProtected(exlib::string snapname, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_image)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;
    int32_t protect;

    hr = _rbd_snap_is_protected(m_image, snapname.c_str(), &protect);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = protect > 0;

    return 0;
}

#else
result_t RbdImage::read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::write(Buffer_base* data, AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::close(AsyncEvent* ac)
{
    return 0;
}
void RbdImage::close()
{
    return 0;
}
result_t RbdImage::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::seek(int64_t offset, int32_t whence)
{
    return 0;
}
result_t RbdImage::tell(int64_t& retVal)
{
    return 0;
}
result_t RbdImage::rewind()
{
    return 0;
}
result_t RbdImage::size(int64_t& retVal)
{
    return 0;
}
result_t RbdImage::readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::eof(bool& retVal)
{
    return 0;
}
result_t RbdImage::truncate(int64_t bytes, AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::get_stripe_unit(int64_t& retVal)
{
    return 0;
}
result_t RbdImage::get_stripe_count(int64_t& retVal)
{
    return 0;
}
result_t RbdImage::get_features(int64_t& retVal)
{
    return 0;
}
result_t RbdImage::get_create_timestamp(date_t& retVal)
{
    return 0;
}
result_t RbdImage::get_block_name_prefix(exlib::string& retVal)
{
    return 0;
}
result_t RbdImage::resize(int64_t bytes, AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::flush(AsyncEvent* ac)
{
    return 0;
}
result_t RbdImage::open(rados_ioctx_t io, exlib::string name, exlib::string snapshot)
{
    return 0;
}

result_t RbdImage::createSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::removeSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::rollbackSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::listSnaps(obj_ptr<List_base>& retVal, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::protectSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::unprotectSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::setSnap(exlib::string snapname, AsyncEvent* ac)
{
    return 0;
}

result_t RbdImage::isSnapProtected(exlib::string snapname, bool& retVal, AsyncEvent* ac)
{
    return 0;
}
#endif
}
