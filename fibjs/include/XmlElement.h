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
    XmlElement(XmlDocument_base *document, const char *tagName, bool isXml):
        XmlNodeImpl(document, this, xml_base::_ELEMENT_NODE), m_isXml(isXml),
        m_tagName(tagName), m_localName(tagName), m_attrs(new XmlNamedNodeMap())
    {
        if (!m_isXml)
            qstrupr(&m_tagName[0]);
    }

    XmlElement(XmlDocument_base *document, const char *namespaceURI, const char *qualifiedName, bool isXml):
        XmlNodeImpl(document, this, xml_base::_ELEMENT_NODE), m_isXml(isXml),
        m_tagName(qualifiedName), m_namespaceURI(namespaceURI), m_attrs(new XmlNamedNodeMap())
    {
        const char *p = qstrchr(qualifiedName, ':');
        if (!p)
            m_localName = m_tagName;
        else
        {
            m_prefix.assign(qualifiedName, p - qualifiedName);
            m_localName.assign(p + 1);
        }

        if (!m_isXml)
            qstrupr(&m_tagName[0]);
    }

    XmlElement(const XmlElement &from):
        XmlNodeImpl(from.m_document, this, xml_base::_ELEMENT_NODE),
        m_isXml(from.m_isXml), m_tagName(from.m_tagName), m_localName(from.m_localName),
        m_prefix(from.m_prefix), m_namespaceURI(from.m_namespaceURI),
        m_attrs(new XmlNamedNodeMap())
    {
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeType(int32_t &retVal);
    virtual result_t get_nodeName(std::string &retVal);
    virtual result_t get_nodeValue(std::string &retVal);
    virtual result_t set_nodeValue(const char *newVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base> &retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base> &retVal);
    virtual result_t hasChildNodes(bool &retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t normalize();
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal);
    virtual result_t lookupPrefix(const char *namespaceURI, std::string &retVal);
    virtual result_t lookupNamespaceURI(const char *prefix, std::string &retVal);
    virtual result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t insertAfter(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);

public:
    // XmlElement_base
    virtual result_t get_namespaceURI(std::string &retVal);
    virtual result_t get_prefix(std::string &retVal);
    virtual result_t set_prefix(const char *newVal);
    virtual result_t get_localName(std::string &retVal);
    virtual result_t get_tagName(std::string &retVal);
    virtual result_t get_id(std::string &retVal);
    virtual result_t set_id(const char *newVal);
    virtual result_t get_textContent(std::string &retVal);
    virtual result_t set_textContent(const char *newVal);
    virtual result_t get_innerHTML(std::string &retVal);
    virtual result_t set_innerHTML(const char *newVal);
    virtual result_t get_className(std::string &retVal);
    virtual result_t set_className(const char *newVal);
    virtual result_t get_attributes(obj_ptr<XmlNamedNodeMap_base> &retVal);
    virtual result_t getAttribute(const char *name, std::string &retVal);
    virtual result_t getAttributeNS(const char *namespaceURI, const char *localName, std::string &retVal);
    virtual result_t setAttribute(const char *name, const char *value);
    virtual result_t setAttributeNS(const char *namespaceURI, const char *qualifiedName, const char *value);
    virtual result_t removeAttribute(const char *name);
    virtual result_t removeAttributeNS(const char *namespaceURI, const char *localName);
    virtual result_t hasAttribute(const char *name, bool &retVal);
    virtual result_t hasAttributeNS(const char *namespaceURI, const char *localName, bool &retVal);
    virtual result_t getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t getElementsByTagNameNS(const char *namespaceURI, const char *localName, obj_ptr<XmlNodeList_base> &retVal);

public:
    result_t get_defaultNamespace(std::string &def_ns)
    {
        if (m_prefix.empty())
        {
            def_ns = m_namespaceURI;
            return 0;
        }

        result_t hr = getAttribute("xmlns", def_ns);
        if (hr == CALL_RETURN_NULL && m_parent)
        {
            int32_t type;

            m_parent->get_nodeType(type);
            if (type == xml_base::_ELEMENT_NODE)
                return ((XmlElement *)m_parent->m_node)->get_defaultNamespace(def_ns);
        }

        return 0;
    }

    void getElementsByTagNameFromThis(const char *tagName, obj_ptr<XmlNodeList> &retVal)
    {
        if (*tagName == '*' ||
                ( m_isXml ? !qstrcmp(m_tagName.c_str(), tagName) :
                  !qstricmp(m_tagName.c_str(), tagName)))
        {
            retVal->appendChild(this);
            Ref();
        }

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

    result_t getFirstElementsByTagName(const char *tagName, obj_ptr<XmlElement_base> &retVal)
    {
        if (m_isXml ? !qstrcmp(m_tagName.c_str(), tagName) :
                !qstricmp(m_tagName.c_str(), tagName))
        {
            retVal = this;
            return 0;
        }

        std::vector<XmlNodeImpl *> &childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i ++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE)
            {
                XmlElement *pEl = (XmlElement *)(childs[i]->m_node);
                if (pEl->getFirstElementsByTagName(tagName, retVal) == 0)
                    return 0;
            }

        return CALL_RETURN_NULL;
    }

    void getTextContent(std::vector<std::string> &retVal)
    {
        std::vector<XmlNodeImpl *> &childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i ++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE)
            {
                XmlElement *pEl = (XmlElement *)(childs[i]->m_node);
                pEl->getTextContent(retVal);
            }
            else if (childs[i]->m_type == xml_base::_TEXT_NODE)
            {
                std::string value;
                childs[i]->m_node->get_nodeValue(value);
                retVal.push_back(value);
            }
    }

    void getElementsByTagNameNSFromThis(const char *namespaceURI, const char *localName,
                                        obj_ptr<XmlNodeList> &retVal)
    {
        if ((*namespaceURI == '*' || !qstrcmp(m_namespaceURI.c_str(), namespaceURI)) &&
                (*localName == '*' || !qstrcmp(m_localName.c_str(), localName)))
        {
            retVal->appendChild(this);
            Ref();
        }
        getElementsByTagNameNS(namespaceURI, localName, retVal);
    }

    void getElementsByTagNameNS(const char *namespaceURI, const char *localName,
                                obj_ptr<XmlNodeList> &retVal)
    {
        std::vector<XmlNodeImpl *> &childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i ++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE)
            {
                XmlElement *pEl = (XmlElement *)(childs[i]->m_node);
                pEl->getElementsByTagNameNSFromThis(namespaceURI, localName, retVal);
            }
    }

    void fix_prefix(const char *namespaceURI, std::string &prefix);

private:
    bool m_isXml;
    std::string m_tagName;
    std::string m_localName;
    std::string m_prefix;
    std::string m_namespaceURI;
    obj_ptr<XmlNamedNodeMap> m_attrs;
};

} /* namespace fibjs */
#endif /* XMLELEMENT_H_ */
