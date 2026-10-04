#ifdef _WIN32

#include "ifs/fs.h"
#include "path.h"
#include "FileStream.h"
#include "AsyncUV.h"

#include <stdio.h>

namespace fibjs {

result_t lchmod_platform(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncEvent* ac)
{
    if (ac->isSync())
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

    return setSystemErrorPayload(hr, "ftruncate", path);
}
}

#endif
