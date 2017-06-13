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
    virtual result_t createSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t removeSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac);
    virtual result_t listOids(obj_ptr<List_base>& retVal, AsyncEvent* ac);
    virtual result_t listOids(Regex_base* reg, obj_ptr<List_base>& retVal, AsyncEvent* ac);
    virtual result_t getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setXattr(exlib::string oid, exlib::string attr, exlib::string value, AsyncEvent* ac);
    virtual result_t rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac);
    virtual result_t getXattrs(exlib::string oid, v8::Local<v8::Object>& retVal, AsyncEvent* ac);
    virtual result_t destroy(AsyncEvent* ac);

public:
    result_t create(rados_t cluster, exlib::string poolName);

public:
    rados_ioctx_t m_ioctx;
};
}

#endif // RADOSIOCTX_H
