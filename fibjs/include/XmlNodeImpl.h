/*
 * XmlNodeImpl.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/xml.h"
#include "ifs/XmlElement.h"
#include "XmlNodeList.h"

namespace fibjs {

class XmlDocument;

class XmlNodeImpl {
public:
    XmlNodeImpl(XmlDocument_base* document, XmlNode_base* node, int32_t type)
        : m_childs(new XmlNodeList(this))
        , m_document(document)
        , m_node(node)
        , m_type(type)
        , m_parent(NULL)
        , m_index(-1)
    {
    }

    ~XmlNodeImpl()
    {
        m_childs->clean();
    }

public:
    result_t get_nodeType(int32_t& retVal)
    {
        retVal = m_type;
        return 0;
    }

    result_t get_parentNode(obj_ptr<XmlNode_base>& retVal)
    {
        if (!m_parent) {
            // clear the out parameter: callers may loop on it
            // (while (p) { ...; get_parentNode(p); }) -- see T16
            retVal = NULL;
            return CALL_RETURN_NULL;
        }
        retVal = m_parent->m_node;
        return 0;
    }

    result_t get_parentElement(obj_ptr<XmlElement_base>& retVal)
    {
        if (!m_parent || m_parent->m_type != xml_base::C_ELEMENT_NODE) {
            retVal = NULL;
            return CALL_RETURN_NULL;
        }
        retVal = (XmlElement_base*)m_parent->m_node;
        return 0;
    }

    result_t get_childNodes(obj_ptr<XmlNodeList_base>& retVal)
    {
        retVal = m_childs;
        return 0;
    }

    result_t get_children(obj_ptr<XmlNodeList_base>& retVal)
    {
        return m_childs->get_children(retVal);
    }

    result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal)
    {
        if (!m_parent) {
            retVal = NULL;
            return CALL_RETURN_NULL;
        }
        return m_parent->m_childs->item(m_index - 1, retVal);
    }

    result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal)
    {
        if (!m_parent) {
            retVal = NULL;
            return CALL_RETURN_NULL;
        }
        return m_parent->m_childs->item(m_index + 1, retVal);
    }

    result_t get_firstElementChild(obj_ptr<XmlNode_base>& retVal)
    {
        return m_childs->find_element(-1, 1, retVal);
    }

    result_t get_lastElementChild(obj_ptr<XmlNode_base>& retVal)
    {
        return m_childs->find_element(m_childs->m_childs.size(), -1, retVal);
    }

    result_t get_previousElementSibling(obj_ptr<XmlNode_base>& retVal)
    {
        if (!m_parent)
            return CALL_RETURN_NULL;
        return m_parent->m_childs->find_element(m_index, -1, retVal);
    }

    result_t get_nextElementSibling(obj_ptr<XmlNode_base>& retVal)
    {
        if (!m_parent)
            return CALL_RETURN_NULL;
        return m_parent->m_childs->find_element(m_index, 1, retVal);
    }

    result_t get_ownerDocument(obj_ptr<XmlDocument_base>& retVal)
    {
        retVal = m_document;
        return !retVal ? CALL_RETURN_NULL : 0;
    }

    result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
    {
        if (!m_parent || m_parent->m_type == xml_base::C_DOCUMENT_NODE)
            return CALL_RETURN_NULL;
        return m_parent->m_node->lookupPrefix(namespaceURI, retVal);
    }

    result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
    {
        if (!m_parent || m_parent->m_type == xml_base::C_DOCUMENT_NODE)
            return CALL_RETURN_NULL;
        return m_parent->m_node->lookupNamespaceURI(prefix, retVal);
    }

    result_t cloneNode(XmlNode_base* to, bool deep, obj_ptr<XmlNode_base>& retVal)
    {
        if (deep) {
            result_t hr = m_childs->cloneChilds(to);
            if (hr < 0)
                return hr;
        }

        retVal = to;
        return 0;
    }

    static const char* s_nss[][2];
    static bool globalNamespaceURI(exlib::string prefix, exlib::string& retVal)
    {
        int32_t i;

        for (i = 0; s_nss[i][0]; i++)
            if (prefix == s_nss[i][0]) {
                retVal = s_nss[i][1];
                return true;
            }

        return false;
    }

    static bool globalPrefix(exlib::string namespaceURI, exlib::string& retVal)
    {
        int32_t i;

        for (i = 0; s_nss[i][1]; i++)
            if (namespaceURI == s_nss[i][1]) {
                retVal = s_nss[i][0];
                return true;
            }

        return false;
    }

public:
    static XmlNodeImpl* fromNode(XmlNode_base* pNode);

    void setDocument(XmlDocument_base* doc)
    {
        if (m_document != doc) {
            m_document = doc;

            int32_t sz = (int32_t)m_childs->m_childs.size();
            for (int32_t i = 0; i < sz; i++)
                m_childs->m_childs[i]->setDocument(doc);
        }
    }

    void setParent(XmlNodeImpl* parent, int32_t idx)
    {
        ex_assert(m_parent == 0);
        ex_assert(m_index == -1);

        setDocument(parent->m_document);

        m_parent = parent;
        m_index = idx;
        m_node->Ref();

        bumpQueryEpoch();
    }

    void clearParent()
    {
        ex_assert(m_parent != 0);
        ex_assert(m_index != -1);

        // bump before the Unref below: it may be the last reference and would
        // delete this node
        bumpQueryEpoch();

        m_parent = NULL;
        m_index = -1;
        m_node->Unref();
    }

    // Owning document as XmlDocument (the only XmlDocument_base implementation
    // in this module), or NULL when the node is detached or the document is
    // gone.  Defined in XmlNodeImpl.cpp: XmlDocument is not visible here.
    XmlDocument* document();

    // Invalidate the document-level query indexes (no-op when detached).
    // Every structural mutation calls this through setParent()/clearParent().
    void bumpQueryEpoch();

    result_t remove(obj_ptr<XmlNode_base>& retVal)
    {
        if (m_parent && m_parent->m_childs) {
            m_parent->m_childs->removeChild(m_node, retVal);
            return 0;
        }

        return CALL_RETURN_NULL;
    }

    result_t replaceWith(OptArgs nodes)
    {
        if (!m_parent || !m_parent->m_childs)
            return 0;

        // Insert all new nodes before the current node
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
                result_t hr = m_parent->m_childs->insertBefore(node, m_node, retVal);
                if (hr < 0)
                    return hr;
            }
        }

        // Remove the current node
        obj_ptr<XmlNode_base> retVal;
        m_parent->m_childs->removeChild(m_node, retVal);

        return 0;
    }

    result_t before(OptArgs nodes)
    {
        if (!m_parent || !m_parent->m_childs)
            return 0;

        // Insert all nodes before the current node
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
                result_t hr = m_parent->m_childs->insertBefore(node, m_node, retVal);
                if (hr < 0)
                    return hr;
            }
        }

        return 0;
    }

    result_t after(OptArgs nodes)
    {
        if (!m_parent || !m_parent->m_childs)
            return 0;

        // Insert all nodes after the current node
        // Track the reference node for subsequent insertions
        XmlNode_base* refNode = m_node;
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
                result_t hr = m_parent->m_childs->insertAfter(node, refNode, retVal);
                if (hr < 0)
                    return hr;
                // Update reference node for next insertion
                refNode = node;
            }
        }

        return 0;
    }

    result_t contains(XmlNode_base* node, bool& retVal)
    {
        retVal = false;

        if (!node)
            return 0;

        // Check if node is the same as this node
        if (node == m_node) {
            retVal = true;
            return 0;
        }

        // Check all descendants
        XmlNodeImpl* impl = fromNode(node);
        if (!impl)
            return 0;

        // Walk up the parent chain to see if we are an ancestor
        XmlNodeImpl* parent = impl->m_parent;
        while (parent) {
            if (parent->m_node == m_node) {
                retVal = true;
                return 0;
            }
            parent = parent->m_parent;
        }

        return 0;
    }

    result_t getRootNode(obj_ptr<XmlNode_base>& retVal)
    {
        XmlNodeImpl* root = this;
        while (root->m_parent)
            root = root->m_parent;

        retVal = root->m_node;
        return 0;
    }

    result_t get_isConnected(bool& retVal)
    {
        // A node is connected if it has an ownerDocument and is in the document tree
        // Walk up to root and check if it's a Document node
        XmlNodeImpl* root = this;
        while (root->m_parent)
            root = root->m_parent;

        retVal = (root->m_type == xml_base::C_DOCUMENT_NODE);
        return 0;
    }

    result_t compareDocumentPosition(XmlNode_base* other, int32_t& retVal)
    {
        // Position constants
        const int32_t DOCUMENT_POSITION_DISCONNECTED = 1;
        const int32_t DOCUMENT_POSITION_PRECEDING = 2;
        const int32_t DOCUMENT_POSITION_FOLLOWING = 4;
        const int32_t DOCUMENT_POSITION_CONTAINS = 8;
        const int32_t DOCUMENT_POSITION_CONTAINED_BY = 16;

        retVal = 0;

        if (!other) {
            retVal = DOCUMENT_POSITION_DISCONNECTED;
            return 0;
        }

        // Same node
        if (other == m_node) {
            retVal = 0;
            return 0;
        }

        XmlNodeImpl* otherImpl = fromNode(other);
        if (!otherImpl) {
            retVal = DOCUMENT_POSITION_DISCONNECTED;
            return 0;
        }

        // Check if nodes are in the same document
        obj_ptr<XmlNode_base> thisRoot, otherRoot;
        getRootNode(thisRoot);
        otherImpl->getRootNode(otherRoot);

        if (thisRoot != otherRoot) {
            retVal = DOCUMENT_POSITION_DISCONNECTED | DOCUMENT_POSITION_PRECEDING;
            return 0;
        }

        // Check if other contains this
        XmlNodeImpl* parent = m_parent;
        while (parent) {
            if (parent->m_node == other) {
                retVal = DOCUMENT_POSITION_CONTAINS | DOCUMENT_POSITION_PRECEDING;
                return 0;
            }
            parent = parent->m_parent;
        }

        // Check if this contains other
        parent = otherImpl->m_parent;
        while (parent) {
            if (parent->m_node == m_node) {
                retVal = DOCUMENT_POSITION_CONTAINED_BY | DOCUMENT_POSITION_FOLLOWING;
                return 0;
            }
            parent = parent->m_parent;
        }

        // Find common ancestor and compare position
        // Build ancestor chain for this node
        std::vector<XmlNodeImpl*> thisChain;
        XmlNodeImpl* node = this;
        while (node) {
            thisChain.push_back(node);
            node = node->m_parent;
        }

        // Build ancestor chain for other node
        std::vector<XmlNodeImpl*> otherChain;
        node = otherImpl;
        while (node) {
            otherChain.push_back(node);
            node = node->m_parent;
        }

        // Find common ancestor
        XmlNodeImpl* commonAncestor = nullptr;
        XmlNodeImpl* thisAncestor = nullptr;
        XmlNodeImpl* otherAncestor = nullptr;

        for (size_t i = 0; i < thisChain.size(); i++) {
            for (size_t j = 0; j < otherChain.size(); j++) {
                if (thisChain[i] == otherChain[j]) {
                    commonAncestor = thisChain[i];
                    if (i > 0)
                        thisAncestor = thisChain[i - 1];
                    if (j > 0)
                        otherAncestor = otherChain[j - 1];
                    break;
                }
            }
            if (commonAncestor)
                break;
        }

        if (!commonAncestor) {
            retVal = DOCUMENT_POSITION_DISCONNECTED;
            return 0;
        }

        // Compare positions under common ancestor
        if (thisAncestor && otherAncestor) {
            if (thisAncestor->m_index < otherAncestor->m_index)
                retVal = DOCUMENT_POSITION_FOLLOWING;
            else
                retVal = DOCUMENT_POSITION_PRECEDING;
        } else if (!thisAncestor) {
            // this is the common ancestor, so other is contained
            retVal = DOCUMENT_POSITION_CONTAINED_BY | DOCUMENT_POSITION_FOLLOWING;
        } else {
            // other is the common ancestor, so this is contained
            retVal = DOCUMENT_POSITION_CONTAINS | DOCUMENT_POSITION_PRECEDING;
        }

        return 0;
    }

    result_t isEqualNode(XmlNode_base* other, bool& retVal)
    {
        retVal = false;

        if (!other)
            return 0;

        // Check node type
        int32_t otherType;
        other->get_nodeType(otherType);
        if (otherType != m_type)
            return 0;

        // Check node name
        exlib::string thisName, otherName;
        m_node->get_nodeName(thisName);
        other->get_nodeName(otherName);
        if (thisName != otherName)
            return 0;

        // Check node value
        exlib::string thisValue, otherValue;
        m_node->get_nodeValue(thisValue);
        other->get_nodeValue(otherValue);
        if (thisValue != otherValue)
            return 0;

        // For Element nodes, compare attributes
        if (m_type == xml_base::C_ELEMENT_NODE) {
            obj_ptr<XmlNamedNodeMap_base> thisAttrs, otherAttrs;
            ((XmlElement_base*)m_node)->get_attributes(thisAttrs);
            ((XmlElement_base*)other)->get_attributes(otherAttrs);

            int32_t thisAttrLen, otherAttrLen;
            thisAttrs->get_length(thisAttrLen);
            otherAttrs->get_length(otherAttrLen);
            if (thisAttrLen != otherAttrLen)
                return 0;

            // Compare each attribute
            for (int32_t i = 0; i < thisAttrLen; i++) {
                obj_ptr<XmlAttr_base> thisAttr;
                thisAttrs->item(i, thisAttr);

                exlib::string attrName, thisAttrValue, otherAttrValue;
                thisAttr->get_name(attrName);
                thisAttr->get_value(thisAttrValue);

                // Get corresponding attribute from other element
                obj_ptr<XmlAttr_base> otherAttr;
                otherAttrs->getNamedItem(attrName, otherAttr);
                if (!otherAttr)
                    return 0;

                otherAttr->get_value(otherAttrValue);
                if (thisAttrValue != otherAttrValue)
                    return 0;
            }
        }

        // Check children count
        obj_ptr<XmlNodeList_base> thisChildren, otherChildren;
        m_node->get_childNodes(thisChildren);
        other->get_childNodes(otherChildren);

        int32_t thisLen, otherLen;
        thisChildren->get_length(thisLen);
        otherChildren->get_length(otherLen);
        if (thisLen != otherLen)
            return 0;

        // Compare children recursively
        for (int32_t i = 0; i < thisLen; i++) {
            obj_ptr<XmlNode_base> thisChild, otherChild;
            thisChildren->item(i, thisChild);
            otherChildren->item(i, otherChild);

            bool childEqual;
            XmlNodeImpl* thisImpl = fromNode(thisChild);
            if (!thisImpl)
                return 0;
            thisImpl->isEqualNode(otherChild, childEqual);
            if (!childEqual)
                return 0;
        }

        retVal = true;
        return 0;
    }

    result_t isSameNode(XmlNode_base* other, bool& retVal)
    {
        retVal = (other == m_node);
        return 0;
    }

public:
    obj_ptr<XmlNodeList> m_childs;
    weak_ptr<XmlDocument_base> m_document;
    XmlNode_base* m_node;
    int32_t m_type;
    XmlNodeImpl* m_parent;
    int32_t m_index;
};

// Iterative pre-order (document order) walk over an element subtree.
//
// The recursive query implementations used ~2 C++ frames per tree level and
// overflowed the stack on deep documents (2500 levels already crash a release
// build), killing the process with SIGSEGV instead of raising a JS error.  This
// walker keeps the exact same order -- a node is visited before its children,
// children in vector order -- using an explicit stack.
//
// The stack holds one frame per *level* (node + child cursor), not one entry
// per pending sibling, so a document with millions of siblings in one parent
// costs O(depth) memory instead of O(width).
//
// includeSelf: visit `root` itself first (document-level queries), or only its
//              element descendants (element-level queries, per CSS spec).
// fn:          called for every element node; return true to stop the walk
//              early (used by querySelector / getFirstElementsByTagName).
template <typename Fn>
void walkElements(XmlNodeImpl* root, bool includeSelf, Fn&& fn)
{
    struct Frame {
        XmlNodeImpl* node;
        size_t index;
    };

    std::vector<Frame> stack;

    if (includeSelf && fn(root))
        return;

    stack.push_back(Frame { root, 0 });

    while (!stack.empty()) {
        Frame& frame = stack.back();
        std::vector<XmlNodeImpl*>& childs = frame.node->m_childs->m_childs;

        if (frame.index >= childs.size()) {
            stack.pop_back();
            continue;
        }

        XmlNodeImpl* child = childs[frame.index++];
        if (child->m_type != xml_base::C_ELEMENT_NODE)
            continue;

        if (fn(child))
            return;

        stack.push_back(Frame { child, 0 });
    }
}

} /* namespace fibjs */
