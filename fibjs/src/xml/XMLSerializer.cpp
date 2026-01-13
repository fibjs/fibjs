/*
 * XMLSerializer.cpp
 *
 *  Created on: Jan 13, 2026
 *      Author: lion
 */

#include "object.h"
#include "ifs/XMLSerializer.h"
#include "ifs/xml.h"
#include "XMLSerializer.h"

namespace fibjs {

DECLARE_MODULE(XMLSerializer);

result_t XMLSerializer_base::_new(obj_ptr<XMLSerializer_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new XMLSerializer();
    return 0;
}

result_t XMLSerializer::serializeToString(XmlNode_base* node, exlib::string& retVal)
{
    return xml_base::serialize(node, retVal);
}

} // namespace fibjs
