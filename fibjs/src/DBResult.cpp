/*
 * DBResult.cpp
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#include "DBResult.h"

namespace fibjs
{

result_t DBResult::_indexed_getter(uint32_t index, Variant& retVal)
{
	if (!m_size)
		return CALL_E_INVALID_CALL;

	return m_array._indexed_getter(index, retVal);
}

result_t DBResult::_indexed_setter(uint32_t index, Variant newVal)
{
	if (!m_size)
		return CALL_E_INVALID_CALL;

	return m_array._indexed_setter(index, newVal);
}

result_t DBResult::get_length(int32_t& retVal)
{
	if (!m_size)
		return CALL_E_INVALID_CALL;

	return m_array.get_length(retVal);
}

result_t DBResult::resize(int32_t sz)
{
	if (!m_size)
		return CALL_E_INVALID_CALL;

	return m_array.resize(sz);
}

result_t DBResult::slice(int32_t start, int32_t end,
		obj_ptr<List_base>& retVal)
{
	if (!m_size)
		return CALL_E_INVALID_CALL;

	return m_array.slice(start, end, retVal);
}

result_t DBResult::toJSON(const char* key, v8::Handle<v8::Object>& retVal)
{
	if (m_size)
		return m_array.toJSON(key, retVal);

	retVal = v8::Object::New();
	retVal->Set(v8::String::NewSymbol("affected", 8), v8::Number::New((double) m_affected));
	retVal->Set(v8::String::NewSymbol("insertId", 8), v8::Number::New((double) m_insertId));

	return 0;
}

result_t DBResult::get_insertId(int64_t& retVal)
{
	if (m_size)
		return CALL_E_INVALID_CALL;

	retVal = m_insertId;
	return 0;
}

result_t DBResult::get_affected(int64_t& retVal)
{
	if (m_size)
		return CALL_E_INVALID_CALL;

	retVal = m_affected;
	return 0;
}

result_t DBResult::get_fields(v8::Handle<v8::Array>& retVal)
{
	if (!m_size)
		return CALL_E_INVALID_CALL;

	return 0;
}

} /* namespace fibjs */
