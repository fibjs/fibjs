/*
 * Map.h
 *
 *  Created on: Aug 23, 2012
 *      Author: lion
 */

#ifndef MAP_H_
#define MAP_H_

#include "ifs/Map.h"
#include "map"

namespace fibjs
{

class Map: public Map_base
{
public:
	// Map_base
	virtual result_t get_size(int32_t& retVal);
	virtual result_t clear();
	virtual result_t has(const char* name, bool& retVal);
	virtual result_t get(const char* name, v8::Handle<v8::Value>& retVal);
	virtual result_t put(v8::Handle<v8::Object> map);
	virtual result_t put(const char* name, v8::Handle<v8::Value> value);
	virtual result_t remove(const char* name);
	virtual result_t isEmpty(bool& retVal);
	virtual result_t _named_getter(const char* property, v8::Handle<v8::Value>& retVal);
	virtual result_t _named_enumerator(v8::Handle<v8::Array>& retVal);
	virtual result_t _named_setter(const char* property, v8::Handle<v8::Value> newVal);
	virtual result_t _named_deleter(const char* property, v8::Handle<v8::Boolean>& retVal);

private:
	std::map<std::string, VariantEx> m_datas;
};

} /* namespace fibjs */
#endif /* MAP_H_ */
