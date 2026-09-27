/*
 * XmlElement.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/XmlElement.h"
#include "ifs/XmlDocumentFragment.h"
#include "DOMTokenList.h"
#include "CSSStyleDeclaration.h"
#include "DOMStringMap.h"
#include "XmlNodeMixin.h"
#include "XmlNodeList.h"
#include "XmlNamedNodeMap.h"
#include "XmlTreeWalk.h"
#include "StringBuffer.h"
#include "parse.h"

namespace fibjs {

class XmlElement : public XmlNodeMixin<XmlElement, XmlElement_base> {
public:
    XmlElement(XmlDocument_base* document, exlib::string tagName, bool isXml)
        : XmlNodeMixin<XmlElement, XmlElement_base>(document, this, xml_base::C_ELEMENT_NODE)
        , m_isXml(isXml)
        , m_tagName(tagName)
        , m_localName(tagName)
        , m_attrs(new XmlNamedNodeMap(this))
    {
        if (!m_isXml)
            exlib::qstrupr(m_tagName);
    }

    XmlElement(XmlDocument_base* document, exlib::string namespaceURI, exlib::string qualifiedName, bool isXml)
        : XmlNodeMixin<XmlElement, XmlElement_base>(document, this, xml_base::C_ELEMENT_NODE)
        , m_isXml(isXml)
        , m_tagName(qualifiedName)
        , m_namespaceURI(namespaceURI)
        , m_attrs(new XmlNamedNodeMap(this))
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
            exlib::qstrupr(m_tagName);
    }

    XmlElement(const XmlElement& from)
        : XmlNodeMixin<XmlElement, XmlElement_base>(from.m_document, this, xml_base::C_ELEMENT_NODE)
        , m_isXml(from.m_isXml)
        , m_tagName(from.m_tagName)
        , m_localName(from.m_localName)
        , m_prefix(from.m_prefix)
        , m_namespaceURI(from.m_namespaceURI)
        , m_attrs(new XmlNamedNodeMap(this))
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);
    result_t toXmlString(exlib::string& retVal);

    // Markup of this element without / after its children.  Serialization is
    // driven by XmlNodeImpl::serializeTo(), which keeps an explicit stack: the
    // recursive form called child->toString() from XmlElement::toString(), i.e.
    // 2 C++ frames per tree level, and a 1200 level document killed the process
    // with SIGBUS instead of raising a JS error.
    // writeOpen() emits the complete markup of a childless element, so
    // writeClose() is only called for elements that have children.
    void writeOpen(exlib::string& out, bool xmlMode);
    void writeClose(exlib::string& out);

public:
    // XmlNode_base - custom implementations
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);

public:
    // XmlElement_base
    virtual result_t get_namespaceURI(exlib::string& retVal);
    virtual result_t get_prefix(exlib::string& retVal);
    virtual result_t set_prefix(exlib::string newVal);
    virtual result_t get_localName(exlib::string& retVal);
    virtual result_t get_tagName(exlib::string& retVal);
    virtual result_t get_id(exlib::string& retVal);
    virtual result_t set_id(exlib::string newVal);
    virtual result_t get_src(exlib::string& retVal);
    virtual result_t set_src(exlib::string newVal);
    virtual result_t get_alt(exlib::string& retVal);
    virtual result_t set_alt(exlib::string newVal);
    virtual result_t get_href(exlib::string& retVal);
    virtual result_t set_href(exlib::string newVal);
    virtual result_t get_title(exlib::string& retVal);
    virtual result_t set_title(exlib::string newVal);
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t set_name(exlib::string newVal);
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t set_type(exlib::string newVal);
    virtual result_t get_rel(exlib::string& retVal);
    virtual result_t set_rel(exlib::string newVal);
    virtual result_t get_target(exlib::string& retVal);
    virtual result_t set_target(exlib::string newVal);
    virtual result_t get_placeholder(exlib::string& retVal);
    virtual result_t set_placeholder(exlib::string newVal);
    virtual result_t get_innerHTML(exlib::string& retVal);
    virtual result_t set_innerHTML(exlib::string newVal);
    virtual result_t get_outerHTML(exlib::string& retVal);
    virtual result_t set_outerHTML(exlib::string newVal);
    virtual result_t get_className(exlib::string& retVal);
    virtual result_t set_className(exlib::string newVal);
    virtual result_t get_classList(obj_ptr<DOMTokenList_base>& retVal);
    virtual result_t get_style(obj_ptr<CSSStyleDeclaration_base>& retVal);
    virtual result_t get_dataset(obj_ptr<DOMStringMap_base>& retVal);
    virtual result_t get_content(obj_ptr<XmlDocumentFragment_base>& retVal);
    virtual result_t get_attributes(obj_ptr<XmlNamedNodeMap_base>& retVal);
    virtual result_t hasAttributes(bool& retVal);
    virtual result_t getAttribute(exlib::string name, exlib::string& retVal);
    virtual result_t getAttributeNS(exlib::string namespaceURI, exlib::string localName, exlib::string& retVal);
    virtual result_t getAttributeNode(exlib::string name, obj_ptr<XmlAttr_base>& retVal);
    virtual result_t getAttributeNodeNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlAttr_base>& retVal);
    virtual result_t setAttribute(exlib::string name, exlib::string value);
    virtual result_t setAttributeNS(exlib::string namespaceURI, exlib::string qualifiedName, exlib::string value);
    virtual result_t setAttributeNode(XmlAttr_base* attr, obj_ptr<XmlAttr_base>& retVal);
    virtual result_t removeAttribute(exlib::string name);
    virtual result_t removeAttributeNS(exlib::string namespaceURI, exlib::string localName);
    virtual result_t removeAttributeNode(XmlAttr_base* attr, obj_ptr<XmlAttr_base>& retVal);
    virtual result_t hasAttribute(exlib::string name, bool& retVal);
    virtual result_t hasAttributeNS(exlib::string namespaceURI, exlib::string localName, bool& retVal);
    virtual result_t getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementById(exlib::string id, obj_ptr<XmlElement_base>& retVal);
    virtual result_t getElementsByClassName(exlib::string className, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t querySelector(exlib::string selectors, obj_ptr<XmlElement_base>& retVal);
    virtual result_t querySelectorAll(exlib::string selectors, obj_ptr<XmlNodeList_base>& retVal);
    // Shared implementation of the two queries above.  includeRoot selects the
    // semantics of the query origin: a query started at a document must also
    // match the document element itself (doc.querySelector('html')), while an
    // element-level query searches descendants only (CSS spec).
    result_t querySelectorImpl(exlib::string selectors, bool includeRoot, obj_ptr<XmlElement_base>& retVal);
    result_t querySelectorAllImpl(exlib::string selectors, bool includeRoot, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t matches(exlib::string selectors, bool& retVal);
    virtual result_t closest(exlib::string selectors, obj_ptr<XmlElement_base>& retVal);
    virtual result_t append(OptArgs nodes);
    virtual result_t prepend(OptArgs nodes);
    virtual result_t replaceChildren(OptArgs nodes);
    virtual result_t insertAdjacentElement(exlib::string position, XmlElement_base* element, obj_ptr<XmlElement_base>& retVal);
    virtual result_t insertAdjacentHTML(exlib::string position, exlib::string html);
    virtual result_t insertAdjacentText(exlib::string position, exlib::string text);
    virtual result_t toggleAttribute(exlib::string name, bool& retVal);
    virtual result_t toggleAttribute(exlib::string name, bool force, bool& retVal);

public:
    // Default namespace in scope for this element (the parser uses it for
    // unprefixed tags, the serializer to decide whether an xmlns="..."
    // attribute can be omitted).  Iterative for the same reason as
    // lookupNamespaceURI(): the recursive form cost one C++ frame per ancestor
    // and a deep document overflowed the native stack.
    result_t get_defaultNamespace(exlib::string& def_ns)
    {
        XmlNodeImpl* p = this;

        while (p && p->m_type == xml_base::C_ELEMENT_NODE) {
            XmlElement* el = (XmlElement*)p->m_node;

            if (el->m_prefix.empty()) {
                def_ns = el->m_namespaceURI;
                return 0;
            }

            result_t hr = el->getAttribute("xmlns", def_ns);
            if (hr != CALL_RETURN_NULL)
                return 0;

            p = p->m_parent;
        }

        return 0;
    }

    // includeSelf: document-level queries start at the root element itself,
    // element-level ones only look at descendants.  All of these used to be
    // recursive (2 frames per level) and crashed the process on deep documents;
    // walkElements() keeps document order with an explicit stack.
    void getElementsByTagNameFromThis(exlib::string tagName, obj_ptr<XmlNodeList>& retVal)
    {
        getElementsByTagName(tagName, retVal, true);
    }

    void getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList>& retVal, bool includeSelf = false)
    {
        walkElements(this, includeSelf, [&](XmlNodeImpl* node) -> bool {
            XmlElement* pEl = static_cast<XmlElement*>(node);

            if (*tagName.c_str() == '*' || (pEl->m_isXml ? (pEl->m_tagName == tagName) : !qstricmp(pEl->m_tagName.c_str(), tagName.c_str())))
                retVal->appendRef(node);

            return false;
        });
    }

    result_t getFirstElementsByTagName(exlib::string tagName, obj_ptr<XmlElement_base>& retVal)
    {
        XmlElement* found = NULL;

        walkElements(this, true, [&](XmlNodeImpl* node) -> bool {
            XmlElement* pEl = static_cast<XmlElement*>(node);

            if (pEl->m_isXml ? (pEl->m_tagName == tagName) : !qstricmp(pEl->m_tagName.c_str(), tagName.c_str())) {
                found = pEl;
                return true;
            }

            return false;
        });

        if (found) {
            retVal = found;
            return 0;
        }

        return CALL_RETURN_NULL;
    }

    void getTextContent(StringBuffer& retVal)
    {
        // text content = every descendant text node in document order; text
        // nodes are pushed onto the same explicit stack as elements
        std::vector<XmlNodeImpl*> stack;
        stack.push_back(this);

        while (!stack.empty()) {
            XmlNodeImpl* node = stack.back();
            stack.pop_back();

            if (node->m_type == xml_base::C_TEXT_NODE) {
                exlib::string value;
                node->m_node->get_nodeValue(value);
                retVal.append(value);
                continue;
            }

            std::vector<XmlNodeImpl*>& childs = node->m_childs->m_childs;
            for (size_t i = childs.size(); i > 0; i--) {
                XmlNodeImpl* child = childs[i - 1];
                if (child->m_type == xml_base::C_ELEMENT_NODE || child->m_type == xml_base::C_TEXT_NODE)
                    stack.push_back(child);
            }
        }
    }

    void getElementsByTagNameNSFromThis(exlib::string namespaceURI, exlib::string localName,
        obj_ptr<XmlNodeList>& retVal)
    {
        getElementsByTagNameNS(namespaceURI, localName, retVal, true);
    }

    void getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName,
        obj_ptr<XmlNodeList>& retVal, bool includeSelf = false)
    {
        walkElements(this, includeSelf, [&](XmlNodeImpl* node) -> bool {
            XmlElement* pEl = static_cast<XmlElement*>(node);

            if ((*namespaceURI.c_str() == '*' || (pEl->m_namespaceURI == namespaceURI)) && (*localName.c_str() == '*' || (pEl->m_localName == localName)))
                retVal->appendRef(node);

            return false;
        });
    }

    result_t getElementByIdFromThis(exlib::string id, obj_ptr<XmlElement_base>& retVal)
    {
        return getElementByIdImpl(id, true, retVal);
    }

    // Pre-order (document order) search for the first element with a matching
    // id attribute; iterative, so deep documents do not overflow the stack.
    result_t getElementByIdImpl(exlib::string id, bool includeSelf, obj_ptr<XmlElement_base>& retVal)
    {
        XmlElement* found = NULL;

        walkElements(this, includeSelf, [&](XmlNodeImpl* node) -> bool {
            XmlElement* pEl = static_cast<XmlElement*>(node);
            exlib::string _id;

            pEl->get_id(_id);
            if (_id == id) {
                found = pEl;
                return true;
            }

            return false;
        });

        if (found) {
            retVal = found;
            return 0;
        }

        return CALL_RETURN_NULL;
    }

    // split a class attribute / query into whitespace separated tokens,
    // dropping duplicates: a repeated token must not be required twice
    static void parseClassNames(const exlib::string& className, std::vector<exlib::string>& classNames)
    {
        _parser p(className);
        exlib::string str;

        p.skipSpace();
        while (p.getWord(str)) {
            bool dup = false;

            for (size_t i = 0; i < classNames.size(); i++)
                if (classNames[i] == str) {
                    dup = true;
                    break;
                }

            if (!dup)
                classNames.push_back(str);

            p.skipSpace();
        }
    }

    // true when the class attribute contains every requested class token.
    // In-place scan: the previous implementation split the attribute into a
    // std::vector<exlib::string> for *every* element, which dominated
    // getElementsByClassName() on large documents.
    static bool hasClassNames(const exlib::string& className, std::vector<exlib::string>& classNames)
    {
        size_t need = classNames.size();
        if (need == 0)
            return false;

        if (className.empty())
            return false;

        const char* s = className.c_str();
        size_t len = className.length();
        size_t i = 0;
        size_t found = 0;

        while (i < len) {
            while (i < len && qisspace(s[i]))
                i++;

            size_t start = i;
            while (i < len && !qisspace(s[i]))
                i++;

            size_t tokenLen = i - start;
            if (tokenLen == 0)
                break;

            for (size_t k = 0; k < need; k++) {
                const exlib::string& want = classNames[k];
                if (want.length() == tokenLen && memcmp(want.c_str(), s + start, tokenLen) == 0) {
                    found++;
                    break;
                }
            }

            if (found == need)
                return true;
        }

        return found == need;
    }

    void getElementsByClassNameFromThis(std::vector<exlib::string>& classNames, obj_ptr<XmlNodeList>& retVal)
    {
        getElementsByClassName(classNames, retVal, true);
    }

    void getElementsByClassName(std::vector<exlib::string>& classNames, obj_ptr<XmlNodeList>& retVal, bool includeSelf = false)
    {
        walkElements(this, includeSelf, [&](XmlNodeImpl* node) -> bool {
            XmlElement* pEl = static_cast<XmlElement*>(node);
            exlib::string _class;

            pEl->get_className(_class);
            if (hasClassNames(_class, classNames))
                retVal->appendRef(node);

            return false;
        });
    }

    void fix_prefix(exlib::string namespaceURI, exlib::string& prefix);

public:
    bool m_isXml;

    // Raw tag name access without copying: get_tagName() copies the string and
    // (in HTML mode) upper-cases it on every call, which the selector matcher
    // and the document query index do not need.  m_tagName is already
    // normalised (upper-case for HTML) by the constructors.
    const exlib::string& tagNameRef() const
    {
        return m_tagName;
    }

    const exlib::string& localNameRef() const
    {
        return m_localName;
    }

    const exlib::string& namespaceURIRef() const
    {
        return m_namespaceURI;
    }

private:
    exlib::string m_tagName;
    exlib::string m_localName;
    exlib::string m_prefix;
    exlib::string m_namespaceURI;
    obj_ptr<XmlNamedNodeMap> m_attrs;
    obj_ptr<DOMTokenList_base> m_classList;
    obj_ptr<CSSStyleDeclaration_base> m_style; // cached CSSStyleDeclaration for the style attribute
    obj_ptr<DOMStringMap_base> m_dataset; // cached DOMStringMap for data-* attributes
    obj_ptr<XmlDocumentFragment_base> m_content; // cached content for <template> elements
};

} /* namespace fibjs */
