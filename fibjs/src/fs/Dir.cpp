/*
 * Dir.cpp
 *
 *  Created on: Sep 24, 2026
 *      Author: fibjs
 */

#include "Dir.h"
#include "DirEntry.h"
#include "file_path.h"
#include "path.h"
#include "Iterator.h"
#include "utils.h"

namespace fibjs {

result_t Dir_base::_new(exlib::string path, obj_ptr<Dir_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Dir(path);
    return 0;
}

result_t Dir::get_path(exlib::string& retVal)
{
    retVal = m_path;
    return 0;
}

result_t Dir::load()
{
    if (m_closed)
        return CHECK_ERROR(CALL_E_CLOSED);

    if (m_loaded)
        return 0;

    setErrorContext("scandir", m_path);

    exlib::string path;
    result_t hr = normalize_file_path_like(m_path, path);
    if (hr < 0)
        return hr;

    os_normalize(path, path, true);

    AutoReq req;
    int32_t ret = uv_fs_scandir(NULL, &req, path.c_str(), 0, NULL);
    if (ret < 0)
        return ret;

    uv_dirent_t dirent;
    while (uv_fs_scandir_next(&req, &dirent) != UV_EOF) {
        obj_ptr<DirEntry> pDirEntry = new DirEntry();
        pDirEntry->fill(dirent.name, path, dirent_type_to_mode(dirent.type));
        m_entries.push_back(pDirEntry);
    }

    m_loaded = true;

    return 0;
}

void Dir::unload()
{
    m_entries.clear();
    m_index = 0;
    m_loaded = false;
    m_closed = true;
}

result_t Dir::read(obj_ptr<DirEntry_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    // Node.js: a closed directory reports the end of the iteration
    if (m_closed)
        return CALL_RETURN_NULL;

    result_t hr = load();
    if (hr < 0)
        return hr;

    // Node.js: read() returns null once the iteration is finished
    if (m_index >= m_entries.size())
        return CALL_RETURN_NULL;

    retVal = m_entries[m_index++];

    return 0;
}

result_t Dir::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    unload();

    return 0;
}

result_t Dir::symbol_iterator(obj_ptr<Iterator_base>& retVal)
{
    retVal = new Iterator(this, [this](size_t index, Variant& retVal, Iterator::IteratorCallback cb) {
        result_t hr = load();
        if (hr < 0) {
            cb(hr, false);
            return;
        }

        if (index >= m_entries.size()) {
            cb(0, false);
            return;
        }

        retVal = m_entries[index];
        cb(0, true);
    });

    return 0;
}

result_t Dir::symbol_asyncIterator(obj_ptr<Iterator_base>& retVal)
{
    return symbol_iterator(retVal);
}

result_t Dir::next(obj_ptr<NextType>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = load();
    if (hr < 0)
        return hr;

    obj_ptr<NextType> result = new NextType();

    if (m_index >= m_entries.size()) {
        result->done = true;
    } else {
        result->value = m_entries[m_index++];
        result->done = false;
    }

    retVal = result;

    return 0;
}

result_t Dir::_return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal)
{
    unload();

    obj_ptr<ReturnType> result = new ReturnType();
    result->done = true;

    retVal = result;

    return 0;
}

} // namespace fibjs
