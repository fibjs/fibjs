/*
 * EventInfo.h
 *
 *  Created on: Mar 23, 2016
 *      Author: lion
 */

#include "ifs/EventInfo.h"
#include "map"

#ifndef EVENTINFO_H_
#define EVENTINFO_H_

namespace fibjs
{

class EventInfo: public EventInfo_base
{
public:
    EventInfo(obj_ptr<object_base> target, exlib::string type,
              int32_t code = 0, exlib::string reason = "") :
        m_target(target), m_type(type), m_code(code), m_reason(reason)
    {}

public:
    // EventInfo_base
    virtual result_t get_code(int32_t& retVal)
    {
        retVal = m_code;
        return 0;
    }

    virtual result_t get_reason(exlib::string& retVal)
    {
        retVal = m_reason;
        return 0;
    }

    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = m_type;
        return 0;
    }

    virtual result_t get_target(v8::Local<v8::Object>& retVal)
    {
        retVal = m_target->wrap();
        return 0;
    }

    virtual result_t _named_getter(const char* property, int32_t& retVal)
    {
        std::map<exlib::string, VariantEx>::iterator it = m_datas.find(property);

        if (it == m_datas.end())
            return CALL_RETURN_NULL;

        retVal = it->second;
        return 0;
    }

    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal)
    {
        int32_t i = 0;
        Isolate* isolate = holder();

        retVal = v8::Array::New(isolate->m_isolate, (int32_t)m_datas.size());
        std::map<exlib::string, VariantEx>::iterator iter;

        for (iter = m_datas.begin(); iter != m_datas.end(); iter++)
            retVal->Set(i++, isolate->NewFromUtf8(iter->first));

        return 0;
    }

    result_t put(exlib::string name, Variant value)
    {
        m_datas.insert(std::pair<exlib::string, VariantEx>(name, value));
        return 0;
    }

private:
    obj_ptr<object_base> m_target;
    exlib::string m_type;
    int32_t m_code;
    exlib::string m_reason;
    std::map<exlib::string, VariantEx> m_datas;
};

} /* namespace fibjs */
#endif /* EVENTINFO_H_ */
