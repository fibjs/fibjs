/*
 * XmlElement.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlElement.h"
#include "XmlAttr.h"
#include "XmlText.h"
#include "XmlDocument.h"
#include "StringBuffer.h"

namespace fibjs {

result_t XmlElement::get_nodeName(exlib::string& retVal)
{
    retVal = m_tagName;
    return 0;
}

result_t XmlElement::get_nodeValue(exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::set_nodeValue(exlib::string newVal)
{
    return 0;
}

result_t XmlElement::get_nodeType(int32_t& retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlElement::get_namespaceURI(exlib::string& retVal)
{
    if (m_namespaceURI.empty())
        return CALL_RETURN_NULL;

    retVal = m_namespaceURI;
    return 0;
}

result_t XmlElement::get_prefix(exlib::string& retVal)
{
    if (m_prefix.empty())
        return CALL_RETURN_NULL;

    retVal = m_prefix;
    return 0;
}

result_t XmlElement::set_prefix(exlib::string newVal)
{
    m_prefix = newVal;
    return 0;
}

result_t XmlElement::get_localName(exlib::string& retVal)
{
    retVal = m_localName;
    return 0;
}

result_t XmlElement::get_parentNode(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlElement::get_childNodes(obj_ptr<XmlNodeList_base>& retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlElement::get_firstChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlElement::get_lastChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlElement::get_previousSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlElement::get_nextSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlElement::get_ownerDocument(obj_ptr<XmlDocument_base>& retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlElement::lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
{
    if (globalPrefix(namespaceURI, retVal))
        return 0;

    result_t hr = m_attrs->lookupPrefix(namespaceURI, retVal);
    if (hr < 0)
        return hr;
    if (hr != CALL_RETURN_NULL)
        return retVal.empty() ? CALL_RETURN_NULL : 0;

    return XmlNodeImpl::lookupPrefix(namespaceURI, retVal);
}

result_t XmlElement::lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
{
    if (globalNamespaceURI(prefix, retVal))
        return 0;

    result_t hr = m_attrs->lookupNamespaceURI(prefix, retVal);
    if (hr < 0)
        return hr;
    if (hr != CALL_RETURN_NULL)
        return retVal.empty() ? CALL_RETURN_NULL : 0;

    return XmlNodeImpl::lookupNamespaceURI(prefix, retVal);
}

result_t XmlElement::insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlElement::insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlElement::replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlElement::removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlElement::appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlElement::hasChildNodes(bool& retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlElement::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlElement> el = new XmlElement(*this);
    result_t hr = m_attrs->cloneAttrs(el->m_attrs, el);
    if (hr < 0)
        return hr;

    return XmlNodeImpl::cloneNode(el, deep, retVal);
}

result_t XmlElement::normalize()
{
    return m_childs->normalize();
}

result_t XmlElement::get_tagName(exlib::string& retVal)
{
    retVal = m_tagName;
    return 0;
}

result_t XmlElement::get_id(exlib::string& retVal)
{
    getAttribute("id", retVal);
    return 0;
}

result_t XmlElement::set_id(exlib::string newVal)
{
    if (newVal.empty())
        return removeAttribute("id");

    return setAttribute("id", newVal);
}

result_t XmlElement::get_textContent(exlib::string& retVal)
{
    StringBuffer strs;
    getTextContent(strs);
    retVal = strs.str();

    return 0;
}

result_t XmlElement::set_textContent(exlib::string newVal)
{
    m_childs->removeAll();

    obj_ptr<XmlText> text = new XmlText(m_document, newVal);

    obj_ptr<XmlNode_base> out;
    appendChild(text, out);

    return 0;
}

result_t XmlElement::get_innerHTML(exlib::string& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    if (m_childs->hasChildNodes())
        m_childs->toString(retVal);

    return 0;
}

result_t XmlElement::set_innerHTML(exlib::string newVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    result_t hr;

    m_childs->removeAll();

    obj_ptr<XmlDocument> doc = new XmlDocument(false);
    hr = doc->load(newVal);
    if (hr < 0)
        return hr;

    obj_ptr<XmlElement_base> body;
    hr = doc->get_body(body);
    if (hr != 0)
        return hr;

    obj_ptr<XmlNode_base> node;
    obj_ptr<XmlNode_base> out;
    while (body->get_firstChild(node) == 0)
        appendChild(node, out);

    return 0;
}

result_t XmlElement::get_className(exlib::string& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    getAttribute("class", retVal);
    return 0;
}

result_t XmlElement::set_className(exlib::string newVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    if (newVal.empty())
        return removeAttribute("class");

    return setAttribute("class", newVal);
}

result_t XmlElement::get_attributes(obj_ptr<XmlNamedNodeMap_base>& retVal)
{
    retVal = m_attrs;
    return 0;
}

result_t XmlElement::getAttribute(exlib::string name, exlib::string& retVal)
{
    result_t hr;
    obj_ptr<XmlAttr_base> node;

    hr = m_attrs->getNamedItem(name, node);
    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return node->get_value(retVal);
}

result_t XmlElement::getAttributeNS(exlib::string namespaceURI, exlib::string localName,
    exlib::string& retVal)
{
    result_t hr;
    obj_ptr<XmlAttr_base> node;

    hr = m_attrs->getNamedItemNS(namespaceURI, localName, node);
    if (hr < 0 || hr == CALL_RETURN_NULL)
        return hr;

    return node->get_value(retVal);
}

result_t XmlElement::setAttribute(exlib::string name, exlib::string value)
{
    obj_ptr<XmlAttr> attr = new XmlAttr(this, name, value);
    return m_attrs->setNamedItem(attr);
}

result_t XmlElement::setAttributeNS(exlib::string namespaceURI, exlib::string qualifiedName,
    exlib::string value)
{
    const char* c_str = qualifiedName.c_str();
    if (!qstrcmp(c_str, "xmlns:", 6)) {
        int32_t i;

        for (i = 0; s_nss[i][0]; i++)
            if (!qstrcmp(c_str + 6, s_nss[i][0]))
                return 0;
    }

    obj_ptr<XmlAttr> attr = new XmlAttr(this, namespaceURI, qualifiedName, value);
    return m_attrs->setNamedItem(attr);
}

result_t XmlElement::removeAttribute(exlib::string name)
{
    return m_attrs->removeNamedItem(name);
}

result_t XmlElement::removeAttributeNS(exlib::string namespaceURI, exlib::string localName)
{
    return m_attrs->removeNamedItemNS(namespaceURI, localName);
}

result_t XmlElement::getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    getElementsByTagName(tagName, ret);

    retVal = ret;
    return 0;
}

result_t XmlElement::getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName,
    obj_ptr<XmlNodeList_base>& retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    getElementsByTagNameNS(namespaceURI, localName, ret);

    retVal = ret;
    return 0;
}

result_t XmlElement::hasAttribute(exlib::string name, bool& retVal)
{
    result_t hr;
    obj_ptr<XmlAttr_base> node;

    hr = m_attrs->getNamedItem(name, node);
    if (hr < 0)
        return hr;

    retVal = hr != CALL_RETURN_NULL;
    return 0;
}

result_t XmlElement::hasAttributeNS(exlib::string namespaceURI, exlib::string localName, bool& retVal)
{
    result_t hr;
    obj_ptr<XmlAttr_base> node;

    hr = m_attrs->getNamedItemNS(namespaceURI, localName, node);
    if (hr < 0)
        return hr;

    retVal = hr != CALL_RETURN_NULL;
    return 0;
}

void XmlElement::fix_prefix(exlib::string namespaceURI, exlib::string& prefix)
{
    exlib::string _namespaceURI;
    int32_t i;

    if (!prefix.empty() && lookupNamespaceURI(prefix, _namespaceURI) == CALL_RETURN_NULL)
        setAttributeNS("http://www.w3.org/2000/xmlns/", ("xmlns:" + prefix), namespaceURI);
    else if (_namespaceURI != namespaceURI) {
        char buf[64];

        for (i = 0; i < 65536; i++) {
            sprintf(buf, "a%d", i);
            if (lookupNamespaceURI(buf, _namespaceURI) == CALL_RETURN_NULL) {
                prefix = buf;
                setAttributeNS("http://www.w3.org/2000/xmlns/", ("xmlns:" + prefix), namespaceURI);
                return;
            }
        }
    }
}

result_t XmlElement::toString(exlib::string& retVal)
{
    retVal = "<";

    exlib::string tagName(m_tagName);

    if (!m_isXml)
        qstrlwr(&tagName[0]);

    if (m_prefix.empty()) {
        if (!m_namespaceURI.empty()) {
            bool skip_def_ns = false;

            if (m_parent) {
                int32_t type;

                m_parent->get_nodeType(type);
                if (type == xml_base::_ELEMENT_NODE) {
                    exlib::string def_ns;
                    ((XmlElement*)m_parent->m_node)->get_defaultNamespace(def_ns);

                    if (def_ns == m_namespaceURI)
                        skip_def_ns = true;
                }
            }

            if (!skip_def_ns)
                setAttribute("xmlns", m_namespaceURI);
        }
        retVal.append(tagName);
    } else {
        fix_prefix(m_namespaceURI, m_prefix);

        retVal.append(m_prefix);
        retVal += ':';
        retVal.append(m_localName);
    }

    exlib::string strAttr;
    m_attrs->toString(strAttr);
    retVal.append(strAttr);

    if (m_childs->hasChildNodes()) {
        exlib::string strChild;
        m_childs->toString(strChild);

        retVal += '>';
        retVal.append(strChild);
        retVal.append("</");
        retVal.append(tagName);
        retVal += '>';
    } else
        retVal.append("/>");

    return 0;
}
}
