/*
 * DataCache.h
 *
 *  Created on: Jul 27, 2013
 *      Author: lion
 */

#include "utils.h"

#ifndef DATACACHE_H_
#define DATACACHE_H_

namespace v8
{
namespace internal
{

class _date_cache
{
public:
    _date_cache();
    ~_date_cache();

public:
    int64_t ToLocal(int64_t time_ms);
    int64_t ToUTC(int64_t time_ms);
    int32_t LocalOffset();

private:
    void *m_pdc;
};

}
}  // namespace v8::internal

namespace fibjs
{
typedef v8::internal::_date_cache DateCache;
}

#endif
