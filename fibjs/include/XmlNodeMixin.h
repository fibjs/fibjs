/*
 * XmlNodeMixin.h
 *
 *  Created on: Jan 5, 2026
 *      Author: lion
 *
 * CRTP mixin class that provides default implementations for XmlNode_base methods.
 * This eliminates the need for repetitive forwarding code in each XML node class.
 *
 * Usage:
 *   class XmlElement : public XmlNodeMixin<XmlElement, XmlElement_base> {
 *       // Only implement Element-specific methods
 *   };
 */

#pragma once

#include "XmlNodeImpl.h"

namespace fibjs {

template <typename Derived, typename BaseInterface>
class XmlNodeMixin : public BaseInterface, public XmlNodeImpl {
public:
    // Constructor with 2 arguments - automatically uses CRTP to get node pointer
    XmlNodeMixin(XmlDocument_base* document, int32_t type)
        : XmlNodeImpl(document, static_cast<Derived*>(this), type)
    {
    }

    // Constructor with explicit node pointer (for compatibility with existing code)
    XmlNodeMixin(XmlDocument_base* document, XmlNode_base* node, int32_t type)
        : XmlNodeImpl(document, node, type)
    {
    }

    XmlNodeMixin(const XmlNodeMixin& from)
        : XmlNodeImpl(from.m_document, static_cast<Derived*>(this), from.m_type)
    {
    }

public:
    // XmlNode_base forwarding methods - these delegate to XmlNodeImpl

    virtual result_t get_nodeType(int32_t& retVal) override
    {
        return XmlNodeImpl::get_nodeType(retVal);
    }

    virtual result_t get_parentNode(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_parentNode(retVal);
    }

    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base>& retVal) override
    {
        return XmlNodeImpl::get_childNodes(retVal);
    }

    virtual result_t get_children(obj_ptr<XmlNodeList_base>& retVal) override
    {
        return XmlNodeImpl::get_children(retVal);
    }

    virtual result_t get_firstChild(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->firstChild(retVal);
    }

    virtual result_t get_lastChild(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->lastChild(retVal);
    }

    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_previousSibling(retVal);
    }

    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_nextSibling(retVal);
    }

    virtual result_t get_firstElementChild(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_firstElementChild(retVal);
    }

    virtual result_t get_lastElementChild(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_lastElementChild(retVal);
    }

    virtual result_t get_previousElementSibling(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_previousElementSibling(retVal);
    }

    virtual result_t get_nextElementSibling(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::get_nextElementSibling(retVal);
    }

    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base>& retVal) override
    {
        return XmlNodeImpl::get_ownerDocument(retVal);
    }

    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal) override
    {
        return XmlNodeImpl::lookupPrefix(namespaceURI, retVal);
    }

    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal) override
    {
        return XmlNodeImpl::lookupNamespaceURI(prefix, retVal);
    }

    virtual result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
        obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->insertBefore(newChild, refChild, retVal);
    }

    virtual result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
        obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->insertAfter(newChild, refChild, retVal);
    }

    virtual result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
        obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->replaceChild(newChild, oldChild, retVal);
    }

    virtual result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->removeChild(oldChild, retVal);
    }

    virtual result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::m_childs->appendChild(newChild, retVal);
    }

    virtual result_t hasChildNodes(bool& retVal) override
    {
        return XmlNodeImpl::m_childs->hasChildNodes(retVal);
    }

    virtual result_t normalize() override
    {
        return XmlNodeImpl::m_childs->normalize();
    }

    virtual result_t remove(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::remove(retVal);
    }

    virtual result_t replaceWith(OptArgs nodes) override
    {
        return XmlNodeImpl::replaceWith(nodes);
    }

    virtual result_t before(OptArgs nodes) override
    {
        return XmlNodeImpl::before(nodes);
    }

    virtual result_t after(OptArgs nodes) override
    {
        return XmlNodeImpl::after(nodes);
    }

    virtual result_t contains(XmlNode_base* node, bool& retVal) override
    {
        return XmlNodeImpl::contains(node, retVal);
    }

    virtual result_t getRootNode(obj_ptr<XmlNode_base>& retVal) override
    {
        return XmlNodeImpl::getRootNode(retVal);
    }

    virtual result_t get_isConnected(bool& retVal) override
    {
        return XmlNodeImpl::get_isConnected(retVal);
    }

    virtual result_t compareDocumentPosition(XmlNode_base* other, int32_t& retVal) override
    {
        return XmlNodeImpl::compareDocumentPosition(other, retVal);
    }

    virtual result_t isEqualNode(XmlNode_base* other, bool& retVal) override
    {
        return XmlNodeImpl::isEqualNode(other, retVal);
    }

    virtual result_t isSameNode(XmlNode_base* other, bool& retVal) override
    {
        return XmlNodeImpl::isSameNode(other, retVal);
    }
};

} /* namespace fibjs */
