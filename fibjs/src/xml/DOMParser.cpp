/*
 * DOMParser.cpp
 *
 *  Created on: Jan 13, 2026
 *      Author: lion
 */

#include "object.h"
#include "ifs/DOMParser.h"
#include "ifs/xml.h"
#include "DOMParser.h"

namespace fibjs {

DECLARE_MODULE(DOMParser);

result_t DOMParser_base::_new(obj_ptr<DOMParser_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new DOMParser();
    return 0;
}

result_t DOMParser::parseFromString(exlib::string str, exlib::string mimeType,
    obj_ptr<XmlDocument_base>& retVal)
{
    exlib::string type;

    if (mimeType == "text/html") {
        type = "text/html";
    } else if (mimeType == "text/xml" || mimeType == "application/xml"
        || mimeType == "application/xhtml+xml" || mimeType == "image/svg+xml") {
        type = "text/xml";
    } else {
        return CHECK_ERROR(Runtime::setError("DOMParser: Invalid MIME type: " + mimeType));
    }

    return xml_base::parse(str, type, retVal);
}

} // namespace fibjs
