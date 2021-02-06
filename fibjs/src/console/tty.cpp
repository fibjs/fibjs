#include "ifs/tty.h"

#ifdef _WIN32
#include <io.h>
#endif

namespace fibjs {

DECLARE_MODULE(tty);

result_t tty_base::isatty(int32_t fd, bool& retVal)
{
#ifndef _WIN32
    if (fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t hr = ::isatty(fd);
#else
    if (fd == -2) {
        retVal = true;
        return 0;
    }

    if (fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t hr = _isatty(fd);
    if (hr == FALSE && _lseek(fd, 0, SEEK_CUR) < 0)
        hr = TRUE;
#endif
    if (hr < 0)
        return CHECK_ERROR(LastError());

    retVal = !!hr;
    return 0;
}
}