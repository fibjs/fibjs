/*
 * BlockQueue.cpp
 *
 *  Created on: Nov 28, 2013
 *      Author: lion
 */

#include <BlockQueue.h>

namespace fibjs
{

result_t BlockQueue_base::_new(int32_t size, obj_ptr<BlockQueue_base> &retVal,
                               v8::Local<v8::Object> This)
{
    retVal = new BlockQueue(size);
    return 0;
}

result_t BlockQueue::add(v8::Local<v8::Value> e, bool &retVal)
{
    if ((int32_t)m_list.size() == m_size)
        return CHECK_ERROR(Runtime::setError("BlockQueue: queue is full."));

    put(e);
    retVal = true;

    return 0;
}

result_t BlockQueue::offer(v8::Local<v8::Value> e, bool &retVal)
{
    if ((int32_t)m_list.size() == m_size)
    {
        retVal = false;
        return 0;
    }

    put(e);
    retVal = true;

    return 0;
}

result_t BlockQueue::remove(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CHECK_ERROR(CALL_E_EMPTY);

    return take(retVal);
}

result_t BlockQueue::poll(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CALL_RETURN_NULL;

    return take(retVal);
}

result_t BlockQueue::element(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CHECK_ERROR(CALL_E_EMPTY);

    retVal = *m_list.begin();

    return 0;
}

result_t BlockQueue::peek(v8::Local<v8::Value> &retVal)
{
    if (m_list.size() == 0)
        return CALL_RETURN_NULL;

    retVal = *m_list.begin();

    return 0;
}

result_t BlockQueue::clear()
{
    int32_t n = (int32_t)m_list.size();

    if (n > 0)
    {
        m_list.clear();

        while (n-- > 0)
        {
            m_semTake.wait();
            m_semPut.post();
        }
    }

    return 0;
}

result_t BlockQueue::toArray(v8::Local<v8::Array> &retVal)
{
    v8::Local<v8::Array> a = v8::Array::New(holder()->m_isolate, (int32_t) m_list.size());
    int32_t i = 0;

    for (std::list<VariantEx>::iterator it = m_list.begin(); it != m_list.end();
            ++it)
        a->Set(i++, *it);

    retVal = a;
    return 0;
}

result_t BlockQueue::get_length(int32_t &retVal)
{
    retVal = (int32_t) m_list.size();
    return 0;
}

result_t BlockQueue::toJSON(const char *key, v8::Local<v8::Value> &retVal)
{
    result_t hr;
    v8::Local<v8::Array> as;

    hr = toArray(as);
    if (hr < 0)
        return hr;

    retVal = as;
    return 0;
}

result_t BlockQueue::put(v8::Local<v8::Value> e)
{
    m_semPut.wait();

    m_list.push_back(e);

    m_semTake.post();

    return 0;
}

result_t BlockQueue::take(v8::Local<v8::Value> &retVal)
{
    m_semTake.wait();

    retVal = *m_list.begin();
    m_list.pop_front();

    m_semPut.post();

    return 0;
}

} /* namespace fibjs */
