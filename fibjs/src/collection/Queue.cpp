/*
 * Queue.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: lion
 */

#include <Queue.h>

namespace fibjs
{

result_t Queue_base::_new(int32_t size, obj_ptr<Queue_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new Queue(size);
    return 0;
}

result_t Queue::add(v8::Local<v8::Value> e, bool &retVal)
{
    if ((int)m_list.size() == m_size)
        return CHECK_ERROR(Runtime::setError("Queue: queue is full."));

    m_list.push_back(e);
    retVal = true;

    return 0;
}

result_t Queue::offer(v8::Local<v8::Value> e, bool &retVal)
{
    if ((int)m_list.size() == m_size)
    {
        retVal = false;
        return 0;
    }

    m_list.push_back(e);
    retVal = true;

    return 0;
}

result_t Queue::remove(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CHECK_ERROR(CALL_E_EMPTY);

    retVal = *m_list.begin();
    m_list.pop_front();
    return 0;
}

result_t Queue::poll(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CALL_RETURN_NULL;

    retVal = *m_list.begin();
    m_list.pop_front();
    return 0;
}

result_t Queue::element(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CHECK_ERROR(CALL_E_EMPTY);

    retVal = *m_list.begin();

    return 0;
}

result_t Queue::peek(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CALL_RETURN_NULL;

    retVal = *m_list.begin();

    return 0;
}

result_t Queue::clear()
{
    m_list.clear();
    return 0;
}

result_t Queue::toArray(v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> a = v8::Array::New(isolate, (int) m_list.size());
    int32_t i = 0;

    for (std::list<VariantEx>::iterator it = m_list.begin(); it != m_list.end();
            ++it)
        a->Set(i++, *it);

    retVal = a;
    return 0;
}

result_t Queue::get_length(int32_t &retVal)
{
    retVal = (int)m_list.size();
    return 0;
}

result_t Queue::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    result_t hr;
    v8::Local<v8::Array> as;

    hr = toArray(as);
    if (hr < 0)
        return hr;

    retVal = as;
    return 0;
}

} /* namespace fibjs */
