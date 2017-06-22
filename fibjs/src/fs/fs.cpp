/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/fs.h"
#include "ifs/zip.h"
#include "ifs/iconv.h"
#include "path.h"
#include "utf8.h"
#include "Stat.h"
#include "List.h"
#include "File.h"
#include "BufferedStream.h"
#include "MemoryStream.h"
#include "Map.h"
#include "list"

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
#define F_OK 0
#define W_OK 4
#define R_OK 2
#define X_OK 1
#endif

#define MAX_PATH_LENGTH 4096

namespace fibjs {

DECLARE_MODULE(fs);

class cache_node : public obj_base {
public:
    exlib::string m_name;
    date_t m_date;
    obj_ptr<List_base> m_list;
};

static std::list<obj_ptr<cache_node>> s_cache;
static exlib::spinlock s_cachelock;

void init_fs()
{
#ifndef _WIN32
    ::umask(0);
#endif
}

result_t fs_base::openFile(exlib::string fname, exlib::string flags,
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

        obj_ptr<cache_node> _node;
        std::list<obj_ptr<cache_node>>::iterator it;

        date_t _now;
        _now.now();

        s_cachelock.lock();
        while ((it = s_cache.begin()) != s_cache.end())
            if (_now.diff((*it)->m_date) > 3000)
                s_cache.erase(it);
            else
                break;

        for (it = s_cache.begin(); it != s_cache.end(); ++it)
            if ((*it)->m_name == zip_file) {
                _node = *it;
                break;
            }
        s_cachelock.unlock();

        if (_node == NULL) {
            hr = zip_base::cc_open(zip_file, "r", zip_base::_ZIP_DEFLATED, zfile);
            if (hr < 0)
                return hr;

            obj_ptr<List_base> list;
            hr = zfile->cc_readAll("", list);
            if (hr < 0)
                return hr;

            _node = new cache_node();
            _node->m_name = zip_file;
            _node->m_list = list;
            _node->m_date.now();

            s_cachelock.lock();
            s_cache.push_back(_node);
            s_cachelock.unlock();
        }

        int32_t len, i;
        bool bFound = false;
        obj_ptr<ZipInfo_base> zi;

        _node->m_list->get_length(len);

        for (i = 0; i < len; i++) {
            Variant v;
            exlib::string s;

            _node->m_list->_indexed_getter(i, v);
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

        date_t _d;

        zi->get_data(data);
        if (data)
            data->toString(strData);

        zi->get_date(_d);
        if (_d.empty())
            _d = _node->m_date;

        retVal = new MemoryStream::CloneStream(strData, _d);
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

result_t fs_base::get_constants(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = v8::Object::New(isolate->m_isolate);

    retVal->Set(isolate->NewFromUtf8("F_OK"), v8::Integer::New(isolate->m_isolate, F_OK));
    retVal->Set(isolate->NewFromUtf8("R_OK"), v8::Integer::New(isolate->m_isolate, R_OK));
    retVal->Set(isolate->NewFromUtf8("W_OK"), v8::Integer::New(isolate->m_isolate, W_OK));
    retVal->Set(isolate->NewFromUtf8("X_OK"), v8::Integer::New(isolate->m_isolate, X_OK));

    return 0;
}

result_t fs_base::openTextStream(exlib::string fname, exlib::string flags,
    obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> pFile;
    result_t hr = cc_openFile(fname, flags, pFile);
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

    hr = cc_openFile(fname, "r", f);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->cc_close();

    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return buf->toString(retVal);
}

result_t fs_base::readFile(exlib::string fname, exlib::string encoding,
    Variant& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = cc_openFile(fname, "r", f);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->cc_close();

    if (encoding != "") {
        exlib::string str;
        hr = iconv_base::decode(encoding, buf, str);
        if (hr < 0)
            return hr;
        retVal = str;
    } else
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

    hr = cc_openFile(fname, "w", f);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(txt);

    hr = f->cc_write(buf);
    f->cc_close();

    return hr;
}

result_t fs_base::writeFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = cc_openFile(fname, "w", f);
    if (hr < 0)
        return hr;

    hr = f->cc_write(data);
    f->cc_close();

    return hr;
}

result_t fs_base::appendFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);
    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = cc_openFile(fname, "a", f);
    if (hr < 0)
        return hr;

    hr = f->cc_write(data);
    f->cc_close();

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

result_t fs_base::lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stat> pStat = new Stat();

    result_t hr = pStat->getLstat(path);
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

result_t fs_base::access(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    DWORD attr = GetFileAttributesW(UTF8_W(path));

    if (attr == INVALID_FILE_ATTRIBUTES)
        return CHECK_ERROR(LastError());

    if (mode & W_OK && attr & FILE_ATTRIBUTE_READONLY && !(attr & FILE_ATTRIBUTE_DIRECTORY))
        return CHECK_ERROR(CALL_E_PERMIT);

    return 0;
}

result_t fs_base::link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (::CreateHardLinkW(UTF8_W(newPath), UTF8_W(oldPath), NULL) == 0)
        return CHECK_ERROR(LastError());

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

result_t fs_base::readlink(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
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

result_t fs_base::lchmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::realpath(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    bool isDir;
    obj_ptr<Stat> pStat = new Stat();

    result_t hr = pStat->getStat(target);
    if (hr < 0)
        return hr;

    pStat->isDirectory(isDir);
    if (CreateSymbolicLinkW(UTF8_W(linkpath), UTF8_W(target), isDir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) == 0)
        return CHECK_ERROR(LastError());

    return 0;
}

result_t fs_base::truncate(exlib::string path, int32_t len, AsyncEvent* ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    HANDLE file;

    if ((file = CreateFileW(UTF8_W(path),
             GENERIC_WRITE,
             FILE_SHARE_WRITE,
             NULL,
             CREATE_NEW | OPEN_EXISTING,
             FILE_ATTRIBUTE_NORMAL,
             NULL))
        == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    if (SetFilePointer(file, (LONG)len, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return CHECK_ERROR(LastError());

    if (!SetEndOfFile(file))
        return CHECK_ERROR(LastError());

    CloseHandle(file);
    return 0;
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

result_t fs_base::accessSync(exlib::string path, int32_t mode)
{
    return ac_access(path, mode);
}

result_t fs_base::linkSync(exlib::string oldPath, exlib::string newPath)
{
    return ac_link(oldPath, newPath);
}

result_t fs_base::unlinkSync(exlib::string path)
{
    return ac_unlink(path);
}

result_t fs_base::readlinkSync(exlib::string path, exlib::string& retVal)
{
    return ac_readlink(path, retVal);
}

result_t fs_base::realpathSync(exlib::string path, exlib::string& retVal)
{
    return ac_realpath(path, retVal);
}

result_t fs_base::symlinkSync(exlib::string target, exlib::string linkpath)
{
    return ac_symlink(target, linkpath);
}

result_t fs_base::truncateSync(exlib::string path, int32_t len)
{
    return ac_truncate(path, len);
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

result_t fs_base::lchmodSync(exlib::string path, int32_t mode)
{
    return ac_lchmod(path, mode);
}

result_t fs_base::chownSync(exlib::string path, int32_t uid, int32_t gid)
{
    return ac_chown(path, uid, gid);
}

result_t fs_base::lchownSync(exlib::string path, int32_t uid, int32_t gid)
{
    return ac_lchown(path, uid, gid);
}

result_t fs_base::statSync(exlib::string path, obj_ptr<Stat_base>& retVal)
{
    return ac_stat(path, retVal);
}

result_t fs_base::lstatSync(exlib::string path, obj_ptr<Stat_base>& retVal)
{
    return ac_lstat(path, retVal);
}

result_t fs_base::readdirSync(exlib::string path, obj_ptr<List_base>& retVal)
{
    return ac_readdir(path, retVal);
}

result_t fs_base::readFileSync(exlib::string fname, exlib::string encoding, Variant& retVal)
{
    return ac_readFile(fname, encoding, retVal);
}

result_t fs_base::writeFileSync(exlib::string fname, Buffer_base* data)
{
    return ac_writeFile(fname, data);
}

result_t fs_base::appendFileSync(exlib::string fname, Buffer_base* data)
{
    return ac_appendFile(fname, data);
}
}
