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
#include "XmlElement.h"

namespace fibjs {

DECLARE_MODULE(XMLSerializer);

result_t XMLSerializer_base::_new(obj_ptr<XMLSerializer_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new XMLSerializer();
    return 0;
}

result_t XMLSerializer::serializeToString(XmlNode_base* node, exlib::string& retVal)
{
    // XMLSerializer should serialize nodes using XML rules,
    // even if the underlying DOM was parsed as HTML.
    if (node) {
        int32_t type;
        node->get_nodeType(type);

        if (type == xml_base::C_ELEMENT_NODE)
            return ((XmlElement*)node)->toXmlString(retVal);
    }

    return xml_base::serialize(node, retVal);
}

} // namespace fibjs
