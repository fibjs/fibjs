/*
 * TextColor.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "TextColor.h"

namespace fibjs
{

result_t TextColor::get_notice(qstring &retVal)
{
    retVal = m_notice;
    return 0;
}

result_t TextColor::set_notice(const char *newVal)
{
    m_notice = newVal;
    return 0;
}

result_t TextColor::get_warn(qstring &retVal)
{
    retVal = m_warn;
    return 0;
}

result_t TextColor::set_warn(const char *newVal)
{
    m_warn = newVal;
    return 0;
}

result_t TextColor::get_error(qstring &retVal)
{
    retVal = m_error;
    return 0;
}

result_t TextColor::set_error(const char *newVal)
{
    m_error = newVal;
    return 0;
}

result_t TextColor::get_highLight(qstring &retVal)
{
    retVal = m_highLight;
    return 0;
}

result_t TextColor::set_highLight(const char *newVal)
{
    m_highLight = newVal;
    return 0;
}

}
