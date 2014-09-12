/*
 * XmlNodeImpl.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "XmlNodeImpl.h"
#include "XmlDocument.h"
#include "XmlDocumentType.h"
#include "XmlElement.h"
#include "XmlAttr.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"

namespace fibjs
{

result_t XmlNodeImpl::toNode(obj_ptr<XmlNode_base> &retVal)
{
    switch (m_type)
    {
    case xml_base::_ELEMENT_NODE:
        retVal = (XmlElement *)this;
        return 0;
    case xml_base::_ATTRIBUTE_NODE:
        retVal = (XmlAttr *)this;
        return 0;
    case xml_base::_TEXT_NODE:
        retVal = (XmlText *)this;
        return 0;
    case xml_base::_CDATA_SECTION_NODE:
        retVal = (XmlCDATASection *)this;
        return 0;
    case xml_base::_PROCESSING_INSTRUCTION_NODE:
        retVal = (XmlProcessingInstruction *)this;
        return 0;
    case xml_base::_COMMENT_NODE:
        retVal = (XmlComment *)this;
        return 0;
    case xml_base::_DOCUMENT_NODE:
        retVal = (XmlDocument *)this;
        return 0;
    case xml_base::_DOCUMENT_TYPE_NODE:
        retVal = (XmlDocumentType *)this;
        return 0;
    }

    return CALL_E_INTERNAL;
}

XmlNodeImpl *XmlNodeImpl::fromNode(XmlNode_base *pNode)
{
    int32_t type;

    pNode->get_nodeType(type);

    switch (type)
    {
    case xml_base::_ELEMENT_NODE:
        return (XmlElement *)pNode;
    case xml_base::_ATTRIBUTE_NODE:
        return (XmlAttr *)pNode;
    case xml_base::_TEXT_NODE:
        return (XmlText *)pNode;
    case xml_base::_CDATA_SECTION_NODE:
        return (XmlCDATASection *)pNode;
    case xml_base::_PROCESSING_INSTRUCTION_NODE:
        return (XmlProcessingInstruction *)pNode;
    case xml_base::_COMMENT_NODE:
        return (XmlComment *)pNode;
    case xml_base::_DOCUMENT_NODE:
        return (XmlDocument *)pNode;
    case xml_base::_DOCUMENT_TYPE_NODE:
        return (XmlDocumentType *)pNode;
    }

    return NULL;
}

result_t XmlNodeImpl::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    return 0;
}

result_t XmlNodeImpl::normalize()
{
    return 0;
}

}
