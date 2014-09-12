/*
 * XmlNodeList.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlNodeList.h"
#include "ifs/XmlDocument.h"

#ifndef XMLNODELIST_H_
#define XMLNODELIST_H_

namespace fibjs
{

class XmlNodeImpl;

class XmlNodeList: public XmlNodeList_base
{
public:
    XmlNodeList(XmlNodeImpl *pThis) : m_this(pThis)
    {}

public:
    // XmlNodeList_base
    virtual result_t get_length(int32_t &retVal);
    virtual result_t item(int32_t index, obj_ptr<XmlNode_base> &retVal);
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlNode_base> &retVal);
    virtual result_t _indexed_setter(uint32_t index, XmlNode_base *newVal);

public:
    result_t firstChild(obj_ptr<XmlNode_base> &retVal);
    result_t lastChild(obj_ptr<XmlNode_base> &retVal);
    result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    result_t hasChildNodes(bool &retVal);

public:
    void clean()
    {
        m_this = NULL;
    }

private:
    XmlNodeImpl *checkChild(XmlNode_base *child);

public:
    XmlNodeImpl *m_this;

private:
    std::vector<XmlNodeImpl *> m_childs;
};

} /* namespace fibjs */
#endif /* XMLNODELIST_H_ */
