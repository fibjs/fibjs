/*
 * XmlNamedNodeMap.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: lion
 */

#include "XmlNamedNodeMap.h"
#include "XmlNodeImpl.h"
#include <string.h>
#include "StringBuffer.h"

namespace fibjs
{

result_t XmlNamedNodeMap::get_length(int32_t &retVal)
{
    retVal = (int32_t)m_childs.size();
    return 0;
}

result_t XmlNamedNodeMap::item(int32_t index, obj_ptr<XmlAttr_base> &retVal)
{
    if (index < 0 || index >= (int32_t)m_childs.size())
        return CALL_RETURN_NULL;
    retVal = m_childs[index];
    return 0;
}

result_t XmlNamedNodeMap::_indexed_getter(uint32_t index, obj_ptr<XmlAttr_base> &retVal)
{
    if (index >= m_childs.size())
        return CALL_RETURN_NULL;
    retVal = m_childs[index];
    return 0;
}

result_t XmlNamedNodeMap::toString(std::string &retVal)
{
    StringBuffer strs;

    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    if (sz == 0)
    {
        retVal.clear();
        return 0;
    }

    for (i = 0; i < sz; i ++)
        m_childs[i]->fix_prefix();

    sz = (int32_t)m_childs.size();
    if (sz == 1)
        return m_childs[0]->toString(retVal);

    for (i = 0; i < sz; i ++)
    {
        std::string str;

        m_childs[i]->toString(str);
        strs.append(str);
    }

    retVal = strs.str();

    return 0;
}

result_t XmlNamedNodeMap::getNamedItem(const char *name, obj_ptr<XmlAttr_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check(name))
        {
            retVal = node;
            return 0;
        }
    }

    return CALL_RETURN_NULL;
}

result_t XmlNamedNodeMap::getNamedItemNS(const char *namespaceURI, const char *localName,
        obj_ptr<XmlAttr_base> &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check(namespaceURI, localName))
        {
            retVal = node;
            return 0;
        }
    }

    return CALL_RETURN_NULL;
}

result_t XmlNamedNodeMap::removeNamedItem(const char *name)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check(name))
        {
            node->m_owner = NULL;

            while (i < sz - 1)
            {
                m_childs[i] = m_childs[i + 1];
                i ++;
            }

            m_childs.resize(sz - 1);
            return 0;
        }
    }

    return 0;
}

result_t XmlNamedNodeMap::removeNamedItemNS(const char *namespaceURI, const char *localName)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check(namespaceURI, localName))
        {
            node->m_owner = NULL;

            while (i < sz - 1)
            {
                m_childs[i] = m_childs[i + 1];
                i ++;
            }

            m_childs.resize(sz - 1);
            return 0;
        }
    }

    return 0;
}

result_t XmlNamedNodeMap::setNamedItem(XmlAttr *newNode)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check(newNode))
        {
            node->m_owner = NULL;

            m_childs[i] = (XmlAttr *)newNode;
            return 0;
        }
    }

    m_childs.append((XmlAttr *)newNode);
    return 0;
}

result_t XmlNamedNodeMap::cloneAttrs(XmlNamedNodeMap *to, XmlNodeImpl *el)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;
    result_t hr;

    for (i = 0; i < sz; i ++)
    {
        obj_ptr<XmlAttr> child = new XmlAttr(*m_childs[i]);
        child->m_owner = (XmlElement *)el->m_node;
        hr = to->setNamedItem(child);
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t XmlNamedNodeMap::lookupPrefix(const char *namespaceURI, std::string &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check_namespaceURI(namespaceURI))
            return node->get_localName(retVal);
    }

    return CALL_RETURN_NULL;
}

result_t XmlNamedNodeMap::lookupNamespaceURI(const char *prefix, std::string &retVal)
{
    int32_t sz = (int32_t)m_childs.size();
    int32_t i;

    for (i = 0; i < sz; i ++)
    {
        XmlAttr *node = m_childs[i];
        if (node->check_prefix(prefix))
            return node->get_value(retVal);
    }

    return CALL_RETURN_NULL;
}


}
