/*
 * ObjectArray.h
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include <vector>
#include "ifs/ObjectArray.h"

#ifndef OBJECTARRAY_H_
#define OBJECTARRAY_H_

namespace fibjs
{

class ObjectArray : public ObjectArray_base
{
public:
	// ObjectArray_base
	virtual result_t _indexed_getter(uint32_t index, obj_ptr<object_base>& retVal);
	virtual result_t _indexed_setter(uint32_t index, obj_ptr<object_base> newVal);
	virtual result_t get_length(int32_t& retVal);
	virtual result_t resize(int32_t sz);
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<ObjectArray_base>& retVal);

public:
	void push(object_base* newVal)
	{
		m_array.push_back(newVal);
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal);

private:
	std::vector< obj_ptr<object_base> > m_array;
};

} /* namespace fibjs */
#endif /* OBJECTARRAY_H_ */
