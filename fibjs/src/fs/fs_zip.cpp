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
#include "LruCache.h"
#include "AsyncUV.h"
#include <mutex>

namespace fibjs {

// Decompressed entries are kept in an LRU: one archive can serve a working set
// that is much smaller than all of its members, and holding every inflated
// member forever is the unbounded growth the file cache avoids too. The
// directory index itself stays complete - it is small and lets stat answer
// without reopening the archive - and never changes once the node is published.
// No time-based expiry: an entry stays valid until the archive's mtime changes,
// which replaces the whole node. A path-backed archive is never held in memory
// as a whole: a cold read of one member opens the archive, reads the directory
// and that member, and closes it again.
#define ZIP_DATA_CACHE_SIZE 1024

class cache_node : public obj_base {
public:
    enum lookup_result {
        kNotFound,
        kHit,
        kNeedData
    };

    cache_node()
        : m_data(ZIP_DATA_CACHE_SIZE, 0, 0)
    {
    }

public:
    void init(const exlib::string& name, std::unordered_map<exlib::string, ZipFile::index_entry>&& index,
        int64_t mtime_ns, double size, date_t date = INFINITY);
    lookup_result lookup(const exlib::string& member, bool with_data, obj_ptr<ZipFile_base::ReadAllType>& retVal);
    result_t store(const exlib::string& member, Buffer_base* data, obj_ptr<ZipFile_base::ReadAllType>& retVal);
    bool get_pos(const exlib::string& member, unz64_file_pos& pos);
    static obj_ptr<cache_node> find(const exlib::string& name);
    static void erase(const exlib::string& name);

public:
    date_t m_date;
    // Nanosecond mtime plus size identify the archive the index describes.
    // Nanoseconds catch a rewrite within the same second that a coarse
    // timestamp would have hidden; the size catches the rest.
    int64_t m_mtime_ns = 0;
    double m_size = 0;
    std::mutex m_lock;
    // Set only for an archive mounted from memory (fs.setZipFS): its bytes are
    // pinned by the handle, so entries can be read from it directly.
    obj_ptr<ZipFile> m_zfile;
    std::unordered_map<exlib::string, ZipFile::index_entry> m_map;
    LruCache<obj_ptr<Buffer_base>> m_data;
};

static std::unordered_map<exlib::string, obj_ptr<cache_node>> s_cache_map;
static exlib::spinlock s_cachelock;

void cache_node::erase(const exlib::string& name)
{
    if (name.empty()) {
        s_cachelock.lock();
        s_cache_map.clear();
        s_cachelock.unlock();
    } else {
        exlib::string safe_name;
        path_base::normalize(name, safe_name);

        s_cachelock.lock();
        s_cache_map.erase(safe_name);
        s_cachelock.unlock();
    }
}

obj_ptr<cache_node> cache_node::find(const exlib::string& name)
{
    std::unordered_map<exlib::string, obj_ptr<cache_node>>::iterator it;
    obj_ptr<cache_node> node;

    s_cachelock.lock();
    it = s_cache_map.find(name);
    if (it != s_cache_map.end())
        node = it->second;

    s_cachelock.unlock();

    return node;
}

void cache_node::init(const exlib::string& name, std::unordered_map<exlib::string, ZipFile::index_entry>&& index,
    int64_t mtime_ns, double size, date_t date)
{
    m_date = date;
    m_mtime_ns = mtime_ns;
    m_size = size;
    m_map = std::move(index);

    // Publish only once the node is complete: a concurrent find() must never
    // see a node whose index or identity is still being filled in.
    s_cachelock.lock();
    s_cache_map.insert_or_assign(name, this);
    s_cachelock.unlock();
}

static obj_ptr<ZipFile_base::ReadAllType> make_info(const exlib::string& member, const ZipFile::index_entry& index, Buffer_base* data)
{
    obj_ptr<ZipFile_base::ReadAllType> zi = new ZipFile_base::ReadAllType();

    zi->filename = index.filename.empty() ? member : index.filename;
    zi->date = index.date;
    zi->compress_type = index.compress_type;
    zi->compress_size = index.compress_size;
    zi->file_size = index.file_size;
    zi->password = index.password;
    zi->data = data;

    return zi;
}

cache_node::lookup_result cache_node::lookup(const exlib::string& member, bool with_data, obj_ptr<ZipFile_base::ReadAllType>& retVal)
{
    std::lock_guard<std::mutex> lock(m_lock);

    std::unordered_map<exlib::string, ZipFile::index_entry>::iterator it = m_map.find(member);
    if (it == m_map.end())
        return kNotFound;

    if (with_data) {
        obj_ptr<Buffer_base> data;

        // This cache never installs a resolver, so the plain lookup avoids the
        // resolver copy and the wait path.
        if (!m_data.lookup(member, data, false)) {
            if (!m_zfile)
                return kNeedData;

            // In-memory archive: the ZipFile is stateful (the current entry),
            // so the read is serialized by the same lock.
            result_t hr = m_zfile->cc_read(member, "", data);
            if (hr.failed())
                return kNotFound;

            m_data.set(member, data);
        }

        retVal = make_info(it->first, it->second, data);
        return kHit;
    }

    retVal = make_info(it->first, it->second, NULL);
    return kHit;
}

result_t cache_node::store(const exlib::string& member, Buffer_base* data, obj_ptr<ZipFile_base::ReadAllType>& retVal)
{
    std::lock_guard<std::mutex> lock(m_lock);

    std::unordered_map<exlib::string, ZipFile::index_entry>::iterator it = m_map.find(member);
    if (it == m_map.end())
        return result_t(CALL_E_FILE_NOT_FOUND);

    m_data.set(member, data);
    retVal = make_info(it->first, it->second, data);

    return result_t();
}

bool cache_node::get_pos(const exlib::string& member, unz64_file_pos& pos)
{
    std::lock_guard<std::mutex> lock(m_lock);

    std::unordered_map<exlib::string, ZipFile::index_entry>::iterator it = m_map.find(member);
    if (it == m_map.end())
        return false;

    pos = it->second.pos;
    return true;
}

static result_t set_zip_fs(exlib::string fname, Buffer_base* data);

result_t fs_base::setZipFS(exlib::string fname, Union_setZipFS_data data)
{
    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        return set_zip_fs(fname, std::get<obj_ptr<Buffer_base>>(data).get());

    // a string is the zip data itself, encoded as utf8
    obj_ptr<Buffer_base> buf;
    result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", buf);
    if (hr.failed())
        return hr;

    return set_zip_fs(fname, buf.get());
}

static result_t set_zip_fs(exlib::string fname, Buffer_base* data)
{
    result_t hr;
    obj_ptr<ZipFile_base> zfile;
    obj_ptr<cache_node> _node;

    hr = zip_base::cc_open(data, "r", "utf-8", zfile);
    if (hr.failed())
        return hr;

    ZipFile* zf = static_cast<ZipFile*>(ZipFile_base::getInstance(zfile.get()));
    if (zf == NULL)
        return result_t(CALL_E_INVALID_CALL);

    // Parse the directory once: this validates the buffer at the call site (as
    // before) and gives the mapping its metadata, without inflating entries.
    std::unordered_map<exlib::string, ZipFile::index_entry> index;
    hr = zf->get_index(index);
    if (hr.failed())
        return hr;

    exlib::string safe_name;
    path_base::normalize(fname, safe_name);

    _node = new cache_node();
    // Set the pinned handle before init publishes the node: a lookup that
    // finds it must see an in-memory archive, not one that has to be opened
    // by path.
    _node->m_zfile = zf;
    _node->init(safe_name, std::move(index), 0, 0);

    return result_t();
}

result_t fs_base::clearZipFS(exlib::string fname)
{
    cache_node::erase(fname);
    return result_t();
}

// The mount lookup is a chain of synchronous-in-fiber steps (open -> stat ->
// read). Under the handle protocol a chain that reuses the continuation is
// expressed as a state machine whose steps borrow the machine through
// next(state) tickets - the only shape that keeps exactly one handle per event
// (plans/async-handle-protocol-minimal-2026-10-08.md 8.1 P3).
class asyncResolveZipFile : public AsyncState {
public:
    asyncResolveZipFile(exlib::string zip_file, exlib::string member, bool with_data, bool bChanged,
        exlib::string member1, obj_ptr<ZipFile_base::ReadAllType>& retVal, AsyncHandle ac)
        : AsyncState(ac)
        , m_zip_file(zip_file)
        , m_member(member)
        , m_with_data(with_data)
        , m_bChanged(bChanged)
        , m_member1(member1)
        , m_retVal(retVal)
    {
        init(check_cache);
    }

    ON_STATE(asyncResolveZipFile, check_cache)
    {
        m_node = cache_node::find(m_zip_file);
        m_now.now();

        if (m_node && (m_now.diff(m_node->m_date) > 3000))
            return next(stat_archive);

        return next(load);
    }

    ON_STATE(asyncResolveZipFile, stat_archive)
    {
        return fs_base::stat(m_zip_file, m_stat, next(refresh_done));
    }

    ON_STATE(asyncResolveZipFile, refresh_done)
    {
        int64_t _mtime_ns;
        double _size;

        m_stat->get_mtimeNs(_mtime_ns);
        m_stat->get_size(_size);

        if (_mtime_ns != m_node->m_mtime_ns || _size != m_node->m_size)
            m_node.Release();
        else
            m_node->m_date = m_now;

        return next(load);
    }

    ON_STATE(asyncResolveZipFile, load)
    {
        if (m_node == NULL)
            return next(open);

        return next(lookup);
    }

    ON_STATE(asyncResolveZipFile, open)
    {
        return fs_base::openFile(m_zip_file, "r", m_zip_stream, next(stat));
    }

    ON_STATE(asyncResolveZipFile, stat)
    {
        return fs_base::stat(m_zip_file, m_stat, next(open_zip));
    }

    ON_STATE(asyncResolveZipFile, open_zip)
    {
        m_zfile = new ZipFile(m_zip_stream.get(), "r", "utf-8");
        m_validated = true;

        int64_t _mtime_ns;
        double _size;

        m_stat->get_mtimeNs(_mtime_ns);
        m_stat->get_size(_size);

        if (m_node && _mtime_ns == m_node->m_mtime_ns && _size == m_node->m_size) {
            // The archive is the one the cached index describes.
            m_node->m_date = m_now;
            return next(lookup);
        }

        // Cold, or the archive changed: (re)build the index from this handle.
        // Replacing the node also drops its decompressed entries.
        std::unordered_map<exlib::string, ZipFile::index_entry> index;
        result_t hr = m_zfile->get_index(index);
        if (hr.failed())
            return hr;

        m_node = new cache_node();
        m_node->init(m_zip_file, std::move(index), _mtime_ns, _size, m_now);

        return next(lookup);
    }

    ON_STATE(asyncResolveZipFile, lookup)
    {
        cache_node::lookup_result lr = m_node->lookup(m_member, m_with_data, m_retVal);
#ifdef _WIN32
        if (lr == cache_node::kNotFound && m_bChanged) {
            lr = m_node->lookup(m_member1, m_with_data, m_retVal);
            if (lr != cache_node::kNotFound)
                m_member = m_member1;
        }
#endif

        if (lr == cache_node::kNotFound) {
            // A miss on a path-backed archive validates the file first: a
            // member that was written since the index was built must be
            // visible now, not after the 3s timer. A mounted archive is pinned
            // and has no path to re-read.
            if (!m_validated && !m_node->m_zfile)
                return next(open);

            return result_t(CALL_E_FILE_NOT_FOUND);
        }

        if (lr == cache_node::kNeedData) {
            if (m_zip_stream)
                return next(read_member);

            return next(open);
        }

        return next();
    }

    ON_STATE(asyncResolveZipFile, read_member)
    {
        unz64_file_pos pos;

        if (!m_node->get_pos(m_member, pos))
            return result_t(CALL_E_FILE_NOT_FOUND);

        obj_ptr<Buffer_base> data;
        result_t hr = m_zfile->read_at(pos, m_member, "", data);
        if (hr.failed())
            return result_t(CALL_E_FILE_NOT_FOUND);

        hr = m_node->store(m_member, data, m_retVal);
        if (hr.failed())
            return hr;

        return next();
    }

private:
    exlib::string m_zip_file;
    exlib::string m_member;
    bool m_with_data;
    bool m_bChanged;
    exlib::string m_member1;
    obj_ptr<ZipFile_base::ReadAllType>& m_retVal;
    obj_ptr<cache_node> m_node;
    obj_ptr<SeekableStream_base> m_zip_stream;
    obj_ptr<Stat_base> m_stat;
    obj_ptr<ZipFile> m_zfile;
    date_t m_now;
    bool m_validated = false;
};

static result_t resolve_zip_file(exlib::string fname, bool with_data, obj_ptr<ZipFile_base::ReadAllType>& retVal, AsyncHandle ac)
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

        return (new asyncResolveZipFile(zip_file, member, with_data, bChanged, member1, retVal, std::move(ac)))->post(result_t());
#else
        return (new asyncResolveZipFile(zip_file, member, with_data, false, exlib::string(), retVal, std::move(ac)))->post(result_t());
#endif
    }

    return result_t(CALL_E_FILE_NOT_FOUND);
}

static result_t zip_stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    obj_ptr<ZipFile_base::ReadAllType> zi;
    result_t hr = resolve_zip_file(path, false, zi, std::move(ac));
    if (hr.ok()) {
        obj_ptr<Stat> pStat = new Stat();
        pStat->init();

        path_base::basename(path, "", pStat->name);

        pStat->m_mode = S_IRUSR;
        pStat->size = zi->file_size;
        pStat->mtime = pStat->atime = pStat->ctime = pStat->birthtime = zi->date;
        pStat->m_isMemory = true;

        retVal = pStat;

        return result_t();
    }

    return result_t(CALL_E_FILE_NOT_FOUND);
}

result_t fs_base::lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(path, safe_name);
    if (hr.failed())
        return hr;

    // the isolate is read before the continuation is handed over (P4)
    Isolate* isolate = ac.isolate();

    hr = zip_stat(safe_name, retVal, std::move(ac));
    if (hr.ok())
        return result_t();

    if (!isolate->m_enable_FileSystem)
        return result_t::invalid_call();

    AutoReq req;
    int32_t ret = uv_fs_lstat(NULL, &req, safe_name.c_str(), NULL);
    if (ret < 0)
        return result_t::from_system(ret).with_syscall("lstat").with_path(path);

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(safe_name, &req.statbuf);
    retVal = pStat;

    return result_t();
}

result_t fs_base::lstat(exlib::string path, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        bool throwIfNoEntry = true;
        // A truthiness flag: Node accepts any value here and only reads it
        // for truth, so a wrong type is not an error.
        GetConfigValue(options, "throwIfNoEntry", throwIfNoEntry);

        // Node.js compatibility: throwIfNoEntry only affects the synchronous
        // (non-callback) forms; the async forms always report the error.
        ac.ctxv()[0] = throwIfNoEntry || ac.callType() == AsyncEvent::kAsyncCallBack;

        return result_t::nosync();
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr.failed())
        return ctx_hr;

    // the flag is read before the continuation is handed over (P4)
    bool throwIfNoEntry = ac.ctxv()[0].boolVal();

    result_t hr = lstat(path, retVal, std::move(ac));
    if (hr.failed() && !throwIfNoEntry && (hr.is(UV_ENOENT) || hr.is(UV_ENOTDIR)))
        return result_t::return_undefined();

    return hr;
}

result_t fs_base::stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(path, safe_name);
    if (hr.failed())
        return hr;

    // the isolate is read before the continuation is handed over (P4)
    Isolate* isolate = ac.isolate();

    hr = zip_stat(safe_name, retVal, std::move(ac));
    if (hr.ok())
        return result_t();

    if (!isolate->m_enable_FileSystem)
        return result_t::invalid_call();

    AutoReq req;
    int32_t ret = uv_fs_stat(NULL, &req, safe_name.c_str(), NULL);
    if (ret < 0)
        return result_t::from_system(ret).with_syscall("stat").with_path(path);

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(safe_name, &req.statbuf);
    retVal = pStat;

    return result_t();
}

result_t fs_base::stat(exlib::string path, v8::Local<v8::Object> options, obj_ptr<Stat_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(1);

        bool throwIfNoEntry = true;
        // A truthiness flag: Node accepts any value here and only reads it
        // for truth, so a wrong type is not an error.
        GetConfigValue(options, "throwIfNoEntry", throwIfNoEntry);

        // Node.js compatibility: throwIfNoEntry only affects the synchronous
        // (non-callback) forms; the async forms always report the error.
        ac.ctxv()[0] = throwIfNoEntry || ac.callType() == AsyncEvent::kAsyncCallBack;

        return result_t::nosync();
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr.failed())
        return ctx_hr;

    // the flag is read before the continuation is handed over (P4)
    bool throwIfNoEntry = ac.ctxv()[0].boolVal();

    result_t hr = stat(path, retVal, std::move(ac));
    if (hr.failed() && !throwIfNoEntry && (hr.is(UV_ENOENT) || hr.is(UV_ENOTDIR)))
        return result_t::return_undefined();

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
        return result_t::nosync();

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr.failed())
        return hr;

    // the isolate is read before the continuation is handed over (P4)
    Isolate* isolate = ac.isolate();

    obj_ptr<ZipFile_base::ReadAllType> zi;
    hr = resolve_zip_file(safe_name, true, zi, std::move(ac));
    if (hr.ok()) {
        // Share the cached entry bytes with the stream; the previous code
        // copied them into a string and then again into the stream, so every
        // fs.readFile of a zipped file moved its content three times.
        obj_ptr<Buffer_base> data = zi->data;
        if (!data)
            data = new Buffer();

        retVal = new MemoryStream::CloneStream(data, zi->date);
        return result_t();
    }

    if (!isolate->m_enable_FileSystem)
        return result_t::invalid_call();

    obj_ptr<FileStream> pFile = new FileStream();
    hr = pFile->open(safe_name, flags);
    if (hr.failed())
        return hr;

    retVal = pFile;

    return result_t();
}

static result_t open_file_int(exlib::string fname, int32_t flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    if (!ac.isolate()->m_enable_FileSystem)
        return result_t::invalid_call();

    exlib::string safe_name;
    result_t hr = normalize_file_path_like(fname, safe_name);
    if (hr.failed())
        return hr;

    int32_t _fd;
    hr = file_open(safe_name, flags, 0666, _fd);
    if (hr.failed())
        return hr;

    retVal = new FileStream(_fd);

    return result_t();
}
}
