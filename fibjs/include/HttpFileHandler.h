/*
 * HttpFileHandler.h
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Handler.h"
#include <unordered_map>
#include <vector>
#include <utility>
#include "path.h"
#include "../src/fs/match/path_match.h"

namespace fibjs {

class HttpResponse_base;

class HttpFileHandler : public Handler_base {
    FIBER_FREE();

private:
    HttpFileHandler(exlib::string root, bool isDir, bool autoIndex)
        : m_root(root)
        , m_isDir(isDir)
        , m_autoIndex(autoIndex)
    {
        if (!m_root.empty() && isDir && !isPathSlash(m_root[m_root.length() - 1]))
            m_root += PATH_SLASH;
    }

public:
    static result_t create(exlib::string root, bool autoIndex, obj_ptr<Handler_base>& retVal);
    static result_t create(exlib::string root, v8::Local<v8::Object> options, obj_ptr<Handler_base>& retVal);

public:
    // Handler_base
    virtual result_t isRouting(bool& retVal);
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

private:
    struct HeaderRule {
        HeaderRule(MinimatchPattern&& pattern_)
            : pattern(std::move(pattern_))
        {
        }

        MinimatchPattern pattern;
        std::vector<std::pair<exlib::string, exlib::string>> headers;
    };

    void applyHeaders(const exlib::string& target, HttpResponse_base* rep);

private:
    exlib::string m_root;
    bool m_isDir;
    bool m_autoIndex;
    bool m_cacheControl = true;
    int64_t m_maxAge = 0;
    bool m_immutable = false;
    std::vector<HeaderRule> m_headers;
};

} /* namespace fibjs */
