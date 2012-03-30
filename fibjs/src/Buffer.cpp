#include "Buffer.h"

namespace fibjs
{

result_t Buffer::_indexed_getter(uint32_t index, int32_t& retVal)
{
    retVal = index + 1000;
    return 0;
}

result_t Buffer::_indexed_setter(uint32_t index, int32_t newVal)
{
    return 0;
}

result_t Buffer_base::_new(const char* str, const char* encoding, Buffer_base*& retVal)
{
    retVal = new Buffer();
    return 0;
}

}
