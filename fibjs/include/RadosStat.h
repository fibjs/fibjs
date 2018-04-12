#include "ifs/RadosStat.h"
#include <rados/librados.h>

#ifndef _fj_RADOSSTAT_H
#define _fj_RADOSSTAT_H

namespace fibjs {

class RadosStat : public RadosStat_base {
public:
    RadosStat(int64_t size, double time)
        : m_size(size)
        , m_mtime(time)
    {
    }

public:
    // RadosStat_base
    virtual result_t get_size(int64_t& retVal);
    virtual result_t get_mtime(date_t& retVal);

private:
    int64_t m_size;
    date_t m_mtime;
};
}

#endif // RADOSSTAT_H
