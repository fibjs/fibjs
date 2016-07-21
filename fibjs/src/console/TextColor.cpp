/*
 * TextColor.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "object.h"
#include "TextColor.h"

namespace fibjs
{

result_t TextColor::get_notice(exlib::string &retVal)
{
    retVal = m_notice;
    return 0;
}

result_t TextColor::set_notice(exlib::string newVal)
{
    m_notice = newVal;
    return 0;
}

result_t TextColor::get_warn(exlib::string &retVal)
{
    retVal = m_warn;
    return 0;
}

result_t TextColor::set_warn(exlib::string newVal)
{
    m_warn = newVal;
    return 0;
}

result_t TextColor::get_error(exlib::string &retVal)
{
    retVal = m_error;
    return 0;
}

result_t TextColor::set_error(exlib::string newVal)
{
    m_error = newVal;
    return 0;
}

result_t TextColor::get_highLight(exlib::string &retVal)
{
    retVal = m_highLight;
    return 0;
}

result_t TextColor::set_highLight(exlib::string newVal)
{
    m_highLight = newVal;
    return 0;
}

}
