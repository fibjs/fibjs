/*
 * Chain.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "object.h"
#include "Chain.h"
#include "ifs/mq.h"

namespace fibjs {

result_t Chain_base::_new(std::vector<obj_ptr<Handler_base>>& hdlrs,
    obj_ptr<Chain_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Chain_base> chain = new Chain();
    chain->wrap(This);

    result_t hr = chain->append(hdlrs);
    if (hr < 0)
        return hr;

    retVal = chain;

    return 0;
}

result_t Chain::isRouting(bool& retVal)
{
    int32_t len = m_array.size();

    for (int i = 0; i < len; i++) {
        bool isRouting = false;
        m_array[i]->isRouting(isRouting);
        if (isRouting) {
            retVal = true;
            return 0;
        }
    }

    retVal = false;
    return 0;
}

result_t Chain::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    if (m_array.size() == 0)
        return CHECK_ERROR(Runtime::setError("Chain: empty chain."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncInvoke(m_array, v, ac))->post(0);
}

result_t Chain::append(Handler_base* hdlr)
{
    int32_t no = (int32_t)m_array.size();

    char strBuf[32];
    snprintf(strBuf, sizeof(strBuf), "handler_%d", no);

    SetPrivate(strBuf, hdlr->wrap());
    m_array.append(hdlr);

    return 0;
}

result_t Chain::append(std::vector<obj_ptr<Handler_base>>& hdlrs)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    int32_t len = hdlrs.size();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        hr = append(hdlrs[i]);
        if (hr < 0)
            return hr;
    }

    return 0;
}

} /* namespace fibjs */
