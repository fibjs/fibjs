/*
 * console_file.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: lion
 */

#include "console.h"
#include "File.h"
#include "ifs/fs.h"
#include "path.h"
#include "Buffer.h"
#include <algorithm>

namespace fibjs {

#define MAX_COUNT 128

result_t file_logger::config(Isolate* isolate, v8::Local<v8::Object> o)
{
    result_t hr = logger::config(isolate, o);
    if (hr < 0)
        return hr;

    exlib::string path;
    hr = GetConfigValue(isolate->m_isolate, o, "path", path);
    if (hr < 0)
        return hr;

    path_base::normalize(path, path);
    path_base::dirname(path, m_folder);
    if (m_folder.length())
        m_folder += PATH_SLASH;

    path_base::basename(path, "", m_name1);
    size_t idx = m_name1.find("%s", 0, 2);
    if (idx != exlib::string::npos) {
        m_name2 = m_name1.substr(idx + 2);
        m_name1 = m_name1.substr(0, idx);
    }

    m_split_size = 0;
    m_split_mode = 0;

    exlib::string split("1m");
    hr = GetConfigValue(isolate->m_isolate, o, "split", split);
    if (hr >= 0) {
        if ((split == "day"))
            m_split_mode = date_t::_DAY;
        else if ((split == "hour"))
            m_split_mode = date_t::_HOUR;
        else if ((split == "minute"))
            m_split_mode = date_t::_MINUTE;
        else {
            int32_t l = (int32_t)split.length();
            int32_t i;

            if (l > 4 || l < 2)
                return CHECK_ERROR(Runtime::setError("Unknown split mode."));

            for (i = 0; i < l - 1; i++)
                if (!qisdigit(split[i]))
                    return CHECK_ERROR(Runtime::setError("Unknown split mode."));
                else
                    m_split_size = m_split_size * 10 + split[i] - '0';

            if (split[i] == 'k')
                m_split_size <<= 10;
            else if (split[i] == 'm')
                m_split_size <<= 20;
            else if (split[i] == 'g')
                m_split_size <<= 30;
            else
                return CHECK_ERROR(Runtime::setError("Unknown split mode."));
        }
    } else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate->m_isolate, o, "count", m_count);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        m_count = MAX_COUNT;
    else if (hr < 0)
        return hr;
    else {
        if (m_split_size == 0 && m_split_mode == 0)
            return CHECK_ERROR(Runtime::setError("Missing split mode."));

        if (m_count < 2 || m_count > MAX_COUNT)
            return CHECK_ERROR(Runtime::setError("Count must between 2 to 128."));
    }

    return 0;
}

void file_logger::clearFile()
{
    obj_ptr<List_base> fd;
    result_t hr;

    exlib::string name;

    if (m_folder.empty())
        hr = fs_base::cc_readdir(".", fd);
    else
        hr = fs_base::cc_readdir(m_folder, fd);
    if (hr < 0)
        return;

    std::vector<exlib::string> files;
    int32_t sz = 0, i;

    fd->get_length(sz);

    for (i = 0; i < sz; i++) {
        Variant v;

        fd->_indexed_getter(i, v);
        name = v.string();

        const char* c_str = name.c_str();
        if ((name.length() == m_name1.length() + m_name2.length() + 14)
            && !qstrcmp(c_str, m_name1.c_str(), (int32_t)m_name1.length())
            && !qstrcmp(c_str + m_name1.length() + 14, m_name2.c_str(), (int32_t)m_name2.length())) {
            int32_t p, l;

            l = (int32_t)m_name1.length() + 14;
            for (p = (int32_t)m_name1.length(); p < l && qisdigit(c_str[p]); p++)
                ;

            if (p == l) {
                exlib::string p(m_folder);
                resolvePath(p, name);
                files.push_back(p);
            }
        }
    }

    if ((int32_t)files.size() > m_count - 1) {
        std::sort(files.begin(), files.end());

        int32_t dels = (int32_t)files.size() - m_count + 1;

        for (i = 0; i < dels; i++)
            fs_base::cc_unlink(files[i]);
    }
}

result_t file_logger::initFile()
{
    if (m_split_mode && m_file) {
        date_t d;

        d.now();
        if (d.diff(m_date) >= 0)
            m_file.Release();
    }

    result_t hr;

    if (!m_file) {
        obj_ptr<File> f = new File();
        exlib::string name(m_folder);
        resolvePath(name, m_name1);

        if (m_count > 1) {
            exlib::string tm;

            m_date.now();
            if (m_split_mode)
                m_date.fix(m_split_mode);
            m_date.stamp(tm);
            name.append(tm);
            if (m_split_mode)
                m_date.add(1, m_split_mode);

            if (m_count <= MAX_COUNT)
                clearFile();
        }
        name.append(m_name2);

        hr = f->open(name, "a+");
        if (hr < 0)
            return hr;

        hr = f->tell(m_size);
        if (hr < 0)
            return hr;

        m_file = f;
    }

    return 0;
}

result_t file_logger::write(AsyncEvent* ac)
{
    item* p1;

    while (!m_workinglogs.empty()) {
        exlib::string outBuffer;
        result_t hr;

        hr = initFile();
        if (hr < 0) {
            while ((p1 = m_workinglogs.getHead()) != 0)
                delete p1;

            break;
        }

        while ((p1 = m_workinglogs.getHead()) != 0) {
            if (p1->m_priority != console_base::_PRINT) {
                outBuffer.append(p1->full());
                outBuffer.append("\n", 1);
            }

            delete p1;

            if (outBuffer.length() > STREAM_BUFF_SIZE)
                break;

            if (m_split_size && m_size + (int32_t)outBuffer.length() >= m_split_size)
                break;
        }

        if (m_file) {
            hr = m_file->Write(outBuffer);
            if (hr < 0)
                m_file.Release();

            m_size += outBuffer.length();
            if (m_split_size && m_size >= m_split_size)
                m_file.Release();
        }
    }

    return 0;
}
}
