/*
 * Event.cpp
 *
 *  Created on: Apr 30, 2012
 *      Author: lion
 */

#include "object.h"
#include "Event.h"

namespace fibjs {

result_t Event_base::_new(bool value, obj_ptr<Event_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Event();
    if (value)
        retVal->set();

    return result_t();
}

result_t Event::acquire(bool blocking, bool& retVal, AsyncHandle ac)
{
    if (!blocking) {
        retVal = m_event.isSet();
        return result_t();
    }

    retVal = true;
    return wait(std::move(ac));
}

result_t Event::release()
{
    m_event.set();
    return result_t();
}

result_t Event::count(int32_t& retVal)
{
    retVal = m_event.count();
    return result_t();
}

result_t Event::isSet(bool& retVal)
{
    retVal = m_event.isSet();
    return result_t();
}

result_t Event::set()
{
    m_event.set();
    return result_t();
}

result_t Event::pulse()
{
    m_event.pulse();
    return result_t();
}

result_t Event::clear()
{
    m_event.reset();
    return result_t();
}

result_t Event::wait(AsyncHandle ac)
{
    // 受控同步快路径（C 类例外，见审计报告 §3-C）：事件已置位时无需等待，
    // 直接返回；未置位才进 async 相位。
    if (m_event.isSet())
        return result_t();

    if (ac.isSync())
        return result_t::nosync();

    m_event.wait();
    return result_t();
}
}
