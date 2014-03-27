/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */


#ifndef _WIN32
#include <dirent.h>
#else
#include <stdio.h>
#endif


#include "ifs/fs.h"
#include "utf8.h"
#include "Stat.h"
#include "List.h"
#include "File.h"
#include "BufferedStream.h"

namespace fibjs
{

result_t fs_base::open(const char *fname, const char *mode,
                       obj_ptr<File_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    obj_ptr<File> pFile = new File();
    result_t hr;

    hr = pFile->open(fname, mode, ac);
    if (hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}

result_t fs_base::tmpFile(obj_ptr<File_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new File();
    return 0;
}

result_t fs_base::openTextStream(const char *fname, const char *mode,
                                 obj_ptr<BufferedStream_base> &retVal,
                                 exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    obj_ptr<File_base> pFile;
    result_t hr = open(fname, mode, pFile, ac);
    if (hr < 0)
        return hr;

    return BufferedStream_base::_new(pFile, retVal);
}

result_t fs_base::readFile(const char *fname, std::string &retVal,
                           exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    obj_ptr<File> f = new File();
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = f->open(fname, "r", ac);
    if (hr < 0)
        return hr;

    hr = f->readAll(buf, ac);
    f->close(ac);

    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return buf->toString(retVal);
}

result_t fs_base::writeFile(const char *fname, const char *txt,
                            exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    obj_ptr<File> f = new File();
    result_t hr;

    hr = f->open(fname, "w", ac);
    if (hr < 0)
        return hr;

    hr = f->Write(txt, (int) qstrlen(txt));
    f->close(ac);

    return hr;
}

result_t fs_base::stat(const char *path, obj_ptr<Stat_base> &retVal,
                       exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    obj_ptr<Stat> pStat = new Stat();

    result_t hr = pStat->getStat(path);
    if (hr < 0)
        return hr;

    retVal = pStat;

    return 0;
}

#ifndef _WIN32

result_t fs_base::exists(const char *path, bool &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = access(path, F_OK) == 0;
    return 0;
}

result_t fs_base::unlink(const char *path, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::unlink(path))
        return LastError();

    return 0;
}

result_t fs_base::mkdir(const char *path, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::mkdir(path, 715))
        return LastError();

    return 0;
}

result_t fs_base::rmdir(const char *path, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::rmdir(path))
        return LastError();

    return 0;
}

result_t fs_base::chmod(const char *path, int32_t mode, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::chmod(path, mode))
        return LastError();

    return 0;
}

result_t fs_base::rename(const char *from, const char *to,
                         exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::rename(from, to))
        return LastError();

    return 0;
}

result_t fs_base::readdir(const char *path, obj_ptr<List_base> &retVal,
                          exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    DIR *dp;
    struct dirent *ep;
    std::string fpath;
    result_t hr;
    obj_ptr<List> oa;

    dp = ::opendir(path);
    if (dp == NULL)
        return LastError();

    oa = new List();

    while ((ep = ::readdir(dp)))
    {
        obj_ptr<Stat_base> fstat;

        fpath = path;
        fpath += '/';
        fpath += ep->d_name;

        hr = stat(fpath.c_str(), fstat, ac);
        if (hr < 0)
            return hr;

        oa->append(fstat);
    }
    ::closedir(dp);

    retVal = oa;

    return 0;
}

#else

result_t fs_base::exists(const char *path, bool &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = _waccess(UTF8_W(path), 0) == 0;
    return 0;
}

result_t fs_base::unlink(const char *path, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::_wunlink(UTF8_W(path)))
        return LastError();

    return 0;
}

result_t fs_base::mkdir(const char *path, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::_wmkdir(UTF8_W(path)))
        return LastError();

    return 0;
}

result_t fs_base::rmdir(const char *path, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::_wrmdir(UTF8_W(path)))
        return LastError();

    return 0;
}

result_t fs_base::chmod(const char *path, int32_t mode, exlib::AsyncEvent *ac)
{
    return CALL_E_INVALID_CALL;
}

result_t fs_base::rename(const char *from, const char *to, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (::_wrename(UTF8_W(from), UTF8_W(to)))
        return LastError();

    return 0;
}

result_t fs_base::readdir(const char *path, obj_ptr<List_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    WIN32_FIND_DATAW fd;
    HANDLE hFind;
    std::wstring fpath;
    obj_ptr<List> oa;

    fpath = utf8to16String(path);
    fpath.append(L"/*", 2);

    hFind = FindFirstFileW(fpath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return LastError();

    oa = new List();

    do
    {
        obj_ptr<Stat> pStat = new Stat();
        pStat->fill(fd);
        oa->append(pStat);
    }
    while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    retVal = oa;

    return 0;
}

#endif

}

