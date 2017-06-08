#include "ifs/RadosIoCtx.h"
#include <rados/librados.h>

#ifndef _fj_RADOSIOCTX_H
#define _fj_RADOSIOCTX_H

namespace fibjs {

class RadosIoCtx : public RadosIoCtx_base {
public:
    RadosIoCtx()
        : m_ioctx(NULL)
    {
    }
    ~RadosIoCtx();

public:
    // RadosIoCtx_base
    virtual result_t open(exlib::string key, obj_ptr<RadosStream_base>& retVal);
    virtual result_t remove(exlib::string key, AsyncEvent* ac);
    virtual result_t destroy(AsyncEvent* ac);

public:
    result_t create(rados_t cluster, exlib::string poolName);

public:
    rados_ioctx_t m_ioctx;
};
}

#endif // RADOSIOCTX_H
