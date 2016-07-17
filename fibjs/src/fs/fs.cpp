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


#include "object.h"
#include "ifs/fs.h"
#include "utf8.h"
#include "Stat.h"
#include "List.h"
#include "File.h"
#include "BufferedStream.h"

namespace fibjs
{

DECLARE_MODULE(fs);

void init_fs()
{
#ifdef _WIN32
    struct stat64 st;
    ::stat64(".", &st);
#endif
}

result_t fs_base::open(const char *fname, const char *flags,
                       obj_ptr<File_base> &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<File> pFile = new File();
    result_t hr;

    hr = pFile->open(fname, flags);
    if (hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}

result_t fs_base::tmpFile(obj_ptr<File_base> &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new File();
    return 0;
}

result_t fs_base::openTextStream(const char *fname, const char *flags,
                                 obj_ptr<BufferedStream_base> &retVal,
                                 AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<File_base> pFile;
    result_t hr = open(fname, flags, pFile, ac);
    if (hr < 0)
        return hr;

    return BufferedStream_base::_new(pFile, retVal);
}

result_t fs_base::readFile(const char *fname, exlib::string &retVal,
                           AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<File> f = new File();
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = f->open(fname, "r");
    if (hr < 0)
        return hr;

    hr = f->readAll(buf, ac);
    f->close(ac);

    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return buf->toString(retVal);
}

result_t fs_base::readLines(const char *fname, int32_t maxlines,
                            v8::Local<v8::Array> &retVal)
{
    obj_ptr<BufferedStream_base> pFile;
    result_t hr;

    hr = ac_openTextStream(fname, "r", pFile);
    if (hr < 0)
        return hr;

    return pFile->readLines(maxlines, retVal);
}

result_t fs_base::writeFile(const char *fname, const char *txt,
                            AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<File> f = new File();
    result_t hr;

    hr = f->open(fname, "w");
    if (hr < 0)
        return hr;

    hr = f->Write(txt, (int32_t) qstrlen(txt));
    f->close(ac);

    return hr;
}

result_t fs_base::stat(const char *path, obj_ptr<Stat_base> &retVal,
                       AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stat> pStat = new Stat();

    result_t hr = pStat->getStat(path);
    if (hr < 0)
        return hr;

    retVal = pStat;

    return 0;
}

#ifndef _WIN32

result_t fs_base::exists(const char *path, bool &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = access(path, F_OK) == 0;
    return 0;
}

result_t fs_base::unlink(const char *path, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::unlink(path))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::umask(int32_t mask, int32_t &retVal , AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = ::umask(mask);

    return 0;
}

result_t fs_base::mkdir(const char *path, int32_t mode, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::mkdir(path, mode))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rmdir(const char *path, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::rmdir(path))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::chmod(const char *path, int32_t mode, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::chmod(path, mode))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rename(const char *from, const char *to,
                         AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::rename(from, to))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readdir(const char *path, obj_ptr<List_base> &retVal,
                          AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    DIR *dp;
    struct dirent *ep;
    exlib::string fpath;
    result_t hr;
    obj_ptr<List> oa;

    dp = ::opendir(path);
    if (dp == NULL)
        return CHECK_ERROR(LastError());

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

result_t fs_base::exists(const char *path, bool &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = _waccess(UTF8_W(path), 0) == 0;
    return 0;
}

result_t fs_base::unlink(const char *path, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wunlink(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::umask(int32_t mask, int32_t &retVal , AsyncEvent *ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::mkdir(const char *path, int32_t mode, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wmkdir(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rmdir(const char *path, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wrmdir(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::chmod(const char *path, int32_t mode, AsyncEvent *ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::rename(const char *from, const char *to, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wrename(UTF8_W(from), UTF8_W(to)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readdir(const char *path, obj_ptr<List_base> &retVal, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    WIN32_FIND_DATAW fd;
    HANDLE hFind;
    exlib::wstring fpath;
    obj_ptr<List> oa;

    fpath = utf8to16String(path);
    fpath.append(L"\\*", 2);

    hFind = FindFirstFileW(fpath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

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

