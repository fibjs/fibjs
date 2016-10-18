/*
 * Service.h
 *
 *  Created on: Oct 15, 2016
 *      Author: lion
 */

#include "ifs/Service.h"

#ifndef _SERVICE_H_
#define _SERVICE_H_

namespace fibjs
{

class Service : public Service_base
{
public:
    Service(exlib::string name) : m_name(name)
    {}

public:
    EVENT_SUPPORT();

public:
    // Service_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t set_name(exlib::string newVal);
    virtual result_t install(exlib::string cmd, exlib::string displayName, exlib::string description);
    virtual result_t remove();
    virtual result_t start();
    virtual result_t stop();
    virtual result_t restart();
    virtual result_t run(AsyncEvent* ac);
    virtual result_t isInstalled(bool& retVal);
    virtual result_t isRunning(bool& retVal);
    virtual result_t onstop(v8::Local<v8::Function> func, int32_t& retVal);
    virtual result_t onpause(v8::Local<v8::Function> func, int32_t& retVal);
    virtual result_t oncontinue(v8::Local<v8::Function> func, int32_t& retVal);

private:
    exlib::string m_name;
};

inline result_t Service::get_name(exlib::string& retVal)
{
    retVal = m_name;
    return 0;
}

inline result_t Service::set_name(exlib::string newVal)
{
    m_name = newVal;
    return 0;
}

inline result_t Service::onstop(v8::Local<v8::Function> func, int32_t& retVal)
{
    return on("stop", func, retVal);
}

inline result_t Service::onpause(v8::Local<v8::Function> func, int32_t& retVal)
{
    return on("pause", func, retVal);
}

inline result_t Service::oncontinue(v8::Local<v8::Function> func, int32_t& retVal)
{
    return on("continue", func, retVal);
}



} /* namespace fibjs */
#endif /* _SERVICE_H_ */
