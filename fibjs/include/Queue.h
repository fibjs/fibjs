/*
 * Queue.h
 *
 *  Created on: Nov 28, 2013
 *      Author: lion
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "ifs/Queue.h"
#include <list>

namespace fibjs
{

class Queue: public Queue_base
{
public:
	Queue(int32_t size) :
			m_size(size)
	{
	}

public:
	// Queue_base
	virtual result_t add(Variant e, bool& retVal);
	virtual result_t offer(Variant e, bool& retVal);
	virtual result_t remove(Variant& retVal);
	virtual result_t poll(Variant& retVal);
	virtual result_t element(Variant& retVal);
	virtual result_t peek(Variant& retVal);
	virtual result_t clear();
	virtual result_t toArray(v8::Handle<v8::Array>& retVal);
	virtual result_t get_length(int32_t& retVal);

public:
	// Object
	virtual result_t toJSON(const char* key, v8::Handle<v8::Value>& retVal);

private:
	std::list<VariantEx> m_list;
	int32_t m_size;
};

} /* namespace fibjs */

#endif /* QUEUE_H_ */
