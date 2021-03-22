#ifndef _WIN32

#include "ifs/fs.h"
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

result_t fs_base::copy(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

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
