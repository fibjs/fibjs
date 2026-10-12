#ifdef _WIN32

#include "ifs/fs.h"
#include "path.h"
#include "FileStream.h"
#include "AsyncUV.h"

#include <stdio.h>

namespace fibjs {

result_t lchmod_platform(exlib::string path, int32_t mode, AsyncHandle ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js compatibility: a negative length is treated as zero
    if (len < 0)
        len = 0;

    // Node opens the path with 'r+', and so does this: a missing file is
    // reported instead of being created
    int32_t fd = _wopen(UTF8_W(path), _O_BINARY | _O_RDWR);
    if (fd < 0)
        return CHECK_ERROR(LastError("ftruncate", path));

    AutoReq req;
    result_t hr = uv_fs_ftruncate(NULL, &req, fd, len, NULL);

    ::_close(fd);

    {
        result_t hr_err = hr;
        if (!hr_err.is_system_error())
            return hr_err;
        return result_t(result_t::inherit(std::move(hr_err))
            .with_system_errno()
            .with_syscall("ftruncate")
            .with_path(path));
    }
}
}

#endif
