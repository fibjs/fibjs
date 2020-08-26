/*
 * MemoryCloneStream.cpp
 *
 *  Created on: Jul 4, 2012
 *      Author: lion
 */

#include "object.h"
#include "Range.h"

namespace fibjs {

result_t Range_base::_new(SeekableStream_base* stm, exlib::string range, obj_ptr<Range_base>& retVal, v8::Local<v8::Object> This)
{
    int64_t sz, begin, end;

    stm->size(sz);

    int32_t pr;
    if ((pr = _parseRange(range, sz, begin, end)) == -1)
        pr = _parseRange("bytes=" + range, sz, begin, end);

    if (pr != 0)
        return CALL_E_INVALIDARG;

    retVal = new Range(stm, begin, end);
    return 0;
}

result_t Range_base::_new(SeekableStream_base* stm, int32_t begin, int32_t end, obj_ptr<Range_base>& retVal, v8::Local<v8::Object> This)
{
    if (begin < 0)
        return CALL_E_INVALIDARG;

    int64_t sz;
    stm->size(sz);

    if (begin > sz)
        return CALL_E_INVALIDARG;

    retVal = new Range(stm, begin, end);
    return 0;
}
}
