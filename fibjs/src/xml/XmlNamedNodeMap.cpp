/*
 * XmlNamedNodeMap.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: lion
 */

#include "XmlNamedNodeMap.h"
#include "XmlNodeImpl.h"
#include <string.h>
 
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

result_t XmlNamedNodeMap::toString(std::string &retVal)
{
    std::vector<std::string> strs;

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;
    int32_t len = 0, pos = 0;

    if (sz == 0)
    {
        retVal.clear();
        return 0;
    }
    else if (sz == 1)
        return m_childs[0]->m_node->toString(retVal);

    for (i = 0; i < sz; i ++)
    {
        std::string str;

        m_childs[i]->m_node->toString(str);
        len += (int32_t)str.length();
        strs.push_back(str);
    }

    retVal.resize(len);
    for (i = 0; i < sz; i ++)
    {
        int32_t l = (int32_t)strs[i].length();
        memcpy(&retVal[pos], strs[i].c_str(), l);
        pos += l;
    }

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

result_t XmlNamedNodeMap::cloneAttrs(XmlNamedNodeMap *to)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;
    result_t hr;

    for (i = 0; i < sz; i ++)
    {
        obj_ptr<XmlNode_base> child;
        obj_ptr<XmlNode_base> out;

        hr = m_childs[i]->m_node->cloneNode(true, child);
        if (hr < 0)
            return hr;

        hr = to->setAttributeNode(child, out);
        if (hr < 0)
            return hr;
    }

    return 0;
}

}
