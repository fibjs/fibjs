/*
 * HttpFileHandler.h
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Handler.h"
#include <unordered_map>
#include "path.h"

namespace fibjs {

class HttpFileHandler : public Handler_base {
    FIBER_FREE();

private:
    HttpFileHandler(exlib::string root, bool isDir, bool autoIndex)
        : m_root(root)
        , m_isDir(isDir)
        , m_autoIndex(autoIndex)
    {
        if (!m_root.empty() && isDir && !isPathSlash(m_root.c_str()[m_root.length() - 1]))
            m_root += PATH_SLASH;
    }

public:
    static result_t create(exlib::string root, bool autoIndex, obj_ptr<Handler_base>& retVal);

public:
    // Handler_base
    virtual result_t isRouting(bool& retVal);
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

private:
    exlib::string m_root;
    bool m_isDir;
    bool m_autoIndex;
};

} /* namespace fibjs */
