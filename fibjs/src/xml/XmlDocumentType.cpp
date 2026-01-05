/*
 * XmlDocumentType.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlDocumentType.h"

namespace fibjs {

result_t XmlDocumentType::get_nodeName(exlib::string& retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlDocumentType::get_nodeValue(exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::set_nodeValue(exlib::string newVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::get_textContent(exlib::string& retVal)
{
    return 0;
}

result_t XmlDocumentType::set_textContent(exlib::string newVal)
{
    return 0;
}

result_t XmlDocumentType::lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlDocumentType> doctype = new XmlDocumentType(*this);
    return XmlNodeImpl::cloneNode(doctype, deep, retVal);
}

result_t XmlDocumentType::get_name(exlib::string& retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlDocumentType::get_publicId(exlib::string& retVal)
{
    retVal = m_publicId;
    return 0;
}

result_t XmlDocumentType::get_systemId(exlib::string& retVal)
{
    retVal = m_systemId;
    return 0;
}

result_t XmlDocumentType::toString(exlib::string& retVal)
{
    retVal = "<!DOCTYPE ";
    retVal.append(m_name);

    if (!m_publicId.empty()) {
        retVal.append(" PUBLIC \"");
        retVal.append(m_publicId);
        retVal += '\"';
    }

    if (!m_systemId.empty()) {
        if (m_publicId.empty())
            retVal.append(" SYSTEM \"");
        else
            retVal.append(" \"");

        retVal.append(m_systemId);
        retVal += '\"';
    }

    retVal += '>';

    return 0;
}
}
