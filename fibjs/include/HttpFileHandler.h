/*
 * HttpFileHandler.h
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"
#include "map"

#ifndef HTTPFILEHANDLER_H_
#define HTTPFILEHANDLER_H_

namespace fibjs {

class HttpFileHandler : public Handler_base {
    FIBER_FREE();

public:
    HttpFileHandler(exlib::string root, bool autoIndex)
        : m_root(root)
        , m_autoIndex(autoIndex)
    {
        if (!m_root.empty() && !isPathSlash(m_root.c_str()[m_root.length() - 1]))
            m_root += PATH_SLASH;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

    result_t set_mimes(v8::Local<v8::Object> mimes);

private:
    exlib::string m_root;
    bool m_autoIndex;
    std::map<exlib::string, exlib::string> m_mimes;
};

} /* namespace fibjs */
#endif /* HTTPFILEHANDLER_H_ */
