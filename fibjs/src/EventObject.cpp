/*
 * Event.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "Event.h"

namespace fibjs
{

result_t EventObject_base::_new(obj_ptr<EventObject_base>& retVal)
{
	retVal = new Event();
	return 0;
}

result_t object_base::on(const char* ev, v8::Handle<v8::Function> func)
{
	return 0;
}

result_t object_base::once(const char* ev, v8::Handle<v8::Function> func)
{
	return 0;
}

result_t object_base::off(const char* ev, v8::Handle<v8::Function> func)
{
	return 0;
}

result_t object_base::trigger(const char* ev, const v8::Arguments& args)
{
	return 0;
}


}
