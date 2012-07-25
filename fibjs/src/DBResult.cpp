/*
 * DBResult.cpp
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#include "DBResult.h"

namespace fibjs
{

result_t DBResult::_indexed_getter(uint32_t index, obj_ptr<object_base>& retVal)
{
	return m_array._indexed_getter(index, retVal);
}

result_t DBResult::_indexed_setter(uint32_t index, obj_ptr<object_base>& newVal)
{
	return m_array._indexed_setter(index, newVal);
}

result_t DBResult::get_length(int32_t& retVal)
{
	return m_array.get_length(retVal);
}

result_t DBResult::resize(int32_t sz)
{
	return m_array.resize(sz);
}

result_t DBResult::slice(int32_t start, int32_t end,
		obj_ptr<ObjectArray_base>& retVal)
{
	return m_array.slice(start, end, retVal);
}

result_t DBResult::toJSON(const char* key, v8::Handle<v8::Object>& retVal)
{
	return m_array.toJSON(key, retVal);
}

result_t DBResult::get_insertId(int64_t& retVal)
{
	return 0;
}

result_t DBResult::get_affected(int64_t& retVal)
{
	return 0;
}

result_t DBResult::get_fields(v8::Handle<v8::Array>& retVal)
{
	return 0;
}

} /* namespace fibjs */
