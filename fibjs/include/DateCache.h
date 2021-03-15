/*
 * DataCache.h
 *
 *  Created on: Jul 27, 2013
 *      Author: lion
 */

#pragma once

#include "utils.h"

namespace fibjs {

class DateCache {
public:
    DateCache();
    ~DateCache();

public:
    int64_t ToLocal(int64_t time_ms);
    int64_t ToUTC(int64_t time_ms);
    int32_t LocalOffset();

private:
    void* m_pdc;
};
}
