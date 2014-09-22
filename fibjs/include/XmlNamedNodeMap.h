/*
 * XmlNamedNodeMap.h
 *
 *  Created on: Sep 13, 2014
 *      Author: lion
 */

#include "ifs/XmlNamedNodeMap.h"
#include "XmlAttr.h"

#ifndef XMLNAMEDNODEMAP_H_
#define XMLNAMEDNODEMAP_H_

namespace fibjs
{

class XmlNamedNodeMap: public XmlNamedNodeMap_base
{
public:
    ~XmlNamedNodeMap()
    {
        int32_t sz = (int32_t)m_childs.size();
        int32_t i;

        for (i = 0; i < sz; i ++)
            m_childs[i]->m_owner = NULL;
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlNamedNodeMap_base
    virtual result_t get_length(int32_t &retVal);
    virtual result_t item(int32_t index, obj_ptr<XmlAttr_base> &retVal);
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlAttr_base> &retVal);
    virtual result_t getNamedItem(const char *name, obj_ptr<XmlAttr_base> &retVal);

public:
    result_t setNamedItem(XmlAttr *newAttr);
    result_t removeNamedItem(const char *name);
    result_t getNamedItemNS(const char *namespaceURI, const char *localName, obj_ptr<XmlAttr_base> &retVal);
    result_t removeNamedItemNS(const char *namespaceURI, const char *localName);
    result_t cloneAttrs(XmlNamedNodeMap *to, XmlNodeImpl *el);
    result_t lookupPrefix(const char *namespaceURI, std::string &retVal);
    result_t lookupNamespaceURI(const char *prefix, std::string &retVal);

public:
    std::vector<obj_ptr<XmlAttr> > m_childs;
};

} /* namespace fibjs */
#endif /* XMLNAMEDNODEMAP_H_ */
