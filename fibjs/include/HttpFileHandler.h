/*
 * HttpFileHandler.h
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"

#ifndef HTTPFILEHANDLER_H_
#define HTTPFILEHANDLER_H_

namespace fibjs
{

class HttpFileHandler: public Handler_base
{
public:
    HttpFileHandler(const char *root) :
        m_root(root)
    {
        if (m_root.length() > 0 && !isPathSlash(m_root[m_root.length() - 1]))
            m_root += PATH_SLASH;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            exlib::AsyncEvent *ac);

private:
    std::string m_root;
};

} /* namespace fibjs */
#endif /* HTTPFILEHANDLER_H_ */
