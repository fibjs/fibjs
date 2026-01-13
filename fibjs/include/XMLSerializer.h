/*
 * XMLSerializer.h
 *
 *  Created on: Jan 13, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/XMLSerializer.h"

namespace fibjs {

class XMLSerializer : public XMLSerializer_base {
public:
    // XMLSerializer_base
    virtual result_t serializeToString(XmlNode_base* node, exlib::string& retVal);
};

} // namespace fibjs
