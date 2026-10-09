/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ifs/zip.h"
#include "file_path.h"
#include "path.h"
#include "Stat.h"
#include "FileStream.h"
#include "MemoryStream.h"
#include "ZipFile.h"
#include "AsyncUV.h"
#include <list>

namespace fibjs {

class cache_node : public obj_base {
public:
    void init(exlib::string name, std::vector<obj_ptr<ZipFile_base::ReadAllType>>& list, date_t date = INFINITY);
    static cache_node* lookup(exlib::string name);
    static void erase(exlib::string name);

public:
    exlib::string m_name;
    date_t m_date;
    date_t m_mtime;
    std::unordered_map<exlib::string, obj_ptr<ZipFile_base::ReadAllType>> m_map;
};

static std::unordered_map<exlib::string, obj_ptr<cache_node>> s_cache_map;
static exlib::spinlock s_cachelock;

void cache_node::erase(exlib::string name)
{
    if (name.empty()) {
        s_cachelock.lock();
        s_cache_map.clear();
        s_cachelock.unlock();
    } else {
        std::list<obj_ptr<cache_node>>::iterator it;

        exlib::string safe_name;
        path_base::normalize(name, safe_name);

        s_cachelock.lock();
        s_cache_map.erase(safe_name);
        s_cachelock.unlock();
    }
}

cache_node* cache_node::lookup(exlib::string name)
{
    std::unordered_map<exlib::string, obj_ptr<cache_node>>::iterator it;
    cache_node* _node = NULL;

    s_cachelock.lock();
    it = s_cache_map.find(name);
    if (it != s_cache_map.end())
        _node = it->second;

    s_cachelock.unlock();

    return _node;
}

void cache_node::init(exlib::string name, std::vector<obj_ptr<ZipFile_base::ReadAllType>>& list, date_t date)
{
    m_name = name;
    m_date = date;
    m_mtime.now();

    for (auto& zi : list) {
        if (zi->date.empty())
            zi->date = date;

        m_map.insert_or_assign(zi->filename, zi);
    }

    s_cachelock.lock();
    s_cache_map.insert_or_assign(name, this);
    s_cachelock.unlock();
}

static result_t set_zip_fs(exlib::string fname, Buffer_base* data);

result_t fs_base::setZipFS(exlib::string fname, Union_setZipFS_data data)
{
    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        return set_zip_fs(fname, std::get<obj_ptr<Buffer_base>>(data).get());

    // a string is the zip data itself, encoded as utf8
    obj_ptr<Buffer_base> buf;
    result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", buf);
    if (hr < 0)
        return hr;

    return set_zip_fs(fname, buf.get());
}

static result_t set_zip_fs(exlib::string fname, Buffer_base* data)
{
    result_t hr;
    obj_ptr<ZipFile_base> zfile;
    obj_ptr<cache_node> _node;

    hr = zip_base::cc_open(data, "r", "utf-8", zfile);
    if (hr < 0)
        return hr;

    std::vector<obj_ptr<ZipFile_base::ReadAllType>> list;
    hr = zfile->cc_readAll("", list);
    if (hr < 0)
        return hr;

    exlib::string safe_name;
    path_base::normalize(fname, safe_name);

    _node = new cache_node();
    _node->init(safe_name, list);

    return 0;
}

result_t fs_base::clearZipFS(exlib::string fname)
{
    cache_node::erase(fname);
    return 0;
}

// The mount lookup is a chain of synchronous-in-fiber steps (open -> stat ->
// readAll -> zip open -> zip readAll). Under the handle protocol a chain that
// reuses the continuation is expressed as a state machine whose steps borrow
// the machine through next(state) tickets - the only shape that keeps exactly
// one handle per event (plans/async-handle-protocol-minimal-2026-10-08.md 8.1 P3).
class asyncResolveZipFile : public AsyncState {
public:
    asyncResolveZipFile(exlib::string zip_file, exlib::string member, bool bChanged, exlib::string member1,
        obj_ptr<ZipFile_base::ReadAllType>& retVal, AsyncHandle ac)
        : AsyncState(ac)
        , m_zip_file(zip_file)
        , m_member(member)
        , m_bChanged(bChanged)
        , m_member1(member1)
        , m_retVal(retVal)
    {
        init(check_cache);
    }

    ON_STATE(asyncResolveZipFile, check_cache)
    {
        m_node = cache_node::lookup(m_zip_file);
        m_now.now();

        if (m_node && (m_now.diff(m_node->m_date) > 3000)) {
            m_after_open = refresh_done;
            return next(open);
        }

        return next(load);
    }

    ON_STATE(asyncResolveZipFile, open)
    {
        return fs_base::openFile(m_zip_file, "r", m_zip_stream, next(stat));
    }

    ON_STATE(asyncResolveZipFile, stat)
    {
        return m_zip_stream->stat(m_stat, next(m_after_open));
    }

    ON_STATE(asyncResolveZipFile, refresh_done)
    {
        date_t _mtime;
        m_stat->get_mtime(_mtime);

        if (_mtime.diff(m_node->m_mtime) != 0)
            m_node.Release();
        else
            m_node->m_date = m_now;

        return next(load);
    }

    ON_STATE(asyncResolveZipFile, load)
    {
        if (m_node == NULL) {
            if (m_zip_stream == NULL) {
                m_after_open = read_zip;
                return next(open);
            }

            return next(read_zip);
        }

        return next(lookup);
    }

    ON_STATE(asyncResolveZipFile, read_zip)
    {
        return m_zip_stream->readAll(m_data, next(open_zip));
    }

    ON_STATE(asyncResolveZipFile, open_zip)
    {
        return zip_base::open(m_data, "r", "utf-8", m_zfile, next(read_all));
    }

    ON_STATE(asyncResolveZipFile, read_all)
    {
        return m_zfile->readAll("", m_list, next(build_node));
    }

    ON_STATE(asyncResolveZipFile, build_node)
    {
        m_node = new cache_node();
        m_node->init(m_zip_file, m_list, m_now);
        m_stat->get_mtime(m_node->m_mtime);

        return next(lookup);
    }

    ON_STATE(asyncResolveZipFile, lookup)
    {
        std::unordered_map<exlib::string, obj_ptr<ZipFile_base::ReadAllType>>::iterator it;

        it = m_node->m_map.find(m_member);
#ifdef _WIN32
        if (m_bChanged && it == m_node->m_map.end())
            it = m_node->m_map.find(m_member1);
#endif

        if (it == m_node->m_map.end())
            return CALL_E_FILE_NOT_FOUND;

        m_retVal = it->second;
        return next();
    }

private:
    exlib::string m_zip_file;
    exlib::string m_member;
    bool m_bChanged;
    exlib::string m_member1;
    obj_ptr<ZipFile_base::ReadAllType>& m_retVal;
    obj_ptr<cache_node> m_node;
    obj_ptr<SeekableStream_base> m_zip_stream;
    obj_ptr<Stat_base> m_stat;
    obj_ptr<Buffer_base> m_data;
    obj_ptr<ZipFile_base> m_zfile;
    std::vector<obj_ptr<ZipFile_base::ReadAllType>> m_list;
    date_t m_now;
    int32_t (*m_after_open)(AsyncState*, int32_t) = nullptr;
};

static result_t resolve_zip_file(exlib::string fname, obj_ptr<ZipFile_base::ReadAllType>& retVal, AsyncHandle ac)
{
    size_t pos = fname.find('$');
    if (pos != exlib::string::npos && fname[pos + 1] == PATH_SLASH) {
        exlib::string zip_file = fname.substr(0, pos);
        exlib::string member = fname.substr(pos + 2);

#ifdef _WIN32
        bool bChanged = false;
        exlib::string member1 = member;
        {
            int32_t sz = (int32_t)member1.length();
            const char* buf = member1.c_str();
            char* _member = NULL;
            for (int32_t i = 0; i < sz; i++)
                if (buf[i] == PATH_SLASH) {
                    if (!_member)
                        _member = member.data();
                    _member[i] = '/';
                    bChanged = true;
                }
        }

        return (new asyncResolveZipFile(zip_file, member, bChanged, member1, retVal, std::move(ac)))->post(0);
#else
        return (new asyncResolveZipFile(zip_file, member, false, exlib::string(), retVal, std::move(ac)))->post(0);
#endif
    }

    return CALL_E_FILE_NOT_FOUND;
}

static result_t zip_stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    obj_ptr<ZipFile_base::ReadAllType> zi;
    result_t hr = resolve_zip_file(path, zi, std::move(ac));
    if (hr >= 0) {
        obj_ptr<Stat> pStat = new Stat();
        pStat->init();

        path_base::basename(path, "", pStat->name);

        pStat->m_mode = S_IRUSR;
        pStat->size = zi->file_size;
        pStat->mtime = pStat->atime = pStat->ctime = pStat->birthtime = zi->date;
        pStat->m_isMemory = true;

        retVal = pStat;

        return 0;
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t fs_base::lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(path, safe_name);
    if (hr < 0)
        return hr;

    // the isolate is read before the continuation is handed over (P4)
    Isolate* isolate = ac.isolate();

    hr = zip_stat(safe_name, retVal, std::move(ac));
    if (hr >= 0)
        return 0;

    if (!isolate->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    AutoReq req;
    int32_t ret = uv_fs_lstat(NULL, &req, safe_name.c_str(), NULL);
    if (ret < 0)
        return setSystemErrorPayload(ret, "lstat", path);

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(safe_name, &req.statbuf);
    retVal = pStat;

    return 0;
}

result_t fs_base::lstat(exlib::string path, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        bool throwIfNoEntry = true;
        GetConfigValue(options, "throwIfNoEntry", throwIfNoEntry);

        // Node.js compatibility: throwIfNoEntry only affects the synchronous
        // (non-callback) forms; the async forms always report the error.
        ac.ctxv()[0] = throwIfNoEntry || ac.callType() == AsyncEvent::kAsyncCallBack;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    // the flag is read before the continuation is handed over (P4)
    bool throwIfNoEntry = ac.ctxv()[0].boolVal();

    result_t hr = lstat(path, retVal, std::move(ac));
    if (hr < 0 && !throwIfNoEntry && (hr == UV_ENOENT || hr == UV_ENOTDIR))
        return CALL_RETURN_UNDEFINED;

    return hr;
}

result_t fs_base::stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(path, safe_name);
    if (hr < 0)
        return hr;

    // the isolate is read before the continuation is handed over (P4)
    Isolate* isolate = ac.isolate();

    hr = zip_stat(safe_name, retVal, std::move(ac));
    if (hr >= 0)
        return 0;

    if (!isolate->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    AutoReq req;
    int32_t ret = uv_fs_stat(NULL, &req, safe_name.c_str(), NULL);
    if (ret < 0)
        return setSystemErrorPayload(ret, "stat", path);

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(safe_name, &req.statbuf);
    retVal = pStat;

    return 0;
}

result_t fs_base::stat(exlib::string path, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        bool throwIfNoEntry = true;
        GetConfigValue(options, "throwIfNoEntry", throwIfNoEntry);

        // Node.js compatibility: throwIfNoEntry only affects the synchronous
        // (non-callback) forms; the async forms always report the error.
        ac.ctxv()[0] = throwIfNoEntry || ac.callType() == AsyncEvent::kAsyncCallBack;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    // the flag is read before the continuation is handed over (P4)
    bool throwIfNoEntry = ac.ctxv()[0].boolVal();

    result_t hr = stat(path, retVal, std::move(ac));
    if (hr < 0 && !throwIfNoEntry && (hr == UV_ENOENT || hr == UV_ENOTDIR))
        return CALL_RETURN_UNDEFINED;

    return hr;
}

// the flags forms: a string mode name, or the integer fs.constants flags
static result_t open_file_string(exlib::string fname, exlib::string flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac);
static result_t open_file_int(exlib::string fname, int32_t flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac);

result_t fs_base::openFile(exlib::string fname, Union_openFile_flags flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<int32_t>(flags))
        return open_file_int(fname, std::get<int32_t>(flags), retVal, std::move(ac));

    return open_file_string(fname, std::get<exlib::string>(flags), retVal, std::move(ac));
}

static result_t open_file_string(exlib::string fname, exlib::string flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    // the isolate is read before the continuation is handed over (P4)
    Isolate* isolate = ac.isolate();

    obj_ptr<ZipFile_base::ReadAllType> zi;
    hr = resolve_zip_file(safe_name, zi, std::move(ac));
    if (hr >= 0) {
        obj_ptr<Buffer_base> data;
        exlib::string strData;
        date_t _d;

        data = zi->data;
        if (data)
            data->toString(strData);

        _d = zi->date;
        retVal = new MemoryStream::CloneStream(strData, _d);
        return 0;
    }

    if (!isolate->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<FileStream> pFile = new FileStream();
    hr = pFile->open(safe_name, flags);
    if (hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}

static result_t open_file_int(exlib::string fname, int32_t flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!ac.isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr < 0)
        return hr;

    int32_t _fd;
    hr = file_open(safe_name, flags, 0666, _fd);
    if (hr < 0)
        return hr;

    retVal = new FileStream(_fd);

    return 0;
}
}
