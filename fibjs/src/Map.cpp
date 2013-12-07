/*
 * Map.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: lion
 */

#include "Map.h"

namespace fibjs
{

result_t Map_base::_new(obj_ptr<Map_base>& retVal)
{
	retVal = new Map();
	return 0;
}

result_t Map::get_size(int32_t& retVal)
{
	retVal = (int32_t) m_datas.size();
	return 0;
}

result_t Map::clear()
{
	m_datas.clear();
	return 0;
}

result_t Map::has(const char* name, bool& retVal)
{
	retVal = m_datas.find(name) != m_datas.end();
	return 0;
}

result_t Map::get(const char* name, Variant& retVal)
{
	std::map<std::string, VariantEx>::iterator it = m_datas.find(name);

	if (it == m_datas.end())
		return 0;

	retVal = it->second;
	return 0;
}

result_t Map::put(const char* name, Variant value)
{
	m_datas.insert(std::pair<std::string, Variant>(name, value));
	return 0;
}

result_t Map::remove(const char* name)
{
	m_datas.erase(name);
	return 0;
}

result_t Map::isEmpty(bool& retVal)
{
	retVal = m_datas.empty();
	return 0;
}

result_t Map::_named_getter(const char* property, Variant& retVal)
{
	get(property, retVal);
	return retVal.type() == Variant::VT_Undefined ? CALL_RETURN_NULL : 0;
}

result_t Map::_named_enumerator(v8::Handle<v8::Array>& retVal)
{
	int32_t i = 0;

	retVal = v8::Array::New((int)m_datas.size());
	std::map<std::string, VariantEx>::iterator iter;

	for (iter = m_datas.begin(); iter != m_datas.end(); iter++)
		retVal->Set(i++,
				v8::String::New(iter->first.c_str(),
						(int) iter->first.length()));

	return 0;
}

result_t Map::_named_setter(const char* property, Variant newVal)
{
	return put(property, newVal);
}

result_t Map::_named_deleter(const char* property,
		v8::Handle<v8::Boolean>& retVal)
{
	return remove(property);
}

} /* namespace fibjs */
