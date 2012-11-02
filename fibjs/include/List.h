/*
 * List.h
 *
 *  Created on: Apr 11, 2012
 *      Author: lion
 */

#include "ifs/List.h"
#include "QuickArray.h"

#ifndef VARIANTLIST_H_
#define VARIANTLIST_H_

namespace fibjs
{

class List: public List_base
{
public:
	// object_base
	virtual result_t toJSON(const char* key, v8::Handle<v8::Value>& retVal);

public:
	// ObjectArray_base
	virtual result_t _indexed_getter(uint32_t index, Variant& retVal);
	virtual result_t _indexed_setter(uint32_t index, Variant newVal);
	virtual result_t get_length(int32_t& retVal);
	virtual result_t resize(int32_t sz);
	virtual result_t append(Variant v);
	virtual result_t slice(int32_t start, int32_t end,
			obj_ptr<List_base>& retVal);

public:
	class array
	{
	public:
		result_t _indexed_getter(uint32_t index, Variant& retVal);
		result_t _indexed_setter(uint32_t index, Variant newVal);
		result_t get_length(int32_t& retVal);
		result_t resize(int32_t sz);
		result_t append(Variant v);
		result_t slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal);
		result_t toJSON(const char* key, v8::Handle<v8::Value>& retVal);

	public:
		void append(object_base* newVal)
		{
			Variant v;
			v = newVal;
			append(v);
		}

	private:
		QuickArray<VariantEx> m_array;
	};

public:
	void append(object_base* newVal)
	{
		m_array.append(newVal);
	}

private:
	array m_array;
};

} /* namespace fibjs */
#endif /* VARIANTLIST_H_ */
