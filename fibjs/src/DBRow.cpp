/*
 * DBRow.cpp
 *
 *  Created on: Jul 27, 2012
 *      Author: lion
 */

#include "DBRow.h"

namespace fibjs
{

result_t DBRow::_indexed_getter(uint32_t index, v8::Handle<v8::Value>& retVal)
{
	if (index >= m_cols.size())
		return CALL_E_BADINDEX;

	retVal = m_cols[index];

	return 0;
}

result_t DBRow::_named_getter(const char* property,
		v8::Handle<v8::Value>& retVal)
{
	int32_t i = m_fields->index(property);

	if (i >= 0)
		return _indexed_getter(i, retVal);

	return 0;
}

result_t DBRow::toJSON(const char* key, v8::Handle<v8::Object>& retVal)
{
	retVal = v8::Object::New();
	int32_t i;

	for (i = 0; i < (int32_t) m_cols.size(); i++)
	{
		std::string& s = m_fields->name(i);
		retVal->Set(v8::String::New(s.c_str(), s.length()), m_cols[i]);
	}

	return 0;
}

} /* namespace fibjs */
