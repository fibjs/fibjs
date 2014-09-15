/*
 * XmlElement.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlElement.h"
#include "XmlNodeImpl.h"
#include "XmlNodeList.h"
#include "XmlNamedNodeMap.h"

#ifndef XMLELEMENT_H_
#define XMLELEMENT_H_

namespace fibjs
{

class XmlElement: public XmlElement_base, public XmlNodeImpl
{
public:
    XmlElement(XmlDocument_base *document, const char *tagName):
        XmlNodeImpl(document, this, xml_base::_ELEMENT_NODE),
        m_tagName(tagName), m_attrs(new XmlNamedNodeMap(this))
    {
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeName(std::string &retVal);
    virtual result_t get_nodeValue(std::string &retVal);
    virtual result_t set_nodeValue(const char *newVal);
    virtual result_t get_nodeType(int32_t &retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base> &retVal);
    virtual result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t hasChildNodes(bool &retVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal);
    virtual result_t normalize();

public:
    // XmlElement_base
    virtual result_t get_tagName(std::string &retVal);
    virtual result_t get_attributes(obj_ptr<XmlNamedNodeMap_base> &retVal);
    virtual result_t getAttribute(const char *name, std::string &retVal);
    virtual result_t setAttribute(const char *name, const char *value);
    virtual result_t removeAttribute(const char *name);
    virtual result_t getAttributeNode(const char *name, obj_ptr<XmlAttr_base> &retVal);
    virtual result_t setAttributeNode(XmlAttr_base *newAttr, obj_ptr<XmlAttr_base> &retVal);
    virtual result_t removeAttributeNode(XmlAttr_base *oldAttr, obj_ptr<XmlAttr_base> &retVal);
    virtual result_t getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t hasAttribute(bool &retVal);

public:
    void getElementsByTagNameFromThis(const char *tagName, obj_ptr<XmlNodeList> &retVal)
    {
        if (!qstrcmp(m_tagName.c_str(), tagName))
            retVal->appendChild(this);
        getElementsByTagName(tagName, retVal);
    }

    void getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList> &retVal)
    {
        std::vector<XmlNodeImpl *> &childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i ++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE)
            {
                XmlElement *pEl = (XmlElement *)(childs[i]->m_node);
                pEl->getElementsByTagNameFromThis(tagName, retVal);
            }
    }

private:
    std::string m_tagName;
    obj_ptr<XmlNamedNodeMap> m_attrs;
};

} /* namespace fibjs */
#endif /* XMLELEMENT_H_ */
