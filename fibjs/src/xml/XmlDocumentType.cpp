/*
 * XmlDocumentType.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlDocumentType.h"

namespace fibjs
{

result_t XmlDocumentType::get_nodeName(std::string &retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlDocumentType::get_nodeValue(std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::set_nodeValue(const char *newVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlDocumentType::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlDocumentType::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlDocumentType::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlDocumentType::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlDocumentType::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlDocumentType::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlDocumentType::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlDocumentType::lookupPrefix(const char *namespaceURI, std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::lookupNamespaceURI(const char *prefix, std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentType::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                                       obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlDocumentType::insertAfter(XmlNode_base *newChild, XmlNode_base *refChild,
                                      obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlDocumentType::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                                       obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlDocumentType::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlDocumentType::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlDocumentType::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlDocumentType::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    obj_ptr<XmlDocumentType> doctype = new XmlDocumentType(*this);
    return XmlNodeImpl::cloneNode(doctype, deep, retVal);
}

result_t XmlDocumentType::normalize()
{
    return m_childs->normalize();
}

result_t XmlDocumentType::get_name(std::string &retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlDocumentType::get_publicId(std::string &retVal)
{
    retVal = m_publicId;
    return 0;
}

result_t XmlDocumentType::get_systemId(std::string &retVal)
{
    retVal = m_systemId;
    return 0;
}

result_t XmlDocumentType::toString(std::string &retVal)
{
    retVal = "<!DOCTYPE ";
    retVal.append(m_name);

    if (!m_publicId.empty())
    {
        retVal.append(" PUBLIC \"");
        retVal.append(m_publicId);
        retVal += '\"';
    }

    if (!m_systemId.empty())
    {
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
