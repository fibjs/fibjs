/*
 * fs.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ifs/zip.h"
#include "path.h"
#include "Stat.h"
#include "File.h"
#include "MemoryStream.h"
#include "ZipFile.h"
#include "AsyncUV.h"
#include <list>

namespace fibjs {

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

    hr = zip_base::cc_open(data, "r", "utf-8", zfile);
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

static result_t resolve_zip_file(exlib::string fname, obj_ptr<ZipFile::Info>& retVal, AsyncEvent* ac)
{
    size_t pos = fname.find('$');
    if (pos != exlib::string::npos && fname[pos + 1] == PATH_SLASH) {
        exlib::string zip_file = fname.substr(0, pos);
        exlib::string member = fname.substr(pos + 2);
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
            char* _member = NULL;
            for (int32_t i = 0; i < sz; i++)
                if (buf[i] == PATH_SLASH) {
                    if (!_member)
                        _member = member.c_buffer();
                    _member[i] = '/';
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
            hr = fs_base::openFile(zip_file, "r", zip_stream, ac);
            if (hr < 0)
                return hr;

            hr = zip_stream->stat(stat, ac);
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
                hr = fs_base::openFile(zip_file, "r", zip_stream, ac);
                if (hr < 0)
                    return hr;

                hr = zip_stream->stat(stat, ac);
                if (hr < 0)
                    return hr;
            }

            obj_ptr<Buffer_base> zip_data;
            hr = zip_stream->readAll(zip_data, ac);
            if (hr < 0)
                return hr;

            hr = zip_base::open(zip_data, "r", "utf-8", zfile, ac);
            if (hr < 0)
                return hr;

            obj_ptr<NArray> list;
            hr = zfile->readAll("", list, ac);
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

        len = _node->m_list->length();

        for (i = 0; i < len; i++) {
            Variant v;
            exlib::string s;

            _node->m_list->_indexed_getter(i, v);

            retVal = (ZipFile::Info*)v.object();
            if (retVal->m_date.empty())
                retVal->m_date = _node->m_date;

            retVal->get_filename(s);

            if (member == s)
                return 0;

#ifdef _WIN32
            if (bChanged && member1 == s)
                return 0;
#endif
        }
    }

    return CALL_E_FILE_NOT_FOUND;
}

static result_t zip_stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    obj_ptr<ZipFile::Info> zi;
    result_t hr = resolve_zip_file(path, zi, ac);
    if (hr >= 0) {
        obj_ptr<Stat> pStat = new Stat();
        pStat->init();

        path_base::basename(path, "", pStat->name);

        pStat->m_mode = S_IRUSR;
        pStat->size = zi->m_file_size;
        pStat->mtime = pStat->atime = pStat->ctime = pStat->birthtime = zi->m_date;
        pStat->m_isMemory = true;

        retVal = pStat;

        return 0;
    }

    return CALL_E_FILE_NOT_FOUND;
}

result_t fs_base::lstat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    path_base::normalize(path, safe_name);

    result_t hr = zip_stat(safe_name, retVal, ac);
    if (hr >= 0)
        return 0;

    if (!ac->isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    AutoReq req;
    int32_t ret = uv_fs_lstat(NULL, &req, safe_name.c_str(), NULL);
    if (ret < 0)
        return ret;

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(safe_name, &req.statbuf);
    retVal = pStat;

    return 0;
}

result_t fs_base::stat(exlib::string path, obj_ptr<Stat_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    path_base::normalize(path, safe_name);

    result_t hr = zip_stat(safe_name, retVal, ac);
    if (hr >= 0)
        return 0;

    if (!ac->isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    AutoReq req;
    int32_t ret = uv_fs_stat(NULL, &req, safe_name.c_str(), NULL);
    if (ret < 0)
        return ret;

    obj_ptr<Stat> pStat = new Stat();

    pStat->fill(safe_name, &req.statbuf);
    retVal = pStat;

    return 0;
}

result_t fs_base::openFile(exlib::string fname, exlib::string flags,
    obj_ptr<SeekableStream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string safe_name;
    path_base::normalize(fname, safe_name);

    obj_ptr<ZipFile::Info> zi;
    result_t hr = resolve_zip_file(safe_name, zi, ac);
    if (hr >= 0) {
        obj_ptr<Buffer_base> data;
        exlib::string strData;
        date_t _d;

        zi->get_data(data);
        if (data)
            data->toString(strData);

        zi->get_date(_d);
        retVal = new MemoryStream::CloneStream(strData, _d);
        return 0;
    }

    if (!ac->isolate()->m_enable_FileSystem)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    obj_ptr<File> pFile = new File();
    hr = pFile->open(safe_name, flags);
    if (hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}
}
