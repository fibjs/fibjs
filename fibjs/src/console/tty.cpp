#include "ifs/tty.h"

#ifdef _WIN32
#include <io.h>
#endif

namespace fibjs {

DECLARE_MODULE(tty);

result_t tty_base::isatty(int32_t fd, bool& retVal)
{
    if (fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

#ifndef _WIN32
    int32_t hr = ::isatty(fd);
#else
    int32_t hr = _isatty(fd);
#endif
    if (hr < 0)
        return CHECK_ERROR(LastError());
    if (hr)
        retVal = true;
    else
        retVal = false;
    return 0;
}
}