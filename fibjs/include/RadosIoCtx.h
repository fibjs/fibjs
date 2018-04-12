#include "ifs/RadosIoCtx.h"
#include <rados/librados.h>
#include <rados/librbd.h>
#include <pcre/pcre.h>

#ifndef _fj_RADOSIOCTX_H
#define _fj_RADOSIOCTX_H

namespace fibjs {

class RadosIoCtx : public RadosIoCtx_base {
public:
    class Attrs : public NObject {
    public:
        Attrs() {}
    };
    RadosIoCtx()
        : m_ioctx(NULL)
    {
    }
    ~RadosIoCtx();

public:
    // RadosIoCtx_base
    virtual result_t createImage(exlib::string name, int64_t size, int64_t stripe_unit, int64_t stripe_count, AsyncEvent* ac);
    virtual result_t cloneImage(exlib::string pName, exlib::string pSnapshot, RadosIoCtx_base* dstio, exlib::string cName, int64_t stripe_unit, int32_t stripe_count, AsyncEvent* ac);
    virtual result_t removeImage(exlib::string name, AsyncEvent* ac);
    virtual result_t renameImage(exlib::string src, exlib::string dst, AsyncEvent* ac);
    virtual result_t listImages(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t openImage(exlib::string name, exlib::string snapshot, obj_ptr<RbdImage_base>& retVal, AsyncEvent* ac);
    virtual result_t version(exlib::string& retVal);
    virtual result_t open(exlib::string key, obj_ptr<RadosStream_base>& retVal);
    virtual result_t remove(exlib::string key, AsyncEvent* ac);
    virtual result_t createSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t removeSnap(exlib::string snapname, AsyncEvent* ac);
    virtual result_t rollbackSnap(exlib::string oid, exlib::string snapname, AsyncEvent* ac);
    virtual result_t listOids(obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t listOids(exlib::string pattern, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t getXattr(exlib::string oid, exlib::string attr, exlib::string& retVal, AsyncEvent* ac);
    virtual result_t setXattr(exlib::string oid, exlib::string attr, exlib::string value, AsyncEvent* ac);
    virtual result_t rmXattr(exlib::string oid, exlib::string attr, AsyncEvent* ac);
    virtual result_t getXattrs(exlib::string oid, obj_ptr<NObject>& retVal, AsyncEvent* ac);
    virtual result_t destroy(AsyncEvent* ac);

public:
    result_t create(rados_t cluster, exlib::string poolName);

public:
    rados_ioctx_t m_ioctx;
};
}

#endif // RADOSIOCTX_H
