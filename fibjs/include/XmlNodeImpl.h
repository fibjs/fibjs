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
    XmlNodeImpl(XmlDocument_base *document, XmlNode_base *node, int32_t type) :
        m_childs(new XmlNodeList(this)),
        m_document(document), m_node(node), m_type(type), m_parent(NULL), m_index(-1)
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
        retVal = m_parent->m_node;
        return 0;
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

    result_t lookupPrefix(exlib::string namespaceURI, exlib::string &retVal)
    {
        if (!m_parent || m_parent->m_type == xml_base::_DOCUMENT_NODE)
            return CALL_RETURN_NULL;
        return m_parent->m_node->lookupPrefix(namespaceURI, retVal);
    }

    result_t lookupNamespaceURI(exlib::string prefix, exlib::string &retVal)
    {
        if (!m_parent || m_parent->m_type == xml_base::_DOCUMENT_NODE)
            return CALL_RETURN_NULL;
        return m_parent->m_node->lookupNamespaceURI(prefix, retVal);
    }

    result_t cloneNode(XmlNode_base *to, bool deep, obj_ptr<XmlNode_base> &retVal)
    {
        if (deep)
        {
            result_t hr = m_childs->cloneChilds(to);
            if (hr < 0)
                return hr;
        }

        retVal = to;
        return 0;
    }

    static const char *s_nss[][2];
    static bool globalNamespaceURI(exlib::string prefix, exlib::string &retVal)
    {
        int32_t i;

        for (i = 0; s_nss[i][0]; i ++)
            if (prefix == s_nss[i][0])
            {
                retVal = s_nss[i][1];
                return true;
            }

        return false;
    }

    static bool globalPrefix(exlib::string namespaceURI, exlib::string &retVal)
    {
        int32_t i;

        for (i = 0; s_nss[i][1]; i ++)
            if (namespaceURI == s_nss[i][1])
            {
                retVal = s_nss[i][0];
                return true;
            }

        return false;
    }

public:
    static XmlNodeImpl *fromNode(XmlNode_base *pNode);

    void setParent(XmlNodeImpl *parent, int32_t idx)
    {
        m_document = parent->m_document;
        m_parent = parent;
        m_index = idx;
        m_node->Ref();
    }

    void clearParent()
    {
        m_parent = NULL;
        m_index = -1;
        m_node->Unref();
    }

public:
    obj_ptr<XmlNodeList> m_childs;
    weak_ptr<XmlDocument_base> m_document;
    XmlNode_base *m_node;
    int32_t m_type;
    XmlNodeImpl *m_parent;
    int32_t m_index;
};

} /* namespace fibjs */
#endif /* XMLNODEIMPL_H_ */
