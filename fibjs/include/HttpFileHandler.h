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

public:
    HttpFileHandler(exlib::string root, bool autoIndex)
        : m_autoIndex(autoIndex)
    {
        path_base::normalize(root, m_root);
        if (!m_root.empty() && !isPathSlash(m_root.c_str()[m_root.length() - 1]))
            m_root += PATH_SLASH;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

private:
    exlib::string m_root;
    bool m_autoIndex;
};

} /* namespace fibjs */
