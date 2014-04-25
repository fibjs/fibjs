/*
 * console.h
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#ifndef _fj_CONSOLE_H
#define _fj_CONSOLE_H

#include "utils.h"
#include "TextColor.h"

namespace fibjs
{

class logger : public obj_base
{
public:
    class item : public asyncEvent
    {
    public:
        item(int32_t priority, std::string msg) :
            m_priority(priority), m_msg(msg)
        {
        }

    public:
        int32_t m_priority;
        std::string m_msg;
    };

public:
    static std::string &notice()
    {
        return get_std_color()->m_notice;
    }

    static std::string &warn()
    {
        return get_std_color()->m_warn;
    }

    static std::string &error()
    {
        return get_std_color()->m_error;
    }

    static std::string &highLight()
    {
        return get_std_color()->m_highLight;
    }

    static TextColor *get_std_color();

    static void std_out(const char *txt);

public:
    virtual result_t write(const item *data, exlib::AsyncEvent *ac) = 0;
};

}

#endif // _fj_CONSOLE_H
