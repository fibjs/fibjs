#include <string>
#include <list>

#include "ifs/Buffer.h"

#ifndef _fj_BUFFER_H
#define _fj_BUFFER_H

namespace fibjs
{

class Buffer : public Buffer_base
{
public:
    result_t _indexed_getter(uint32_t index, int32_t& retVal);
    result_t _indexed_setter(uint32_t index, int32_t newVal);
    result_t get_length(int32_t& retVal);
    result_t resize(int32_t sz);
    result_t write(const char* str);
    result_t slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal);
    result_t toString(std::string& retVal);

private:
    std::string m_data;
};

}

#endif // BUFFER_H
