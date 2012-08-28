/*
 * List.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "List.h"

namespace fibjs
{

result_t List_base::_new(obj_ptr<List_base>& retVal)
{
	retVal = new List();
	return 0;
}

result_t List::_indexed_getter(uint32_t index, Variant& retVal)
{
	return m_array._indexed_getter(index, retVal);
}

result_t List::_indexed_setter(uint32_t index, Variant newVal)
{
	return m_array._indexed_setter(index, newVal);
}

result_t List::get_length(int32_t& retVal)
{
	return m_array.get_length(retVal);
}

result_t List::resize(int32_t sz)
{
	return m_array.resize(sz);
}

result_t List::slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal)
{
	return m_array.slice(start, end, retVal);
}

result_t List::toJSON(const char* key, v8::Handle<v8::Object>& retVal)
{
	return m_array.toJSON(key, retVal);
}

result_t List::array::_indexed_getter(uint32_t index, Variant& retVal)
{
	if (index >= m_array.size())
		return CALL_E_BADINDEX;

	retVal = m_array[index];

	return 0;
}

result_t List::array::_indexed_setter(uint32_t index, Variant newVal)
{
	if (index >= m_array.size())
		return CALL_E_BADINDEX;

	m_array[index] = newVal;

	return 0;
}

result_t List::array::get_length(int32_t& retVal)
{
	retVal = (int32_t) m_array.size();
	return 0;
}

result_t List::array::resize(int32_t sz)
{
	m_array.resize(sz);
	return 0;
}

result_t List::array::slice(int32_t start, int32_t end,
		obj_ptr<List_base>& retVal)
{
	return 0;
}

result_t List::array::toJSON(const char* key, v8::Handle<v8::Object>& retVal)
{
	v8::Handle<v8::Array> a = v8::Array::New((int) m_array.size());
	int i;

	for (i = 0; i < (int) m_array.size(); i++)
		if (m_array[i])
			a->Set(i, m_array[i]);

	retVal = a;

	return 0;
}

} /* namespace fibjs */
