/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ifs/zip.h"
#include "ifs/iconv.h"
#include "path.h"
#include "Stat.h"
#include "File.h"
#include "BufferedStream.h"
#include "MemoryStream.h"
#include "ZipFile.h"
#include "FSWatcher.h"
#include "StatsWatcher.h"
#include "AsyncUV.h"

#ifndef _WIN32
#include <dirent.h>
#else
#include <stdio.h>
#endif

namespace fibjs {

DECLARE_MODULE(fs);

class cache_node : public obj_base {
public:
    exlib::string m_name;
    date_t m_date;
    date_t m_mtime;
    obj_ptr<NArray> m_list;
};

static std::list<obj_ptr<cache_node>> s_cache;
static exlib::spinlock s_cachelock;

result_t fs_base::setZipFS(exlib::string fname, Buffer_base* data)
{
    result_t hr;
    std::list<obj_ptr<cache_node>>::iterator it;
    obj_ptr<ZipFile_base> zfile;
    obj_ptr<cache_node> _node;

    hr = zip_base::cc_open(data, "r", zip_base::_ZIP_DEFLATED, zfile);
    if (hr < 0)
        return hr;

    obj_ptr<NArray> list;
    hr = zfile->cc_readAll("", list);
    if (hr < 0)
        return hr;

    exlib::string safe_name;
    path_base::normalize(fname, safe_name);

    _node = new cache_node();
    _node->m_name = safe_name;
    _node->m_list = list;
    _node->m_date = INFINITY;
    _node->m_mtime.now();

    s_cachelock.lock();
    for (it = s_cache.begin(); it != s_cache.end(); ++it)
        if ((*it)->m_name == safe_name) {
            *it = _node;
            break;
        }
    if (it == s_cache.end())
        s_cache.push_back(_node);
    s_cachelock.unlock();
    return 0;
}

result_t fs_base::clearZipFS(exlib::string fname)
{
    if (fname.empty()) {
        s_cachelock.lock();
        s_cache.clear();
        s_cachelock.unlock();
    } else {
        std::list<obj_ptr<cache_node>>::iterator it;

        exlib::string safe_name;
        path_base::normalize(fname, safe_name);

        s_cachelock.lock();
        for (it = s_cache.begin(); it != s_cache.end(); ++it)
            if ((*it)->m_name == safe_name) {
                s_cache.erase(it);
                break;
            }
        s_cachelock.unlock();
    }

    return 0;
}

result_t fs_base::openFile(exlib::string fname, exlib::string flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
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
            int32_t sz = (int32_t)member1.length();
            const char* buf = member1.c_str();
            for (int32_t i = 0; i < sz; i++)
                if (buf[i] == PATH_SLASH) {
                    member[i] = '/';
                    bChanged = true;
                }
        }
#endif

        obj_ptr<cache_node> _node;
        obj_ptr<SeekableStream_base> zip_stream;
        obj_ptr<Stat_base> stat;
        std::list<obj_ptr<cache_node>>::iterator it;

        date_t _now;
        _now.now();

        s_cachelock.lock();
        for (it = s_cache.begin(); it != s_cache.end(); ++it)
            if ((*it)->m_name == zip_file) {
                _node = *it;
                break;
            }
        s_cachelock.unlock();

        if (_node && (_now.diff(_node->m_date) > 3000)) {
            hr = openFile(zip_file, "r", zip_stream, ac);
            if (hr < 0)
                return hr;

            hr = zip_stream->cc_stat(stat);
            if (hr < 0)
                return hr;

            date_t _mtime;
            stat->get_mtime(_mtime);

            if (_mtime.diff(_node->m_mtime) != 0)
                _node.Release();
            else
                _node->m_date = _now;
        }

        if (_node == NULL) {
            if (zip_stream == NULL) {
                hr = openFile(zip_file, "r", zip_stream, ac);
                if (hr < 0)
                    return hr;

                hr = zip_stream->cc_stat(stat);
                if (hr < 0)
                    return hr;
            }

            obj_ptr<Buffer_base> zip_data;
            hr = zip_stream->cc_readAll(zip_data);
            if (hr < 0)
                return hr;

            hr = zip_base::cc_open(zip_data, "r", zip_base::_ZIP_DEFLATED, zfile);
            if (hr < 0)
                return hr;

            obj_ptr<NArray> list;
            hr = zfile->cc_readAll("", list);
            if (hr < 0)
                return hr;

            _node = new cache_node();
            _node->m_name = zip_file;
            _node->m_list = list;
            _node->m_date.now();
            stat->get_mtime(_node->m_mtime);

            s_cachelock.lock();
            for (it = s_cache.begin(); it != s_cache.end(); ++it)
                if ((*it)->m_name == zip_file) {
                    *it = _node;
                    break;
                }
            if (it == s_cache.end())
                s_cache.push_back(_node);
            s_cachelock.unlock();
        }

        int32_t len, i;
        bool bFound = false;
        obj_ptr<ZipFile::Info> zi;

        _node->m_list->get_length(len);

        for (i = 0; i < len; i++) {
            Variant v;
            exlib::string s;

            _node->m_list->_indexed_getter(i, v);
            zi = (ZipFile::Info*)v.object();

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
        if (!ac->isolate()->m_enable_FileSystem)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

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

    retVal->Set(isolate->NewString("F_OK"), v8::Integer::New(isolate->m_isolate, F_OK));
    retVal->Set(isolate->NewString("R_OK"), v8::Integer::New(isolate->m_isolate, R_OK));
    retVal->Set(isolate->NewString("W_OK"), v8::Integer::New(isolate->m_isolate, W_OK));
    retVal->Set(isolate->NewString("X_OK"), v8::Integer::New(isolate->m_isolate, X_OK));

    return 0;
}

result_t fs_base::open(exlib::string fname, exlib::string flags, int32_t mode,
    int32_t& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    path_base::normalize(fname, safe_name);

    return file_open(safe_name, flags, mode, retVal);
}

result_t fs_base::close(int32_t fd, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (fd != -1) {
        ::_close(fd);

        fd = -1;
    }

    return 0;
}

result_t fs_base::openTextStream(exlib::string fname, exlib::string flags,
    obj_ptr<BufferedStream_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> pFile;
    result_t hr = openFile(fname, flags, pFile, ac);
    if (hr < 0)
        return hr;

    return BufferedStream_base::_new(pFile, retVal);
}

result_t fs_base::readTextFile(exlib::string fname, exlib::string& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = openFile(fname, "r", f, ac);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->cc_close();

    if (hr == CALL_RETURN_NULL) {
        retVal.clear();
        return 0;
    }

    if (hr < 0)
        return hr;

    return buf->toString(retVal);
}

result_t fs_base::readFile(exlib::string fname, exlib::string encoding,
    Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    obj_ptr<Buffer_base> buf;
    result_t hr;

    hr = openFile(fname, "r", f, ac);
    if (hr < 0)
        return hr;

    hr = f->cc_readAll(buf);
    f->cc_close();

    if (hr < 0)
        return hr;

    if (hr == CALL_RETURN_NULL) {
        if (encoding != "") {
            exlib::string str;
            retVal = str;
        } else {
            buf = new Buffer();
            retVal = buf;
        }
    } else {
        if (encoding != "") {
            exlib::string str;
            hr = iconv_base::decode(encoding, buf, str);
            if (hr < 0)
                return hr;
            retVal = str;
        } else
            retVal = buf;
    }

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
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = openFile(fname, "w", f, ac);
    if (hr < 0)
        return hr;

    obj_ptr<Buffer_base> buf = new Buffer(txt);

    hr = f->cc_write(buf);
    f->cc_close();

    return hr;
}

result_t fs_base::writeFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = openFile(fname, "w", f, ac);
    if (hr < 0)
        return hr;

    hr = f->cc_write(data);
    f->cc_close();

    return hr;
}

result_t fs_base::appendFile(exlib::string fname, Buffer_base* data, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);
    obj_ptr<SeekableStream_base> f;
    result_t hr;

    hr = openFile(fname, "a", f, ac);
    if (hr < 0)
        return hr;

    hr = f->cc_write(data);
    f->cc_close();

    return hr;
}

result_t get_fs_stat(exlib::string fname, obj_ptr<Stat_base>& retVal, bool use_lstat = false)
{
    obj_ptr<Stat> pStat = new Stat();

    result_t hr = !use_lstat ? pStat->getStat(fname) : pStat->getLstat(fname);
    if (hr < 0)
        return hr;

    retVal = pStat;
    return 0;
}

result_t fs_base::stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return get_fs_stat(path, retVal);
}

result_t fs_base::lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return get_fs_stat(path, retVal, true);
}

result_t fs_base::read(int32_t fd, Buffer_base* buffer, int32_t offset, int32_t length,
    int32_t position, int32_t& retVal, AsyncEvent* ac)
{
    if (fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t bufLength;
    buffer->get_length(bufLength);

    if (offset < 0 || offset >= bufLength)
        return Runtime::setError("fs: Offset is out of bounds");

    exlib::string strBuf;

    if (length < 0 || (offset + length > bufLength)) {
        return Runtime::setError("fs: Length extends beyond buffer");
    }

    if (position > -1) {
        if (_lseeki64(fd, position, SEEK_SET) < 0)
            return CHECK_ERROR(LastError());
    }

    if (length > 0) {
        strBuf.resize(length);
        int32_t sz = length;
        char* p = &strBuf[0];

        while (sz) {
            int32_t n = (int32_t)::_read(fd, p, sz > STREAM_BUFF_SIZE ? STREAM_BUFF_SIZE : sz);
            if (n < 0)
                return CHECK_ERROR(LastError());
            if (n == 0)
                break;

            sz -= n;
            p += n;
        }

        strBuf.resize(length - sz);
    }

    if (strBuf.length() == 0) {
        retVal = 0;
        return 0;
    }

    return buffer->write(strBuf, offset, (int32_t)strBuf.length(), "utf8", retVal);
}

result_t fs_base::exists(exlib::string path, bool& retVal, AsyncEvent* ac)
{
    class AsyncUVFSStatue : public uv_fs_t {
    public:
        AsyncUVFSStatue(bool& retVal, AsyncEvent* ac)
            : m_retVal(retVal)
            , m_ac(ac)
        {
        }

    public:
        static void callback(uv_fs_t* req)
        {
            AsyncUVFSStatue* pThis = (AsyncUVFSStatue*)req;

            pThis->m_retVal = uv_fs_get_result(req) == 0;
            pThis->m_ac->apost(0);

            uv_fs_req_cleanup(req);
            delete pThis;
        }

    private:
        bool& m_retVal;
        AsyncEvent* m_ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_access(s_uv_loop, new AsyncUVFSStatue(retVal, ac), path.c_str(), F_OK, AsyncUVFSStatue::callback);
    });
    if (ret != 0) {
        retVal = false;
        return 0;
    }

    return CALL_E_PENDDING;
}

result_t fs_base::access(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_access(s_uv_loop, new AsyncUVFS(ac), path.c_str(), mode, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::link(exlib::string oldPath, exlib::string newPath, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_link(s_uv_loop, new AsyncUVFS(ac), oldPath.c_str(), newPath.c_str(), AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::unlink(exlib::string path, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_unlink(s_uv_loop, new AsyncUVFS(ac), path.c_str(), AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::symlink(exlib::string target, exlib::string linkpath, exlib::string type, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        int _type = 0;

        if (type == "dir")
            _type = 1;
        else if (type == "junction")
            _type = 2;
        return uv_fs_symlink(s_uv_loop, new AsyncUVFS(ac), target.c_str(), linkpath.c_str(), _type, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::readlink(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_readlink(s_uv_loop, new AsyncUVFSResult(retVal, ac), path.c_str(), AsyncUVFSResult::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::realpath(exlib::string path, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_realpath(s_uv_loop, new AsyncUVFSResult(retVal, ac), path.c_str(), AsyncUVFSResult::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::mkdir(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_mkdir(s_uv_loop, new AsyncUVFS(ac), path.c_str(), mode, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::rmdir(exlib::string path, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_rmdir(s_uv_loop, new AsyncUVFS(ac), path.c_str(), AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::fchmod(int32_t fd, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_fchmod(s_uv_loop, new AsyncUVFS(ac), fd, mode, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::fchown(int32_t fd, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_fchown(s_uv_loop, new AsyncUVFS(ac), fd, uid, gid, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::fsync(int32_t fd, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_fsync(s_uv_loop, new AsyncUVFS(ac), fd, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::chmod(exlib::string path, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_chmod(s_uv_loop, new AsyncUVFS(ac), path.c_str(), mode, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::chown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_chown(s_uv_loop, new AsyncUVFS(ac), path.c_str(), uid, gid, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::lchown(exlib::string path, int32_t uid, int32_t gid, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_lchown(s_uv_loop, new AsyncUVFS(ac), path.c_str(), uid, gid, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::rename(exlib::string from, exlib::string to, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_rename(s_uv_loop, new AsyncUVFS(ac), from.c_str(), to.c_str(), AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::fdatasync(int32_t fd, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_fdatasync(s_uv_loop, new AsyncUVFS(ac), fd, AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::watch(exlib::string fname, obj_ptr<FSWatcher_base>& retVal)
{
    return watch(fname, v8::Local<v8::Function>(), retVal);
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    return watch(fname, opts, callback, retVal);
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Object> options, obj_ptr<FSWatcher_base>& retVal)
{
    return watch(fname, options, v8::Local<v8::Function>(), retVal);
}

result_t get_safe_abs_path(exlib::string fname, exlib::string& safe_name)
{
    result_t hr = 0;
    path_base::normalize(fname, safe_name);
    bool is_abs;
    path_base::isAbsolute(safe_name, is_abs);

    if (!is_abs)
        hr = _resolve(safe_name);

    return hr;
}

result_t fs_base::watch(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<FSWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    Isolate* isolate = Isolate::current();
    bool persistent = true;
    hr = GetConfigValue(isolate->m_isolate, options, "persistent", persistent, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    bool recursive = false;
    hr = GetConfigValue(isolate->m_isolate, options, "recursive", recursive, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    obj_ptr<FSWatcher> pFW = new FSWatcher(safe_name, callback, persistent, recursive);
    retVal = pFW;

    pFW->start();

    return 0;
}

result_t fs_base::watchFile(exlib::string fname, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Object> opts = v8::Object::New(isolate->m_isolate);

    return watchFile(fname, opts, callback, retVal);
}

result_t fs_base::watchFile(exlib::string fname, v8::Local<v8::Object> options, v8::Local<v8::Function> callback, obj_ptr<StatsWatcher_base>& retVal)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    obj_ptr<StatsWatcher> pSW = StatsWatcher::getTargetWatcher(safe_name);
    if (pSW == NULL) {
        Isolate* isolate = Isolate::current();
        bool persistent = true;
        hr = GetConfigValue(isolate->m_isolate, options, "persistent", persistent, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        bool useBigInt = false;
        hr = GetConfigValue(isolate->m_isolate, options, "bigint", useBigInt, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        int32_t interval = DEFAULT_STATS_WATCH_INTERVAL;
        hr = GetConfigValue(isolate->m_isolate, options, "interval", interval, true);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        pSW = new StatsWatcher(safe_name, persistent, interval, useBigInt);
        retVal = pSW;

        hr = pSW->start();

        if (hr < 0)
            return hr;
    }

    pSW->bindChangeHandler(callback);

    retVal = pSW;

    return hr;
}

result_t fs_base::unwatchFile(exlib::string fname)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    obj_ptr<StatsWatcher> pSW = StatsWatcher::getTargetWatcher(safe_name);
    if (pSW == NULL)
        return 0;

    pSW->removeChangeHandler();

    return 0;
}

result_t fs_base::unwatchFile(exlib::string fname, v8::Local<v8::Function> callback)
{
    result_t hr;
    exlib::string safe_name;
    if ((hr = get_safe_abs_path(fname, safe_name)) < 0)
        return 0;

    obj_ptr<StatsWatcher> pSW = StatsWatcher::getTargetWatcher(safe_name);
    if (pSW == NULL)
        return 0;

    pSW->removeChangeHandler(callback);

    return 0;
}

}
