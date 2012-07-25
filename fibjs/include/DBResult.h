/*
 * DBResult.h
 *
 *  Created on: Jul 25, 2012
 *      Author: lion
 */

#include "ifs/DbResult.h"
#include "ObjectArray.h"

#ifndef DBRESULT_H_
#define DBRESULT_H_

namespace fibjs
{

class DBResult: public fibjs::DBResult_base
{
public:
	// ObjectArray_base
	virtual result_t _indexed_getter(uint32_t index, obj_ptr<object_base>& retVal);
	virtual result_t _indexed_setter(uint32_t index, obj_ptr<object_base>& newVal);
	virtual result_t get_length(int32_t& retVal);
	virtual result_t resize(int32_t sz);
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<ObjectArray_base>& retVal);

public:
	// DBResult_base
	virtual result_t get_insertId(int64_t& retVal);
	virtual result_t get_affected(int64_t& retVal);
	virtual result_t get_fields(v8::Handle<v8::Array>& retVal);

public:
	void push(object_base* newVal)
	{
		m_array.push(newVal);
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal);

private:
	ObjectArray::array m_array;
};

} /* namespace fibjs */
#endif /* DBRESULT_H_ */
