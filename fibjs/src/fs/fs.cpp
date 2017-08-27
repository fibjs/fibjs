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
#include "List.h"
#include "File.h"
#include "BufferedStream.h"
#include "MemoryStream.h"

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
    obj_ptr<List_base> m_list;
};

static std::list<obj_ptr<cache_node>> s_cache;
static exlib::spinlock s_cachelock;

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
            hr = cc_openFile(zip_file, "r", zip_stream);
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
                hr = cc_openFile(zip_file, "r", zip_stream);
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

            obj_ptr<List_base> list;
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
    result_t hr = cc_openFile(fname, flags, pFile);
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
    if (ac->isSync())
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
    if (ac->isSync())
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
    if (ac->isSync())
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
    if (ac->isSync())
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
    if (ac->isSync())
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
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stat> pStat = new Stat();

    result_t hr = pStat->getLstat(path);
    if (hr < 0)
        return hr;

    retVal = pStat;

    return 0;
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
        return Runtime::setError("Offset is out of bounds");

    exlib::string strBuf;

    if (length < 0 || (offset + length > bufLength)) {
        return Runtime::setError("Length extends beyond buffer");
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
}
