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
#include "StringBuffer.h"
#include "parse.h"

#ifndef XMLELEMENT_H_
#define XMLELEMENT_H_

namespace fibjs {

class XmlElement : public XmlElement_base, public XmlNodeImpl {
public:
    XmlElement(XmlDocument_base* document, exlib::string tagName, bool isXml)
        : XmlNodeImpl(document, this, xml_base::_ELEMENT_NODE)
        , m_isXml(isXml)
        , m_tagName(tagName)
        , m_localName(tagName)
        , m_attrs(new XmlNamedNodeMap())
    {
        if (!m_isXml)
            qstrupr(&m_tagName[0]);
    }

    XmlElement(XmlDocument_base* document, exlib::string namespaceURI, exlib::string qualifiedName, bool isXml)
        : XmlNodeImpl(document, this, xml_base::_ELEMENT_NODE)
        , m_isXml(isXml)
        , m_tagName(qualifiedName)
        , m_namespaceURI(namespaceURI)
        , m_attrs(new XmlNamedNodeMap())
    {
        const char* c_str = qualifiedName.c_str();
        const char* p = qstrchr(c_str, ':');
        if (!p)
            m_localName = m_tagName;
        else {
            m_prefix.assign(c_str, p - c_str);
            m_localName.assign(p + 1);
        }

        if (!m_isXml)
            qstrupr(&m_tagName[0]);
    }

    XmlElement(const XmlElement& from)
        : XmlNodeImpl(from.m_document, this, xml_base::_ELEMENT_NODE)
        , m_isXml(from.m_isXml)
        , m_tagName(from.m_tagName)
        , m_localName(from.m_localName)
        , m_prefix(from.m_prefix)
        , m_namespaceURI(from.m_namespaceURI)
        , m_attrs(new XmlNamedNodeMap())
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeType(int32_t& retVal);
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base>& retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base>& retVal);
    virtual result_t hasChildNodes(bool& retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t normalize();
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);
    virtual result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);

public:
    // XmlElement_base
    virtual result_t get_namespaceURI(exlib::string& retVal);
    virtual result_t get_prefix(exlib::string& retVal);
    virtual result_t set_prefix(exlib::string newVal);
    virtual result_t get_localName(exlib::string& retVal);
    virtual result_t get_tagName(exlib::string& retVal);
    virtual result_t get_id(exlib::string& retVal);
    virtual result_t set_id(exlib::string newVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t get_innerHTML(exlib::string& retVal);
    virtual result_t set_innerHTML(exlib::string newVal);
    virtual result_t get_className(exlib::string& retVal);
    virtual result_t set_className(exlib::string newVal);
    virtual result_t get_attributes(obj_ptr<XmlNamedNodeMap_base>& retVal);
    virtual result_t getAttribute(exlib::string name, exlib::string& retVal);
    virtual result_t getAttributeNS(exlib::string namespaceURI, exlib::string localName, exlib::string& retVal);
    virtual result_t setAttribute(exlib::string name, exlib::string value);
    virtual result_t setAttributeNS(exlib::string namespaceURI, exlib::string qualifiedName, exlib::string value);
    virtual result_t removeAttribute(exlib::string name);
    virtual result_t removeAttributeNS(exlib::string namespaceURI, exlib::string localName);
    virtual result_t hasAttribute(exlib::string name, bool& retVal);
    virtual result_t hasAttributeNS(exlib::string namespaceURI, exlib::string localName, bool& retVal);
    virtual result_t getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementById(exlib::string id, obj_ptr<XmlElement_base>& retVal);
    virtual result_t getElementsByClassName(exlib::string className, obj_ptr<XmlNodeList_base>& retVal);

public:
    result_t get_defaultNamespace(exlib::string& def_ns)
    {
        if (m_prefix.empty()) {
            def_ns = m_namespaceURI;
            return 0;
        }

        result_t hr = getAttribute("xmlns", def_ns);
        if (hr == CALL_RETURN_UNDEFINED && m_parent) {
            int32_t type;

            m_parent->get_nodeType(type);
            if (type == xml_base::_ELEMENT_NODE)
                return ((XmlElement*)m_parent->m_node)->get_defaultNamespace(def_ns);
        }

        return 0;
    }

    void getElementsByTagNameFromThis(exlib::string tagName, obj_ptr<XmlNodeList>& retVal)
    {
        if (*tagName.c_str() == '*' || (m_isXml ? (m_tagName == tagName) : !qstricmp(m_tagName.c_str(), tagName.c_str()))) {
            retVal->appendChild(this);
            Ref();
        }

        getElementsByTagName(tagName, retVal);
    }

    void getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList>& retVal)
    {
        QuickArray<XmlNodeImpl*>& childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE) {
                XmlElement* pEl = (XmlElement*)(childs[i]->m_node);
                pEl->getElementsByTagNameFromThis(tagName, retVal);
            }
    }

    result_t getFirstElementsByTagName(exlib::string tagName, obj_ptr<XmlElement_base>& retVal)
    {
        if (m_isXml ? (m_tagName == tagName) : !qstricmp(m_tagName.c_str(), tagName.c_str())) {
            retVal = this;
            return 0;
        }

        QuickArray<XmlNodeImpl*>& childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE) {
                XmlElement* pEl = (XmlElement*)(childs[i]->m_node);
                if (pEl->getFirstElementsByTagName(tagName, retVal) == 0)
                    return 0;
            }

        return CALL_RETURN_UNDEFINED;
    }

    void getTextContent(StringBuffer& retVal)
    {
        QuickArray<XmlNodeImpl*>& childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE) {
                XmlElement* pEl = (XmlElement*)(childs[i]->m_node);
                pEl->getTextContent(retVal);
            } else if (childs[i]->m_type == xml_base::_TEXT_NODE) {
                exlib::string value;
                childs[i]->m_node->get_nodeValue(value);
                retVal.append(value);
            }
    }

    void getElementsByTagNameNSFromThis(exlib::string namespaceURI, exlib::string localName,
        obj_ptr<XmlNodeList>& retVal)
    {
        if ((*namespaceURI.c_str() == '*' || (m_namespaceURI == namespaceURI)) && (*localName.c_str() == '*' || (m_localName == localName))) {
            retVal->appendChild(this);
            Ref();
        }
        getElementsByTagNameNS(namespaceURI, localName, retVal);
    }

    void getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName,
        obj_ptr<XmlNodeList>& retVal)
    {
        QuickArray<XmlNodeImpl*>& childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE) {
                XmlElement* pEl = (XmlElement*)(childs[i]->m_node);
                pEl->getElementsByTagNameNSFromThis(namespaceURI, localName, retVal);
            }
    }

    result_t getElementByIdFromThis(exlib::string id, obj_ptr<XmlElement_base>& retVal)
    {
        exlib::string _id;
        get_id(_id);

        if (_id == id) {
            retVal = this;
            return 0;
        }

        return getElementById(id, retVal);
    }

    void getElementsByClassNameFromThis(QuickArray<exlib::string>& classNames, obj_ptr<XmlNodeList>& retVal)
    {
        exlib::string _class;
        get_className(_class);

        if (!_class.empty()) {
            QuickArray<exlib::string> _classNames;
            _parser p(_class);
            exlib::string str;

            p.skipSpace();
            while (p.getWord(str)) {
                _classNames.append(str);
                p.skipSpace();
            }

            int32_t i, j, cnt = 0;

            for (i = 0; i < (int32_t)classNames.size(); i++) {
                for (j = 0; j < (int32_t)_classNames.size(); j++) {
                    if (_classNames[j] == classNames[i]) {
                        cnt++;
                        break;
                    }
                }
            }

            if (cnt == (int32_t)classNames.size()) {
                retVal->appendChild(this);
                Ref();
            }
        }

        getElementsByClassName(classNames, retVal);
    }

    void getElementsByClassName(QuickArray<exlib::string>& classNames, obj_ptr<XmlNodeList>& retVal)
    {
        QuickArray<XmlNodeImpl*>& childs = m_childs->m_childs;
        int32_t sz = (int32_t)childs.size();
        int32_t i;

        for (i = 0; i < sz; i++)
            if (childs[i]->m_type == xml_base::_ELEMENT_NODE) {
                XmlElement* pEl = (XmlElement*)(childs[i]->m_node);
                pEl->getElementsByClassNameFromThis(classNames, retVal);
            }
    }

    void fix_prefix(exlib::string namespaceURI, exlib::string& prefix);

private:
    bool m_isXml;
    exlib::string m_tagName;
    exlib::string m_localName;
    exlib::string m_prefix;
    exlib::string m_namespaceURI;
    obj_ptr<XmlNamedNodeMap> m_attrs;
};

} /* namespace fibjs */
#endif /* XMLELEMENT_H_ */
