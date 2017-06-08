#include "ifs/RadosCluster.h"
#include <rados/librados.h>

#ifndef _fj_RADOSCLUSTER_H
#define _fj_RADOSCLUSTER_H

namespace fibjs {

class RadosCluster : public RadosCluster_base {
public:
    RadosCluster()
        : m_cluster(NULL)
    {
    }

    ~RadosCluster();

public:
    // RadosCluster_base
    virtual result_t connect(AsyncEvent* ac);
    virtual result_t createIoCtx(exlib::string poolName, obj_ptr<RadosIoCtx_base>& retVal, AsyncEvent* ac);
    virtual result_t shutdown(AsyncEvent* ac);

public:
    result_t open(exlib::string key);

public:
    result_t create(exlib::string cn, exlib::string un, exlib::string path);

protected:
    rados_t m_cluster;
};
}

#endif // RADOSCLUSTER_H
