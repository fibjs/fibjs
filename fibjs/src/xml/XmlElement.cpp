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
#include "XmlDocumentFragment.h"
#include "StringBuffer.h"
#include "parse.h"
#include <algorithm>

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

result_t XmlElement::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlElement> el = new XmlElement(*this);
    result_t hr = m_attrs->cloneAttrs(el->m_attrs, el);
    if (hr < 0)
        return hr;

    return XmlNodeImpl::cloneNode(el, deep, retVal);
}

result_t XmlElement::get_tagName(exlib::string& retVal)
{
    retVal = m_tagName;
    // HTML tag names should be uppercase
    if (!m_isXml) {
        exlib::qstrupr(retVal);
    }
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

// HTML attribute reflection for common element attributes: reading returns the
// attribute value (or empty string), writing syncs to the attribute (empty
// string removes it), mirroring browser behavior for e.g. img.src / input.value.
#define IMPL_ATTR_REFLECT(name)                           \
    result_t XmlElement::get_##name(exlib::string& retVal) \
    {                                                      \
        getAttribute(#name, retVal);                       \
        return 0;                                          \
    }                                                      \
    result_t XmlElement::set_##name(exlib::string newVal)   \
    {                                                      \
        if (newVal.empty())                                \
            return removeAttribute(#name);                 \
        return setAttribute(#name, newVal);                \
    }

IMPL_ATTR_REFLECT(src)
IMPL_ATTR_REFLECT(alt)
IMPL_ATTR_REFLECT(href)
IMPL_ATTR_REFLECT(title)
IMPL_ATTR_REFLECT(value)
IMPL_ATTR_REFLECT(name)
IMPL_ATTR_REFLECT(type)
IMPL_ATTR_REFLECT(rel)
IMPL_ATTR_REFLECT(target)
IMPL_ATTR_REFLECT(placeholder)

#undef IMPL_ATTR_REFLECT

result_t XmlElement::get_innerHTML(exlib::string& retVal)
{
    // For template elements, innerHTML reflects the content's children
    if (!m_isXml && qstricmp(m_tagName.c_str(), "TEMPLATE") == 0 && m_content) {
        obj_ptr<XmlNodeList_base> childNodes;
        m_content->get_childNodes(childNodes);
        bool hasChildren;
        m_content->hasChildNodes(hasChildren);
        if (hasChildren)
            ((XmlNodeList*)(XmlNodeList_base*)childNodes)->toString(retVal);
        return 0;
    }

    if (m_childs->hasChildNodes())
        m_childs->toString(retVal);

    return 0;
}

result_t XmlElement::set_innerHTML(exlib::string newVal)
{
    result_t hr;

    m_childs->removeAll();

    if (newVal.empty())
        return 0;

    if (m_isXml) {
        // For XML documents, wrap content in a root element for parsing
        exlib::string wrappedContent = "<root>" + newVal + "</root>";
        obj_ptr<XmlDocument> doc = new XmlDocument(true);
        hr = doc->load(wrappedContent);
        if (hr < 0)
            return hr;

        // Move children from document element's root
        obj_ptr<XmlElement_base> docElement;
        hr = doc->get_documentElement(docElement);
        if (hr == 0 && docElement) {
            obj_ptr<XmlNode_base> node;
            obj_ptr<XmlNode_base> out;
            while (docElement->get_firstChild(node) == 0) {
                appendChild(node, out);
            }
        }
    } else {
        // For HTML documents, parse as HTML
        obj_ptr<XmlDocument> doc = new XmlDocument(false);
        hr = doc->load(newVal);
        if (hr < 0)
            return hr;

        // Move children from body element
        obj_ptr<XmlElement_base> body;
        hr = doc->get_body(body);
        if (hr == 0 && body) {
            obj_ptr<XmlNode_base> node;
            obj_ptr<XmlNode_base> out;
            while (body->get_firstChild(node) == 0) {
                appendChild(node, out);
            }
        }
    }

    return 0;
}

result_t XmlElement::get_outerHTML(exlib::string& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    return toString(retVal);
}

result_t XmlElement::set_outerHTML(exlib::string newVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    // WHATWG HTML 8.5.5: if the element has no parent, the setter silently
    // does nothing; only a Document parent throws NoModificationAllowedError.
    obj_ptr<XmlNode_base> parent;
    result_t hr = get_parentNode(parent);
    if (hr < 0 || parent == NULL)
        return 0;

    int32_t parentType;
    parent->get_nodeType(parentType);
    if (parentType == xml_base::C_DOCUMENT_NODE)
        return CHECK_ERROR(Runtime::setError("XmlElement: The element is a direct child of a document."));

    // Parse the new HTML content
    obj_ptr<XmlDocument> doc = new XmlDocument(false);
    hr = doc->load(newVal);
    if (hr < 0)
        return hr;

    // Get the body element which contains the parsed nodes
    obj_ptr<XmlElement_base> body;
    hr = doc->get_body(body);
    if (hr < 0 || body == NULL)
        return hr;

    // Insert all new nodes before this element, then remove this element
    obj_ptr<XmlNode_base> node;
    obj_ptr<XmlNode_base> out;
    while (body->get_firstChild(node) == 0) {
        parent->insertBefore(node, this, out);
    }

    // Remove this element from parent
    parent->removeChild(this, out);

    return 0;
}

result_t XmlElement::get_className(exlib::string& retVal)
{
    getAttribute("class", retVal);
    return 0;
}

result_t XmlElement::set_className(exlib::string newVal)
{
    if (newVal.empty())
        return removeAttribute("class");

    return setAttribute("class", newVal);
}

result_t XmlElement::get_classList(obj_ptr<DOMTokenList_base>& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    if (!m_classList)
        m_classList = new DOMTokenList(this);

    retVal = m_classList;
    return 0;
}

result_t XmlElement::get_style(obj_ptr<CSSStyleDeclaration_base>& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    if (!m_style)
        m_style = new CSSStyleDeclaration(this);

    retVal = m_style;
    return 0;
}

result_t XmlElement::get_dataset(obj_ptr<DOMStringMap_base>& retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    if (!m_dataset)
        m_dataset = new DOMStringMap(this);

    retVal = m_dataset;
    return 0;
}

result_t XmlElement::get_content(obj_ptr<XmlDocumentFragment_base>& retVal)
{
    // content property is only valid for <template> elements in HTML mode
    if (m_isXml)
        return CALL_RETURN_NULL;

    // Check if this is a template element (case-insensitive for HTML)
    if (qstricmp(m_tagName.c_str(), "TEMPLATE") != 0)
        return CALL_RETURN_NULL;

    // Return cached content if available
    if (m_content) {
        retVal = m_content;
        return 0;
    }

    // Create a persistent DocumentFragment for template content
    // According to DOM spec, template.content returns a DocumentFragment
    // that contains the template's actual children (not clones)
    obj_ptr<XmlDocumentFragment> fragment = new XmlDocumentFragment(m_document);

    // Move all children from template to the content fragment
    while (m_childs->m_childs.size() > 0) {
        XmlNodeImpl* child = m_childs->m_childs[0];
        obj_ptr<XmlNode_base> childRef = child->m_node;
        obj_ptr<XmlNode_base> tmp;
        m_childs->removeChild(child->m_node, tmp);
        fragment->appendChild(childRef, tmp);
    }

    m_content = fragment;
    retVal = fragment;
    return 0;
}

result_t XmlElement::get_attributes(obj_ptr<XmlNamedNodeMap_base>& retVal)
{
    retVal = m_attrs;
    return 0;
}

result_t XmlElement::hasAttributes(bool& retVal)
{
    int32_t len;
    m_attrs->get_length(len);
    retVal = len > 0;
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

result_t XmlElement::getAttributeNode(exlib::string name, obj_ptr<XmlAttr_base>& retVal)
{
    return m_attrs->getNamedItem(name, retVal);
}

result_t XmlElement::getAttributeNodeNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlAttr_base>& retVal)
{
    return m_attrs->getNamedItemNS(namespaceURI, localName, retVal);
}

result_t XmlElement::setAttribute(exlib::string name, exlib::string value)
{
    // In HTML mode, normalize attribute names to lowercase
    if (!m_isXml)
        exlib::qstrlwr(name);

    obj_ptr<XmlAttr> attr = new XmlAttr(this, name, value);
    obj_ptr<XmlAttr_base> retVal;
    return m_attrs->setNamedItem(attr, retVal);
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
    obj_ptr<XmlAttr_base> retVal;
    return m_attrs->setNamedItem(attr, retVal);
}

result_t XmlElement::setAttributeNode(XmlAttr_base* attr, obj_ptr<XmlAttr_base>& retVal)
{
    XmlAttr* _attr = (XmlAttr*)attr;
    if (_attr->m_owner != NULL && _attr->m_owner != this) {
        return Runtime::setError("The attribute already belongs to another element");
    }

    if (_attr->m_owner == this) {
        retVal = _attr;
        return 0;
    }

    return m_attrs->setNamedItem((XmlAttr*)attr, retVal);
}

result_t XmlElement::removeAttribute(exlib::string name)
{
    return m_attrs->removeNamedItem(name);
}

result_t XmlElement::removeAttributeNS(exlib::string namespaceURI, exlib::string localName)
{
    return m_attrs->removeNamedItemNS(namespaceURI, localName);
}

result_t XmlElement::removeAttributeNode(XmlAttr_base* attr, obj_ptr<XmlAttr_base>& retVal)
{
    retVal = attr;
    return m_attrs->removeNode(attr);
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

result_t XmlElement::getElementById(exlib::string id, obj_ptr<XmlElement_base>& retVal)
{
    if (id.empty())
        return CHECK_ERROR(CALL_RETURN_NULL);

    std::vector<XmlNodeImpl*>& childs = m_childs->m_childs;
    int32_t sz = (int32_t)childs.size();
    int32_t i;
    result_t hr;

    for (i = 0; i < sz; i++)
        if (childs[i]->m_type == xml_base::C_ELEMENT_NODE) {
            XmlElement* pEl = (XmlElement*)(childs[i]->m_node);
            hr = pEl->getElementByIdFromThis(id, retVal);
            if (hr != CALL_RETURN_NULL)
                return hr;
        }

    return CHECK_ERROR(CALL_RETURN_NULL);
}

result_t XmlElement::getElementsByClassName(exlib::string className, obj_ptr<XmlNodeList_base>& retVal)
{
    std::vector<exlib::string> classNames;
    _parser p(className);
    exlib::string str;

    p.skipSpace();
    while (p.getWord(str)) {
        classNames.push_back(str);
        p.skipSpace();
    }

    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);

    if (classNames.size() > 0)
        getElementsByClassName(classNames, ret);

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
            snprintf(buf, sizeof(buf), "a%d", i);
            if (lookupNamespaceURI(buf, _namespaceURI) == CALL_RETURN_NULL) {
                prefix = buf;
                setAttributeNS("http://www.w3.org/2000/xmlns/", ("xmlns:" + prefix), namespaceURI);
                return;
            }
        }
    }
}

static bool is_self_closing_tag(const char* tagName)
{
    static const char* self_closing_tags[] = {
        "area",
        "base",
        "br",
        "col",
        "command",
        "embed",
        "hr",
        "img",
        "input",
        "keygen",
        "link",
        "meta",
        "param",
        "source",
        "track",
        "wbr",
        NULL
    };

    for (int32_t i = 0; self_closing_tags[i]; i++)
        if (!qstrcmp(tagName, self_closing_tags[i]))
            return true;

    return false;
}

result_t XmlElement::toString(exlib::string& retVal)
{
    retVal = "<";

    exlib::string tagName(m_tagName);

    if (!m_isXml)
        exlib::qstrlwr(tagName);

    if (m_prefix.empty()) {
        if (!m_namespaceURI.empty()) {
            bool skip_def_ns = false;

            if (m_parent) {
                int32_t type;

                m_parent->get_nodeType(type);
                if (type == xml_base::C_ELEMENT_NODE) {
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
    } else if (!m_isXml) {
        retVal += '>';
        if (!is_self_closing_tag(tagName.c_str())) {
            retVal.append("</");
            retVal.append(tagName);
            retVal += '>';
        }
    } else
        retVal.append("/>");

    return 0;
}

result_t XmlElement::toXmlString(exlib::string& retVal)
{
    retVal = "<";

    exlib::string tagName(m_tagName);

    if (!m_isXml)
        exlib::qstrlwr(tagName);

    if (m_prefix.empty()) {
        if (!m_namespaceURI.empty()) {
            bool skip_def_ns = false;

            if (m_parent) {
                int32_t type;

                m_parent->get_nodeType(type);
                if (type == xml_base::C_ELEMENT_NODE) {
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
        m_childs->toXmlString(strChild);

        retVal += '>';
        retVal.append(strChild);
        retVal.append("</");
        retVal.append(tagName);
        retVal += '>';
    } else {
        if (m_isXml)
            retVal.append("/>");
        else
            retVal.append(" />");
    }

    return 0;
}

result_t XmlElement::append(OptArgs nodes)
{
    int32_t len = nodes.Length();
    for (int32_t i = 0; i < len; i++) {
        Variant v = nodes[i];
        obj_ptr<XmlNode_base> node;
        if (v.type() == Variant::VT_String) {
            obj_ptr<XmlText_base> textNode;
            m_document->createTextNode(v.string(), textNode);
            node = textNode;
        } else {
            node = XmlNode_base::getInstance(v.object());
        }
        if (node) {
            obj_ptr<XmlNode_base> retVal;
            result_t hr = m_childs->appendChild(node, retVal);
            if (hr < 0)
                return hr;
        }
    }
    return 0;
}

result_t XmlElement::prepend(OptArgs nodes)
{
    obj_ptr<XmlNode_base> firstChild;
    m_childs->firstChild(firstChild);

    int32_t len = nodes.Length();
    for (int32_t i = 0; i < len; i++) {
        Variant v = nodes[i];
        obj_ptr<XmlNode_base> node;
        if (v.type() == Variant::VT_String) {
            obj_ptr<XmlText_base> textNode;
            m_document->createTextNode(v.string(), textNode);
            node = textNode;
        } else {
            node = XmlNode_base::getInstance(v.object());
        }
        if (node) {
            obj_ptr<XmlNode_base> retVal;
            result_t hr;
            if (firstChild) {
                hr = m_childs->insertBefore(node, firstChild, retVal);
            } else {
                hr = m_childs->appendChild(node, retVal);
            }
            if (hr < 0)
                return hr;
        }
    }
    return 0;
}

result_t XmlElement::replaceChildren(OptArgs nodes)
{
    // Remove all existing children
    m_childs->removeAll();

    // Append new nodes
    return append(nodes);
}

// Helper function to parse position string
static int32_t parsePosition(exlib::string position)
{
    // Convert to lowercase for case-insensitive comparison
    exlib::string lower = position;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "beforebegin")
        return 0;
    else if (lower == "afterbegin")
        return 1;
    else if (lower == "beforeend")
        return 2;
    else if (lower == "afterend")
        return 3;
    return -1;
}

result_t XmlElement::insertAdjacentElement(exlib::string position, XmlElement_base* element, obj_ptr<XmlElement_base>& retVal)
{
    int32_t pos = parsePosition(position);
    if (pos < 0)
        return CHECK_ERROR(Runtime::setError("XmlElement: Invalid position argument"));

    obj_ptr<XmlNode_base> result;
    result_t hr;

    switch (pos) {
    case 0: // beforebegin - insert before this element
        if (!m_parent)
            return CALL_RETURN_NULL;
        hr = m_parent->m_childs->insertBefore(element, this, result);
        break;
    case 1: // afterbegin - insert as first child
        {
            obj_ptr<XmlNode_base> firstChild;
            m_childs->firstChild(firstChild);
            if (firstChild)
                hr = m_childs->insertBefore(element, firstChild, result);
            else
                hr = m_childs->appendChild(element, result);
        }
        break;
    case 2: // beforeend - insert as last child
        hr = m_childs->appendChild(element, result);
        break;
    case 3: // afterend - insert after this element
        if (!m_parent)
            return CALL_RETURN_NULL;
        hr = m_parent->m_childs->insertAfter(element, this, result);
        break;
    }

    if (hr < 0)
        return hr;

    retVal = element;
    return 0;
}

result_t XmlElement::insertAdjacentHTML(exlib::string position, exlib::string html)
{
    int32_t pos = parsePosition(position);
    if (pos < 0)
        return CHECK_ERROR(Runtime::setError("XmlElement: Invalid position argument"));

    if (html.empty())
        return 0;

    result_t hr;
    obj_ptr<XmlNode_base> result;

    if (m_isXml) {
        // For XML documents, wrap content in a root element for parsing
        exlib::string wrappedContent = "<root>" + html + "</root>";
        obj_ptr<XmlDocument> doc = new XmlDocument(true);
        hr = doc->load(wrappedContent);
        if (hr < 0)
            return hr;

        obj_ptr<XmlElement_base> docElement;
        hr = doc->get_documentElement(docElement);
        if (hr < 0 || !docElement)
            return hr;

        // Track the reference node for afterend position
        XmlNode_base* refNode = this;

        obj_ptr<XmlNode_base> node;
        while (docElement->get_firstChild(node) == 0) {
            switch (pos) {
            case 0: // beforebegin
                if (!m_parent)
                    continue;
                hr = m_parent->m_childs->insertBefore(node, this, result);
                break;
            case 1: // afterbegin
                {
                    obj_ptr<XmlNode_base> firstChild;
                    m_childs->firstChild(firstChild);
                    if (firstChild)
                        hr = m_childs->insertBefore(node, firstChild, result);
                    else
                        hr = m_childs->appendChild(node, result);
                }
                break;
            case 2: // beforeend
                hr = m_childs->appendChild(node, result);
                break;
            case 3: // afterend
                if (!m_parent)
                    continue;
                hr = m_parent->m_childs->insertAfter(node, refNode, result);
                if (hr >= 0)
                    refNode = node;
                break;
            }
            if (hr < 0)
                return hr;
        }
    } else {
        // For HTML documents, parse as HTML
        obj_ptr<XmlDocument> doc = new XmlDocument(false);
        hr = doc->load(html);
        if (hr < 0)
            return hr;

        obj_ptr<XmlElement_base> body;
        hr = doc->get_body(body);
        if (hr < 0 || !body)
            return hr;

        // Track the reference node for afterend position
        XmlNode_base* refNode = this;

        obj_ptr<XmlNode_base> node;
        while (body->get_firstChild(node) == 0) {
            switch (pos) {
            case 0: // beforebegin
                if (!m_parent)
                    continue;
                hr = m_parent->m_childs->insertBefore(node, this, result);
                break;
            case 1: // afterbegin
                {
                    obj_ptr<XmlNode_base> firstChild;
                    m_childs->firstChild(firstChild);
                    if (firstChild)
                        hr = m_childs->insertBefore(node, firstChild, result);
                    else
                        hr = m_childs->appendChild(node, result);
                }
                break;
            case 2: // beforeend
                hr = m_childs->appendChild(node, result);
                break;
            case 3: // afterend
                if (!m_parent)
                    continue;
                hr = m_parent->m_childs->insertAfter(node, refNode, result);
                if (hr >= 0)
                    refNode = node;
                break;
            }
            if (hr < 0)
                return hr;
        }
    }

    return 0;
}

result_t XmlElement::insertAdjacentText(exlib::string position, exlib::string text)
{
    int32_t pos = parsePosition(position);
    if (pos < 0)
        return CHECK_ERROR(Runtime::setError("XmlElement: Invalid position argument"));

    // Create text node
    obj_ptr<XmlText_base> textNode;
    m_document->createTextNode(text, textNode);

    obj_ptr<XmlNode_base> result;
    result_t hr;

    switch (pos) {
    case 0: // beforebegin
        if (!m_parent)
            return CALL_RETURN_NULL;
        hr = m_parent->m_childs->insertBefore(textNode, this, result);
        break;
    case 1: // afterbegin
        {
            obj_ptr<XmlNode_base> firstChild;
            m_childs->firstChild(firstChild);
            if (firstChild)
                hr = m_childs->insertBefore(textNode, firstChild, result);
            else
                hr = m_childs->appendChild(textNode, result);
        }
        break;
    case 2: // beforeend
        hr = m_childs->appendChild(textNode, result);
        break;
    case 3: // afterend
        if (!m_parent)
            return CALL_RETURN_NULL;
        hr = m_parent->m_childs->insertAfter(textNode, this, result);
        break;
    }

    return hr;
}

result_t XmlElement::toggleAttribute(exlib::string name, bool& retVal)
{
    // In HTML mode, normalize attribute names to lowercase
    if (!m_isXml)
        exlib::qstrlwr(name);

    bool has;
    result_t hr = hasAttribute(name, has);
    if (hr < 0)
        return hr;

    if (has) {
        // Attribute exists - remove it
        hr = removeAttribute(name);
        if (hr < 0)
            return hr;
        retVal = false;
    } else {
        // Attribute does not exist - add it with empty value
        hr = setAttribute(name, "");
        if (hr < 0)
            return hr;
        retVal = true;
    }

    return 0;
}

result_t XmlElement::toggleAttribute(exlib::string name, bool force, bool& retVal)
{
    // In HTML mode, normalize attribute names to lowercase
    if (!m_isXml)
        exlib::qstrlwr(name);

    bool has;
    result_t hr = hasAttribute(name, has);
    if (hr < 0)
        return hr;

    if (force) {
        // force is true - ensure attribute exists
        if (!has) {
            hr = setAttribute(name, "");
            if (hr < 0)
                return hr;
        }
        retVal = true;
    } else {
        // force is false - ensure attribute does not exist
        if (has) {
            hr = removeAttribute(name);
            if (hr < 0)
                return hr;
        }
        retVal = false;
    }

    return 0;
}
}
