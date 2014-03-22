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

result_t os_base::get_hostname(std::string &retVal)
{
    char s[255];

    if (gethostname(s, 255) < 0)
        return LastError();

    retVal = s;
    return 0;
}

result_t os_base::get_arch(std::string &retVal)
{
#ifdef x64
    retVal = "x86_64";
#else
    retVal = "i686";
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

result_t os_base::get_timezone(int32_t &retVal)
{
    retVal = Runtime::now().m_pDateCache->LocalOffset();
    return 0;
}

}

