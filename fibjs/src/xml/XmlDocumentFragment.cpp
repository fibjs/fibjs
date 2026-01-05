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

result_t XmlDocumentFragment::get_parentNode(obj_ptr<XmlNode_base>& retVal)
{
    // DocumentFragment never has a parent
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::get_previousSibling(obj_ptr<XmlNode_base>& retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocumentFragment::get_nextSibling(obj_ptr<XmlNode_base>& retVal)
{
    return CALL_RETURN_NULL;
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

} /* namespace fibjs */
