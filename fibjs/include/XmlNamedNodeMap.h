/*
 * XmlNamedNodeMap.h
 *
 *  Created on: Sep 13, 2014
 *      Author: lion
 */

#include "ifs/XmlNamedNodeMap.h"

#ifndef XMLNAMEDNODEMAP_H_
#define XMLNAMEDNODEMAP_H_

namespace fibjs
{

class XmlNodeImpl;

class XmlNamedNodeMap: public XmlNamedNodeMap_base
{
public:
    XmlNamedNodeMap(XmlNodeImpl *pThis) : m_this(pThis)
    {}

    ~XmlNamedNodeMap();

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlNodeList_base
    virtual result_t get_length(int32_t &retVal);
    virtual result_t item(int32_t index, obj_ptr<XmlNode_base> &retVal);
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlNode_base> &retVal);

public:
    // XmlNamedNodeMap_base
    virtual result_t getNamedItem(const char *name, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeNamedItem(const char *name, obj_ptr<XmlNode_base> &retVal);

public:
    result_t setAttributeNode(XmlNode_base *newAttr, obj_ptr<XmlNode_base> &retVal);
    result_t hasAttribute(bool &retVal);

public:
    bool hasAttribute()
    {
        return !!m_childs.size();
    }

    result_t cloneAttrs(XmlNamedNodeMap *to);

public:
    XmlNodeImpl *m_this;
    std::vector<XmlNodeImpl *> m_childs;
};

} /* namespace fibjs */
#endif /* XMLNAMEDNODEMAP_H_ */
