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

Map::Map()
{
	// TODO Auto-generated constructor stub

}

Map::~Map()
{
	// TODO Auto-generated destructor stub
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

inline result_t _map(Map* o, v8::Handle<v8::Object> m,
		result_t (Map::*fn)(const char* name, Variant value))
{
	v8::Handle<v8::Array> ks = m->GetPropertyNames();
	int len = ks->Length();
	int i;

	for (i = 0; i < len; i++)
	{
		v8::Handle<v8::Value> k = ks->Get(i);
		v8::Handle<v8::Value> v = m->Get(k);

		(o->*fn)(*v8::String::Utf8Value(k), Variant(v));
	}

	return 0;
}

result_t Map::get(const char* name, Variant& retVal)
{
	std::map<std::string, Variant>::iterator it = m_datas.find(name);

	if (it == m_datas.end())
		return CALL_RETURN_NULL;

	retVal = it->second;
	return 0;
}

result_t Map::put(const char* name, Variant value)
{
	m_datas.insert(std::pair<std::string, Variant>(name, value));
	return 0;
}

result_t Map::put(v8::Handle<v8::Object> map)
{
	return _map(this, map, &Map::put);
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
	return get(property, retVal);
}

result_t Map::_named_setter(const char* property, Variant newVal)
{
	return put(property, newVal);
}

} /* namespace fibjs */
