#include "ifs/Buffer.h"

#ifndef BUFFER_H
#define BUFFER_H

namespace fibjs
{

class Buffer : public Buffer_base
{
public:
	result_t _indexed_getter(uint32_t index, int32_t& retVal);
	result_t _indexed_setter(uint32_t index, int32_t newVal);

};

}

#endif // BUFFER_H
