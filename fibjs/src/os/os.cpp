/*
 * os.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/os.h"

#ifndef _WIN32
# include <unistd.h>  // gethostname, sysconf
# include <sys/utsname.h>
#endif

namespace fibjs
{

DECLARE_MODULE(os);

result_t os_base::get_hostname(qstring &retVal)
{
    char s[255];

    if (gethostname(s, 255) < 0)
        return CHECK_ERROR(LastError());

    retVal = s;
    return 0;
}

result_t os_base::get_arch(qstring &retVal)
{
#ifdef amd64
    retVal = "amd64";
#elif defined(i386)
    retVal = "i686";
#elif defined(arm)
    retVal = "arm";
#elif defined(arm64)
    retVal = "arm64";
#endif

    return 0;
}

result_t os_base::time(const char *tmString, date_t &retVal)
{
    if (!tmString || !*tmString)
        retVal.now();
    else
        retVal.parse(tmString);

    return 0;
}

result_t os_base::dateAdd(date_t d, int32_t num, const char *part, date_t &retVal)
{
    retVal = d;
    if (!qstrcmp(part, "year"))
        retVal.add(num, date_t::_YEAR);
    else if (!qstrcmp(part, "month"))
        retVal.add(num, date_t::_MONTH);
    else if (!qstrcmp(part, "day"))
        retVal.add(num, date_t::_DAY);
    else if (!qstrcmp(part, "hour"))
        retVal.add(num, date_t::_HOUR);
    else if (!qstrcmp(part, "minute"))
        retVal.add(num, date_t::_MINUTE);
    else if (!qstrcmp(part, "second"))
        retVal.add(num, date_t::_SECOND);
    else return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

result_t os_base::get_timezone(int32_t &retVal)
{
    retVal = date_t::timezone();
    return 0;
}

}

