/*
 * XmlNodeList.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "XmlNodeList.h"
#include "XmlNodeImpl.h"

namespace fibjs
{

result_t XmlNodeList::get_length(int32_t &retVal)
{
    retVal = (int32_t)m_childs.size();
    return 0;
}

result_t XmlNodeList::item(int32_t index, obj_ptr<XmlNode_base> &retVal)
{
    if (index < 0 || index >= (int32_t)m_childs.size())
        return CALL_RETURN_NULL;
    retVal = m_childs[index]->m_node;
    return 0;
}

result_t XmlNodeList::_indexed_getter(uint32_t index, obj_ptr<XmlNode_base> &retVal)
{
    if (index >= m_childs.size())
        return CALL_RETURN_NULL;
    retVal = m_childs[index]->m_node;
    return 0;
}

void XmlNodeList::clean()
{
    bool isAlone = m_this == NULL;

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
        if (isAlone)
            m_childs[i]->m_node->Unref();
        else
            m_childs[i]->clearParent();

    m_childs.resize(0);

    m_this = NULL;
}

result_t XmlNodeList::firstChild(obj_ptr<XmlNode_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    if (!sz)
        return CALL_RETURN_NULL;

    retVal = m_childs[0]->m_node;
    return 0;
}

result_t XmlNodeList::lastChild(obj_ptr<XmlNode_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    if (!sz)
        return CALL_RETURN_NULL;

    retVal = m_childs[sz - 1]->m_node;
    return 0;
}

XmlNodeImpl *XmlNodeList::checkChild(XmlNode_base *child)
{
    static char s_child_rules[11][11] =
    {
        //  E  A  T  CS ER EN PI C  D  DT
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // NONE
        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0}, // ELEMENT_NODE
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // ATTRIBUTE_NODE
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // TEXT_NODE
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // CDATA_SECTION_NODE
        {0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0}, // ENTITY_REFERENCE_NODE
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // ENTITY_NODE
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // PROCESSING_INSTRUCTION_NODE
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // COMMENT_NODE
        {0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1}, // DOCUMENT_NODE
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // DOCUMENT_TYPE_NODE
    };

    XmlNodeImpl *node = XmlNodeImpl::fromNode(child);
    if (!node)
        return NULL;

    if (!s_child_rules[m_this->m_type][node->m_type])
        return NULL;

    return node;
}

bool XmlNodeList::checkNew(XmlNodeImpl *child)
{
    XmlNodeImpl *pThis = m_this;

    while (pThis)
    {
        if (pThis == child)
            return false;

        pThis = pThis->m_parent;
    }

    if (child->m_parent)
    {
        obj_ptr<XmlNode_base> tmp;
        child->m_parent->m_node->removeChild(child->m_node, tmp);
    }

    return true;
}

result_t XmlNodeList::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                                   obj_ptr<XmlNode_base> &retVal)
{
    XmlNodeImpl *pNew = checkChild(newChild);
    XmlNodeImpl *pRef = checkChild(refChild);
    if (!pNew || !pRef)
        return CALL_E_INVALIDARG;

    if (pRef->m_parent != m_this)
        return Runtime::setError("The node before which the new node is to be inserted is not a child of this node.");

    if (pNew == pRef)
    {
        retVal = newChild;
        return 0;
    }

    if (!checkNew(pNew))
        return Runtime::setError("The new child element contains the parent.");

    int32_t sz = (int32_t)m_childs.size();
    int32_t idx = pRef->m_index;
    int32_t i;

    m_childs.resize(sz + 1);

    for (i = sz; i > idx; i --)
    {
        XmlNodeImpl *pTmp = m_childs[i - 1];
        m_childs[i] = pTmp;
        pTmp->m_index ++;
    }

    pNew->setParent(m_this, idx);
    m_childs[idx] = pNew;

    retVal = newChild;
    return 0;
}

result_t XmlNodeList::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                                   obj_ptr<XmlNode_base> &retVal)
{
    XmlNodeImpl *pNew = checkChild(newChild);
    XmlNodeImpl *pOld = checkChild(oldChild);
    if (!pNew || !pOld)
        return CALL_E_INVALIDARG;

    if (pOld->m_parent != m_this)
        return Runtime::setError("The node to be replaced is not a child of this node.");

    if (pNew == pOld)
    {
        retVal = newChild;
        return 0;
    }

    if (!checkNew(pNew))
        return Runtime::setError("The new child element contains the parent.");

    int32_t idx = pOld->m_index;

    pOld->clearParent();

    pNew->setParent(m_this, idx);
    m_childs[idx] = pNew;

    retVal = oldChild;
    return 0;
}

result_t XmlNodeList::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    XmlNodeImpl *pOld = checkChild(oldChild);
    if (!pOld)
        return CALL_E_INVALIDARG;

    if (pOld->m_parent != m_this)
        return Runtime::setError("The node to be removed is not a child of this node.");

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = pOld->m_index; i < sz - 1; i ++)
    {
        XmlNodeImpl *pTmp = m_childs[i + 1];
        m_childs[i] = pTmp;
        pTmp->m_index --;
    }
    m_childs.resize(sz - 1);

    pOld->clearParent();

    retVal = oldChild;
    return 0;
}

result_t XmlNodeList::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    XmlNodeImpl *pNew = checkChild(newChild);
    if (!pNew)
        return CALL_E_INVALIDARG;

    if (!checkNew(pNew))
        return Runtime::setError("The new child element contains the parent.");

    pNew->setParent(m_this, (int32_t)m_childs.size());
    m_childs.push_back(pNew);

    retVal = newChild;
    return 0;
}

result_t XmlNodeList::hasChildNodes(bool &retVal)
{
    retVal = !!m_childs.size();
    return 0;
}

}
