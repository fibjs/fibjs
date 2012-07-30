/*
 * ObjectArray.h
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/ObjectArray.h"
#include "QuickArray.h"

#ifndef OBJECTARRAY_H_
#define OBJECTARRAY_H_

namespace fibjs
{

class ObjectArray : public ObjectArray_base
{
public:
	// object_base
	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal);

public:
	// ObjectArray_base
	virtual result_t _indexed_getter(uint32_t index, obj_ptr<object_base>& retVal);
	virtual result_t _indexed_setter(uint32_t index, obj_ptr<object_base>& newVal);
	virtual result_t get_length(int32_t& retVal);
	virtual result_t resize(int32_t sz);
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<ObjectArray_base>& retVal);

public:
	class array
	{
	public:
		result_t _indexed_getter(uint32_t index, obj_ptr<object_base>& retVal);
		result_t _indexed_setter(uint32_t index, obj_ptr<object_base>& newVal);
		result_t get_length(int32_t& retVal);
		result_t resize(int32_t sz);
		result_t slice(int32_t start, int32_t end, obj_ptr<ObjectArray_base>& retVal);
		result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal);

	public:
		void push(object_base* newVal)
		{
			m_array.append(newVal);
		}

	private:
		QuickArray< obj_ptr<object_base> > m_array;
	};

public:
	void push(object_base* newVal)
	{
		m_array.push(newVal);
	}

private:
	array m_array;
};

} /* namespace fibjs */
#endif /* OBJECTARRAY_H_ */
