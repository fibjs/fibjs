/*
 * XmlDocument.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlDocument.h"
#include "XmlElement.h"
#include "XmlAttr.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"
#include "XmlParser.h"

namespace fibjs
{

result_t XmlDocument_base::_new(const char *source, obj_ptr<XmlDocument_base> &retVal,
                                v8::Local<v8::Object> This)
{
    return xml_base::parse(source, retVal);
}

result_t xml_base::parse(const char *source, obj_ptr<XmlDocument_base> &retVal)
{
    retVal = new XmlDocument();
    if (!*source)
        return 0;

    return retVal->loadXML(source);
}

result_t XmlDocument::get_nodeName(std::string &retVal)
{
    retVal = "#document";
    return 0;
}

result_t XmlDocument::get_nodeValue(std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::set_nodeValue(const char *newVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlDocument::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlDocument::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlDocument::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlDocument::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlDocument::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlDocument::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlDocument::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlDocument::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                                   obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlDocument::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                                   obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlDocument::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlDocument::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlDocument::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlDocument::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::cloneNode(deep, retVal);
}

result_t XmlDocument::normalize()
{
    return XmlNodeImpl::normalize();
}

result_t XmlDocument::loadXML(const char *source)
{
    XmlParser parser;
    return parser.parse(source);
}

result_t XmlDocument::get_doctype(obj_ptr<XmlDocumentType_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::get_documentElement(obj_ptr<XmlElement_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::createElement(const char *tagName, obj_ptr<XmlElement_base> &retVal)
{
    retVal = new XmlElement(this, tagName);
    return 0;
}

result_t XmlDocument::createTextNode(const char *data, obj_ptr<XmlText_base> &retVal)
{
    retVal = new XmlText(this, data);
    return 0;
}

result_t XmlDocument::createComment(const char *data, obj_ptr<XmlComment_base> &retVal)
{
    retVal = new XmlComment(this, data);
    return 0;
}

result_t XmlDocument::createCDATASection(const char *data, obj_ptr<XmlCDATASection_base> &retVal)
{
    retVal = new XmlCDATASection(this, data);
    return 0;
}

result_t XmlDocument::createProcessingInstruction(const char *target, const char *data,
        obj_ptr<XmlProcessingInstruction_base> &retVal)
{
    retVal = new XmlProcessingInstruction(this, target, data);
    return 0;
}

result_t XmlDocument::createAttribute(const char *name, obj_ptr<XmlAttr_base> &retVal)
{
    retVal = new XmlAttr(this, name);
    return 0;
}

result_t XmlDocument::getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::getElementById(const char *elementId, obj_ptr<XmlElement_base> &retVal)
{
    return CALL_RETURN_NULL;
}

}
