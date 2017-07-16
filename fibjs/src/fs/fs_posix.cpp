#ifndef _WIN32

#include "ifs/fs.h"
#include "List.h"
#include "File.h"

#include <dirent.h>

#if defined(Darwin)
#include <copyfile.h>
#elif defined(Linux)
#include <sys/sendfile.h>
#endif

#ifdef FreeBSD
#include <sys/mman.h>

inline int _copyfile(int ifd, int ofd)
{
    size_t file_size, n;
    int buf_mmapped;
    struct stat sb;
    char *b, *buf;
    ssize_t nr, nw;

    if (fstat(ifd, &sb) < 0)
        return (-1);

    if (sb.st_size == 0)
        return (0);

    buf = NULL;
    buf_mmapped = 0;
    file_size = (size_t)sb.st_size;

    buf = (char*)mmap(NULL, file_size, PROT_READ, MAP_SHARED, ifd, (off_t)0);
    if (buf != MAP_FAILED)
        buf_mmapped = 1;
    else
        buf = NULL;

    if (buf_mmapped == false) {
        if ((buf = (char*)malloc(file_size)) == NULL)
            return (-1);
        b = buf;
        for (n = file_size; n > 0; n -= (size_t)nr, b += nr) {
            if ((nr = read(ifd, b, n)) < 0) {
                free(buf);
                return (-1);
            }
        }
    }

    for (n = file_size, b = buf; n > 0; n -= (size_t)nw, b += nw)
        if ((nw = write(ofd, b, n)) <= 0)
            break;

    if (buf_mmapped && munmap(buf, file_size) < 0)
        return (-1);

    if (!buf_mmapped)
        free(buf);

    return (n > 0 ? -1 : 0);
}
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

result_t fs_base::fchmod(int32_t fd, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::fchmod(fd, mode))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::fchown(int32_t fd, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::fchown(fd, uid, gid))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::fdatasync(int32_t fd, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

#if defined(Darwin) || defined(FreeBSD)
    if (::fcntl(fd, F_FULLFSYNC))
        return CHECK_ERROR(LastError());
#else
    if (::fdatasync(fd))
        return CHECK_ERROR(LastError());
#endif
    return 0;
}

result_t fs_base::fsync(int32_t fd, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::fsync(fd))
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
    int input, output;
    if ((input = ::open(from.c_str(), O_RDONLY)) == -1)
        return CHECK_ERROR(LastError());

    if ((output = ::open(to.c_str(), O_RDWR | O_CREAT, 0666)) == -1) {
        ::close(input);
        return CHECK_ERROR(LastError());
    }

#if defined(FreeBSD)
    int result = _copyfile(input, output);
#elif defined(Darwin)
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
