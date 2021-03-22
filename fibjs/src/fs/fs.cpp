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
#include "Buffer.h"
#include "Stat.h"
#include "File.h"
#include "AsyncUV.h"

namespace fibjs {

DECLARE_MODULE(fs);

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

result_t fs_base::copyFile(exlib::string from, exlib::string to, int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_copyfile(s_uv_loop, new AsyncUVFS(ac), from.c_str(), to.c_str(), mode,
            AsyncUVFS::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}

result_t fs_base::readdir(exlib::string path, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class AsyncUVFSReadDir : public uv_fs_t {
    public:
        AsyncUVFSReadDir(obj_ptr<NArray>& retVal, AsyncEvent* ac)
            : m_retVal(retVal)
            , m_ac(ac)
        {
        }

        ~AsyncUVFSReadDir()
        {
            uv_fs_req_cleanup(this);
        }

    public:
        static void callback(uv_fs_t* req)
        {
            AsyncUVFSReadDir* pThis = (AsyncUVFSReadDir*)req;
            int ret;

            ret = uv_fs_get_result(req);
            if (ret < 0) {
                pThis->m_ac->apost(Runtime::setError(uv_strerror(ret)));
                delete pThis;
                return;
            }

            obj_ptr<NArray> oa = new NArray();
            while (uv_fs_scandir_next(req, &pThis->m_dirent) != UV_EOF)
                oa->append(pThis->m_dirent.name);

            pThis->m_retVal = oa;

            pThis->m_ac->apost(0);
            delete pThis;
        }

    private:
        obj_ptr<NArray>& m_retVal;
        AsyncEvent* m_ac;
        uv_dirent_t m_dirent;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int ret = uv_call([&] {
        return uv_fs_scandir(s_uv_loop, new AsyncUVFSReadDir(retVal, ac), path.c_str(), 0,
            AsyncUVFSReadDir::callback);
    });
    if (ret != 0)
        return CHECK_ERROR(Runtime::setError(uv_strerror(ret)));

    return CALL_E_PENDDING;
}
}
