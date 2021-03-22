#ifndef _WIN32

#include "ifs/fs.h"
#include "File.h"

#include <dirent.h>

#if defined(Darwin)
#include <copyfile.h>
#elif defined(Linux)
#include <sys/sendfile.h>
#endif

namespace fibjs {

class fs_initer {
public:
    fs_initer()
    {
        ::umask(0);
    }
} s_fs_initer;

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::truncate(path.c_str(), len) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::lchmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

#ifdef Linux
    if (::fchmodat(AT_FDCWD, path.c_str(), mode, AT_SYMLINK_NOFOLLOW))
#else
    if (::lchmod(path.c_str(), mode))
#endif
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<NArray>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!ac->isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    DIR* dp;
    struct dirent* ep;
    exlib::string fpath;
    obj_ptr<NArray> oa;

    dp = ::opendir(path.c_str());
    if (dp == NULL)
        return CHECK_ERROR(LastError());

    oa = new NArray();

    while ((ep = ::readdir(dp)))
        if (qstrcmp(ep->d_name, ".") && qstrcmp(ep->d_name, ".."))
            oa->append(ep->d_name);

    ::closedir(dp);

    retVal = oa;

    return 0;
}
}

#endif
