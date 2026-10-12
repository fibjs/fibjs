#ifdef _WIN32

#include "ifs/fs.h"
#include "path.h"
#include "FileStream.h"
#include "AsyncUV.h"

#include <stdio.h>

namespace fibjs {

result_t lchmod_platform(exlib::string path, int32_t mode, AsyncHandle ac)
{
    return result_t::invalid_call();
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    // Node.js compatibility: a negative length is treated as zero
    if (len < 0)
        len = 0;

    // Node opens the path with 'r+', and so does this: a missing file is
    // reported instead of being created
    int32_t fd = _wopen(UTF8_W(path), _O_BINARY | _O_RDWR);
    if (fd < 0)
        return LastError("ftruncate", path);

    AutoReq req;
    int32_t ret = uv_fs_ftruncate(NULL, &req, fd, len, NULL);

    ::_close(fd);

    if (ret < 0)
        return result_t::from_system(ret).with_syscall("ftruncate").with_path(path);

    return result_t();
}
}

#endif
