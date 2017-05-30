/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/fs.h"
#include "ifs/zip.h"
#include "path.h"
#include "utf8.h"
#include "Stat.h"
#include "List.h"
#include "File.h"
#include "BufferedStream.h"
#include "MemoryStream.h"
#include "Map.h"

#ifndef _WIN32
#include <dirent.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#if defined(Darwin) || defined(FreeBSD)
#include <copyfile.h>
#else
#include <sys/sendfile.h>
#endif

#else
#include <stdio.h>
#endif

namespace fibjs {

DECLARE_MODULE(fs);

static exlib::spinlock s_cachelock;
static std::map<exlib::string, obj_ptr<List_base>> s_cache;
static date_t s_date;

void init_fs()
{
#ifndef _WIN32
    ::umask(0);
#endif

    s_date.now();
}

result_t fs_base::open(exlib::string fname, exlib::string flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    path_base::normalize(fname, safe_name);

    size_t pos = safe_name.find('$');
    if (pos != exlib::string::npos && safe_name[pos + 1] == PATH_SLASH) {
        exlib::string zip_file = safe_name.substr(0, pos);
        exlib::string member = safe_name.substr(pos + 2);
        obj_ptr<ZipFile_base> zfile;
        obj_ptr<Buffer_base> data;
        exlib::string strData;
        result_t hr;

#ifdef _WIN32
        bool bChanged = false;
        exlib::string member1 = member;
        {
            int32_t sz = (int32_t)member.length();
            const char* buf = member.c_str();
            for (int32_t i = 0; i < sz; i++)
                if (buf[i] == PATH_SLASH) {
                    member[i] = '/';
                    bChanged = true;
                }
        }
#endif

        obj_ptr<List_base> list;
        std::map<exlib::string, obj_ptr<List_base>>::iterator it;

        s_cachelock.lock();
        it = s_cache.find(zip_file);
        if (it != s_cache.end())
            list = it->second;
        s_cachelock.unlock();

        if (list == NULL) {
            hr = zip_base::cc_open(zip_file, "r", zip_base::_ZIP_DEFLATED, zfile);
            if (hr < 0)
                return hr;

            hr = zfile->cc_readAll("", list);
            if (hr < 0)
                return hr;

            s_cachelock.lock();
            s_cache.insert(std::pair<exlib::string, obj_ptr<List_base>>(zip_file, list));
            s_cachelock.unlock();
        }

        int32_t len, i;
        bool bFound = false;
        obj_ptr<ZipInfo_base> zi;

        list->get_length(len);

        for (i = 0; i < len; i++) {
            Variant v;
            exlib::string s;

            list->_indexed_getter(i, v);
            zi = ZipInfo_base::getInstance(v.object());

            zi->get_filename(s);

            if (member == s) {
                bFound = true;
                break;
            }

#ifdef _WIN32
            if (bChanged && member1 == s) {
                member = member1;
                bFound = true;
                break;
            }
#endif
        }

        if (!bFound)
            return CALL_E_FILE_NOT_FOUND;

        zi->get_data(data);
        if (data)
            data->toString(strData);
        retVal = new MemoryStream::CloneStream(strData, s_date);
    } else {
        obj_ptr<File> pFile = new File();
        result_t hr;

        hr = pFile->open(safe_name, flags);
        if (hr < 0)
            return hr;

        retVal = pFile;
    }

    return 0;
}

result_t fs_base::openTextStream(exlib::string fname, exlib::string flags,
    obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> pFile;
    result_t hr = cc_open(fname, flags, pFile);
    if (hr < 0)
        return hr;

    return BufferedStream_base::_new(pFile, retVal);
}

result_t fs_base::readTextFile(exlib::string fname, exlib::string& retVal,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = cc_open(fname, "r", f);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->close(ac);

    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return buf->toString(retVal);
}

result_t fs_base::readFile(exlib::string fname, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = cc_open(fname, "r", f);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->close(ac);

    retVal = buf;

    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return 0;
}

result_t fs_base::readLines(exlib::string fname, int32_t maxlines,
    v8::Local<v8::Array>& retVal)
{
    obj_ptr<BufferedStream_base> pFile;
    result_t hr;

    hr = ac_openTextStream(fname, "r", pFile);
    if (hr < 0)
        return hr;

    return pFile->readLines(maxlines, retVal);
}

result_t fs_base::writeTextFile(exlib::string fname, exlib::string txt,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = cc_open(fname, "w", f);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(txt);

    hr = f->cc_write(buf);
    f->close(ac);

    return hr;
}

result_t fs_base::writeFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = cc_open(fname, "w", f);
    if (hr < 0)
        return hr;

    hr = f->cc_write(data);
    f->close(ac);

    return hr;
}

result_t fs_base::stat(exlib::string path, obj_ptr<Stat_base>& retVal,
    AsyncEvent* ac)
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

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = access(path.c_str(), F_OK) == 0;
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

#if defined(Darwin) || defined(FreeBSD)
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

#else

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = _waccess(UTF8_W(path), 0) == 0;
    return 0;
}

result_t fs_base::unlink(exlib::string path, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wunlink(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::mkdir(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wmkdir(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::rmdir(exlib::string path, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wrmdir(UTF8_W(path)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::chmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::rename(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::_wrename(UTF8_W(from), UTF8_W(to)))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::copy(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (!CopyFileW(UTF8_W(from), UTF8_W(to), TRUE))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<List_base>& retVal, AsyncEvent* ac)
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

    do {
        if (qstrcmp(fd.cFileName, L".") && qstrcmp(fd.cFileName, L"..")) {
            exlib::string name = UTF8_A(fd.cFileName);
            oa->append(name);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    retVal = oa;

    return 0;
}

#endif

result_t fs_base::existsSync(exlib::string path, bool& retVal)
{
    return ac_exists(path, retVal);
}

result_t fs_base::unlinkSync(exlib::string path)
{
    return ac_unlink(path);
}

result_t fs_base::mkdirSync(exlib::string path, int32_t mode)
{
    return ac_mkdir(path, mode);
}

result_t fs_base::rmdirSync(exlib::string path)
{
    return ac_rmdir(path);
}

result_t fs_base::renameSync(exlib::string from, exlib::string to)
{
    return ac_rename(from, to);
}

result_t fs_base::chmodSync(exlib::string path, int32_t mode)
{
    return ac_chmod(path, mode);
}

result_t fs_base::statSync(exlib::string path, obj_ptr<Stat_base>& retVal)
{
    return ac_stat(path, retVal);
}

result_t fs_base::readdirSync(exlib::string path, obj_ptr<List_base>& retVal)
{
    return ac_readdir(path, retVal);
}

result_t fs_base::openSync(exlib::string fname, exlib::string flags, obj_ptr<SeekableStream_base>& retVal)
{
    return ac_open(fname, flags, retVal);
}

result_t fs_base::readFileSync(exlib::string fname, obj_ptr<Buffer_base>& retVal)
{
    return ac_readFile(fname, retVal);
}

result_t fs_base::writeFileSync(exlib::string fname, Buffer_base* data)
{
    return ac_writeFile(fname, data);
}
}
