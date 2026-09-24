/*
 * Dir.h
 *
 *  Created on: Sep 24, 2026
 *      Author: fibjs
 */

#pragma once

#include "ifs/Dir.h"
#include "ifs/DirEntry.h"
#include "DirEntry.h"
#include "AsyncUV.h"
#include <vector>

namespace fibjs {

class Dir : public Dir_base {
public:
    Dir(exlib::string path)
        : m_path(path)
    {
    }

public:
    // Dir_base
    virtual result_t get_path(exlib::string& retVal);
    virtual result_t read(obj_ptr<DirEntry_base>& retVal, AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);

    // Iterator_base
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal);
    virtual result_t symbol_asyncIterator(obj_ptr<Iterator_base>& retVal);
    virtual result_t next(obj_ptr<NextType>& retVal, AsyncEvent* ac);
    virtual result_t _return(v8::Local<v8::Value> value, obj_ptr<ReturnType>& retVal);

private:
    result_t load();
    void unload();

private:
    exlib::string m_path;
    size_t m_index = 0;
    bool m_loaded = false;
    bool m_closed = false;
    std::vector<obj_ptr<DirEntry>> m_entries;
};

} // namespace fibjs
