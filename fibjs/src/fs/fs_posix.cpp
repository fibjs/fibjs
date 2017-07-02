#include "ifs/fs.h"
#include "List.h"
#include "File.h"

#ifndef _WIN32

#include <dirent.h>

#if defined(Darwin)
#include <copyfile.h>
#elif defined(Linux)
#include <sys/sendfile.h>
#endif

namespace fibjs {

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = ::access(path.c_str(), F_OK) == 0;
    return 0;
}

result_t fs_base::access(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::access(path.c_str(), mode) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::link(oldPath.c_str(), newPath.c_str()))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::unlink(exlib::string path, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::unlink(path.c_str()))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readlink(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    char buf[MAX_PATH_LENGTH];
    int32_t r;

    if ((r = ::readlink(path.c_str(), buf, MAX_PATH_LENGTH)) < 0)
        return CHECK_ERROR(LastError());

    buf[r] = '\0';
    retVal = buf;
    return 0;
}

result_t fs_base::realpath(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    char buf[MAX_PATH_LENGTH];

    if (::realpath(path.c_str(), buf) == NULL)
        return CHECK_ERROR(LastError());

    retVal = buf;
    return 0;
}

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::symlink(target.c_str(), linkpath.c_str()) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::truncate(path.c_str(), len) < 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::mkdir(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::mkdir(path.c_str(), mode))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rmdir(exlib::string path, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::rmdir(path.c_str()))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::chmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::chmod(path.c_str(), mode))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::lchmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

#ifdef Linux
    if (::fchmodat(AT_FDCWD, path.c_str(), mode, AT_SYMLINK_NOFOLLOW))
#else
    if (::lchmod(path.c_str(), mode))
#endif
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::chown(path.c_str(), uid, gid))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::lchown(path.c_str(), uid, gid))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rename(exlib::string from, exlib::string to,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::rename(from.c_str(), to.c_str()))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::copy(exlib::string from, exlib::string to, AsyncEvent* ac)
{
#if defined(FreeBSD)
	return CHECK_ERROR(CALL_E_INVALID_CALL);
#else

    int input, output;
    if ((input = ::open(from.c_str(), O_RDONLY)) == -1)
        return CHECK_ERROR(LastError());

    if ((output = ::open(to.c_str(), O_RDWR | O_CREAT, 0666)) == -1) {
        ::close(input);
        return CHECK_ERROR(LastError());
    }

#if defined(Darwin)
    int result = fcopyfile(input, output, 0, COPYFILE_ALL);
#else
    off_t bytesCopied = 0;
    struct stat fileinfo = { 0 };
    fstat(input, &fileinfo);
    int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
#endif

    ::close(input);
    ::close(output);

    if (result < 0)
        return CHECK_ERROR(LastError());

    return 0;
#endif
}

result_t fs_base::readdir(exlib::string path, obj_ptr<List_base>& retVal,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    DIR* dp;
    struct dirent* ep;
    exlib::string fpath;
    obj_ptr<List> oa;

    dp = ::opendir(path.c_str());
    if (dp == NULL)
        return CHECK_ERROR(LastError());

    oa = new List();

    while ((ep = ::readdir(dp)))
        if (qstrcmp(ep->d_name, ".") && qstrcmp(ep->d_name, ".."))
            oa->append(ep->d_name);

    ::closedir(dp);

    retVal = oa;

    return 0;
}
}

#endif
