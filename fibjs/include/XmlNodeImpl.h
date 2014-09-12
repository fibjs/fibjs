/*
 * XmlNodeImpl.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlNodeList.h"

#ifndef XMLNODEIMPL_H_
#define XMLNODEIMPL_H_

namespace fibjs
{

class XmlNodeImpl
{
public:
    XmlNodeImpl(XmlDocument_base *document, int32_t type) :
        m_childs(new XmlNodeList(this)),
        m_document(document), m_type(type), m_parent(NULL), m_index(-1)
    {}

    ~XmlNodeImpl()
    {
        m_childs->clean();
    }

public:
    result_t get_nodeType(int32_t &retVal)
    {
        retVal = m_type;
        return 0;
    }

    result_t get_parentNode(obj_ptr<XmlNode_base> &retVal)
    {
        if (!m_parent)
            return CALL_RETURN_NULL;
        return m_parent->toNode(retVal);
    }

    result_t get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
    {
        retVal = m_childs;
        return 0;
    }

    result_t get_previousSibling(obj_ptr<XmlNode_base> &retVal)
    {
        if (!m_parent)
            return CALL_RETURN_NULL;
        return m_parent->m_childs->item(m_index - 1, retVal);
    }

    result_t get_nextSibling(obj_ptr<XmlNode_base> &retVal)
    {
        if (!m_parent)
            return CALL_RETURN_NULL;
        return m_parent->m_childs->item(m_index + 1, retVal);
    }

    result_t get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
    {
        retVal = m_document;
        return !retVal ? CALL_RETURN_NULL : 0;
    }

    XmlDocument_base *ownerDocument()
    {
        return m_document;
    }

    result_t cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal);
    result_t normalize();

public:
    result_t toNode(obj_ptr<XmlNode_base> &retVal);
    static XmlNodeImpl *fromNode(XmlNode_base *pNode);

    void setParent(XmlNodeImpl *parent, int32_t idx)
    {
        m_document = parent->m_document;
        m_parent = parent;
        m_index = idx;
    }

    void clearParent()
    {
        m_parent = NULL;
        m_index = -1;
    }

public:
    obj_ptr<XmlNodeList> m_childs;
    weak_ptr<XmlDocument_base> m_document;
    int32_t m_type;
    XmlNodeImpl *m_parent;
    int32_t m_index;
};

} /* namespace fibjs */
#endif /* XMLNODEIMPL_H_ */
