/*
 * DOMParser.h
 *
 *  Created on: Jan 13, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/DOMParser.h"

namespace fibjs {

class DOMParser : public DOMParser_base {
public:
    // DOMParser_base
    virtual result_t parseFromString(exlib::string str, exlib::string mimeType,
        obj_ptr<XmlDocument_base>& retVal);
};

} // namespace fibjs
