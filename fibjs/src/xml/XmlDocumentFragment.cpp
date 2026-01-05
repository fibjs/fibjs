/*
 * XmlDocumentFragment.cpp
 *
 *  Created on: Jan 5, 2026
 *      Author: lion
 */

#include "object.h"
#include "XmlDocumentFragment.h"
#include "XmlDocument.h"
#include "XmlElement.h"
#include "XmlText.h"
#include "StringBuffer.h"

namespace fibjs {

result_t XmlDocumentFragment::toString(exlib::string& retVal)
{
    return m_childs->toString(retVal);
}

result_t XmlDocumentFragment::get_nodeType(int32_t& retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlDocumentFragment::get_nodeName(exlib::string& retVal)
{
    retVal = "#document-fragment";
    return 0;
}

result_t XmlDocumentFragment::get_nodeValue(exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::set_nodeValue(exlib::string newVal)
{
    return 0;
}

result_t XmlDocumentFragment::get_ownerDocument(obj_ptr<XmlDocument_base>& retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlDocumentFragment::get_parentNode(obj_ptr<XmlNode_base>& retVal)
{
    // DocumentFragment never has a parent
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::hasChildNodes(bool& retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlDocumentFragment::get_childNodes(obj_ptr<XmlNodeList_base>& retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlDocumentFragment::get_children(obj_ptr<XmlNodeList_base>& retVal)
{
    return XmlNodeImpl::get_children(retVal);
}

result_t XmlDocumentFragment::get_firstChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlDocumentFragment::get_lastChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlDocumentFragment::get_previousSibling(obj_ptr<XmlNode_base>& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::get_nextSibling(obj_ptr<XmlNode_base>& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::get_firstElementChild(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_firstElementChild(retVal);
}

result_t XmlDocumentFragment::get_lastElementChild(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_lastElementChild(retVal);
}

result_t XmlDocumentFragment::get_previousElementSibling(obj_ptr<XmlNode_base>& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::get_nextElementSibling(obj_ptr<XmlNode_base>& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::get_textContent(exlib::string& retVal)
{
    StringBuffer strs;
    int32_t sz = (int32_t)m_childs->m_childs.size();

    for (int32_t i = 0; i < sz; i++) {
        exlib::string str;
        m_childs->m_childs[i]->m_node->get_textContent(str);
        strs.append(str);
    }

    retVal = strs.str();
    return 0;
}

result_t XmlDocumentFragment::set_textContent(exlib::string newVal)
{
    m_childs->removeAll();

    if (!newVal.empty()) {
        obj_ptr<XmlText> txt = new XmlText(m_document, newVal);
        obj_ptr<XmlNode_base> out;
        appendChild(txt, out);
    }

    return 0;
}

result_t XmlDocumentFragment::normalize()
{
    return m_childs->normalize();
}

result_t XmlDocumentFragment::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlDocumentFragment> frag = new XmlDocumentFragment(*this);
    return XmlNodeImpl::cloneNode(frag, deep, retVal);
}

result_t XmlDocumentFragment::lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlDocumentFragment::insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlDocumentFragment::appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlDocumentFragment::replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlDocumentFragment::removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlDocumentFragment::remove(obj_ptr<XmlNode_base>& retVal)
{
    // DocumentFragment cannot be removed from a parent since it never has one
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::replaceWith(OptArgs nodes)
{
    // DocumentFragment cannot be replaced since it never has a parent
    return 0;
}

result_t XmlDocumentFragment::before(OptArgs nodes)
{
    // DocumentFragment cannot have siblings since it never has a parent
    return 0;
}

result_t XmlDocumentFragment::after(OptArgs nodes)
{
    // DocumentFragment cannot have siblings since it never has a parent
    return 0;
}

result_t XmlDocumentFragment::contains(XmlNode_base* node, bool& retVal)
{
    return XmlNodeImpl::contains(node, retVal);
}

result_t XmlDocumentFragment::getRootNode(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::getRootNode(retVal);
}

result_t XmlDocumentFragment::get_isConnected(bool& retVal)
{
    return XmlNodeImpl::get_isConnected(retVal);
}

result_t XmlDocumentFragment::compareDocumentPosition(XmlNode_base* other, int32_t& retVal)
{
    return XmlNodeImpl::compareDocumentPosition(other, retVal);
}

result_t XmlDocumentFragment::isEqualNode(XmlNode_base* other, bool& retVal)
{
    return XmlNodeImpl::isEqualNode(other, retVal);
}

result_t XmlDocumentFragment::isSameNode(XmlNode_base* other, bool& retVal)
{
    return XmlNodeImpl::isSameNode(other, retVal);
}

} /* namespace fibjs */
