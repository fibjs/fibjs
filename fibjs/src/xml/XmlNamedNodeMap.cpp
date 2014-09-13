/*
 * XmlNamedNodeMap.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: lion
 */

#include "XmlNamedNodeMap.h"
#include "XmlNodeImpl.h"

namespace fibjs
{

XmlNamedNodeMap::~XmlNamedNodeMap()
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
        m_childs[i]->m_node->Unref();

    m_childs.resize(0);
}

result_t XmlNamedNodeMap::get_length(int32_t &retVal)
{
    retVal = (int32_t)m_childs.size();
    return 0;
}

result_t XmlNamedNodeMap::item(int32_t index, obj_ptr<XmlNode_base> &retVal)
{
    if (index < 0 || index >= (int32_t)m_childs.size())
        return CALL_RETURN_NULL;
    retVal = m_childs[index]->m_node;
    return 0;
}

result_t XmlNamedNodeMap::_indexed_getter(uint32_t index, obj_ptr<XmlNode_base> &retVal)
{
    if (index >= m_childs.size())
        return CALL_RETURN_NULL;
    retVal = m_childs[index]->m_node;
    return 0;
}

result_t XmlNamedNodeMap::getNamedItem(const char *name, obj_ptr<XmlNode_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        std::string tagName;
        XmlNode_base *node = m_childs[i]->m_node;
        node->get_nodeName(tagName);

        if (!qstrcmp(tagName.c_str(), name))
        {
            retVal = node;
            return 0;
        }
    }

    return CALL_RETURN_NULL;
}

result_t XmlNamedNodeMap::removeNamedItem(const char *name, obj_ptr<XmlNode_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        std::string tagName;
        XmlNode_base *node = m_childs[i]->m_node;
        node->get_nodeName(tagName);

        if (!qstrcmp(tagName.c_str(), name))
        {
            retVal = node;
            node->Unref();

            while (i < sz - 1)
            {
                m_childs[i] = m_childs[i + 1];
                i ++;
            }

            m_childs.resize(sz - 1);

            return 0;
        }
    }

    return CALL_RETURN_NULL;
}

result_t XmlNamedNodeMap::setAttributeNode(XmlNode_base *newNode, obj_ptr<XmlNode_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;
    XmlNodeImpl *pNew = XmlNodeImpl::fromNode(newNode);
    std::string name;

    newNode->get_nodeName(name);

    for (i = 0; i < sz; i ++)
    {
        std::string tagName;
        XmlNode_base *node = m_childs[i]->m_node;
        node->get_nodeName(tagName);

        if (!qstrcmp(tagName.c_str(), name.c_str()))
        {
            retVal = node;
            node->Unref();

            m_childs[i] = pNew;
            pNew->m_node->Ref();

            return 0;
        }
    }

    m_childs.push_back(pNew);
    pNew->m_node->Ref();

    return CALL_RETURN_NULL;
}

result_t XmlNamedNodeMap::hasAttribute(bool &retVal)
{
    retVal = !!m_childs.size();
    return 0;
}

}
